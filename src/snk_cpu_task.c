/*

  Copyright (c) 2015 ADVANCED MICRO DEVICES, INC.  

  AMD is granting you permission to use this software and documentation (if any) (collectively, the 
  Materials) pursuant to the terms and conditions of the Software License Agreement included with the 
  Materials.  If you do not have a copy of the Software License Agreement, contact your AMD 
  representative for a copy.

  You agree that you will not reverse engineer or decompile the Materials, in whole or in part, except for 
  example code which is provided in source code form and as allowed by applicable law.

  WARRANTY DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
  KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT 
  LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
  PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE SOFTWARE WILL RUN UNINTERRUPTED OR ERROR-
  FREE OR WARRANTIES ARISING FROM CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK 
  ASSOCIATED WITH THE USE OF THE SOFTWARE IS ASSUMED BY YOU.  Some jurisdictions do not 
  allow the exclusion of implied warranties, so the above exclusion may not apply to You. 

  LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL NOT, 
  UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT, INCIDENTAL, 
  INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF THE SOFTWARE OR THIS 
  AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH 
  DAMAGES.  In no event shall AMD's total liability to You for all damages, losses, and 
  causes of action (whether in contract, tort (including negligence) or otherwise) 
  exceed the amount of $100 USD.  You agree to defend, indemnify and hold harmless 
  AMD and its licensors, and any of their directors, officers, employees, affiliates or 
  agents from and against any and all loss, damage, liability and other expenses 
  (including reasonable attorneys' fees), resulting from Your use of the Software or 
  violation of the terms and conditions of this Agreement.  

  U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED RIGHTS." 
  Use, duplication, or disclosure by the Government is subject to the restrictions as set 
  forth in FAR 52.227-14 and DFAR252.227-7013, et seq., or its successor.  Use of the 
  Materials by the Government constitutes acknowledgement of AMD's proprietary rights in them.

  EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as stated in the 
  Software License Agreement.

*/ 
/* This file contains logic for CPU tasking in SNACK */
#include "snk.h"
#include "snk_cpu_task.h"
#include <assert.h>

agent_t *agent;

hsa_signal_t worker_sig;

pthread_t agent_threads[SNK_MAX_CPU_STREAMS];

const cpu_kernel_table_t *_CN__CPU_kernels;

size_t numWorkers;

void set_cpu_kernel_table(const cpu_kernel_table_t *kernel_table) {
    _CN__CPU_kernels = kernel_table;
}

hsa_queue_t* get_cpu_queue(int id) {
    return agent->queues[id];
}

agent_t *get_cpu_q_agent() {
    return agent;
}

void signal_worker(int signal) {
    DEBUG_PRINT("Signaling work\n");
    hsa_signal_store_release(worker_sig, signal);
}

int is_barrier(uint16_t header) {
    return (header & (1 << HSA_PACKET_HEADER_BARRIER)) ? 1 : 0;
}

uint8_t get_packet_type(uint16_t header) {
    // FIXME: The width of packet type is 8 bits. Change to below line if width
    // changes
    //return (header >> HSA_PACKET_HEADER_TYPE) & ((1 << HSA_PACKET_HEADER_WIDTH_TYPE) - 1);
    return (header >> HSA_PACKET_HEADER_TYPE) & 0xFF;
}

int process_packet(hsa_queue_t *queue)
{
    DEBUG_PRINT("Processing Packet from CPU Queue\n");

    uint64_t read_index = hsa_queue_load_read_index_acquire(queue);
    assert(read_index == 0);
    hsa_signal_t doorbell = queue->doorbell_signal;
    /* FIXME: Handle queue overflows */
    while (read_index < queue->size) {
        while (hsa_signal_wait_acquire(doorbell, HSA_SIGNAL_CONDITION_GTE, read_index, UINT64_MAX,
                    HSA_WAIT_STATE_BLOCKED) < (hsa_signal_value_t) read_index);
        hsa_agent_dispatch_packet_t* packets = (hsa_agent_dispatch_packet_t*) queue->base_address;
        hsa_agent_dispatch_packet_t* packet = packets + read_index % queue->size;
        int i;
        switch (get_packet_type(packet->header)) {
            case HSA_PACKET_TYPE_BARRIER_OR: ;
                                             hsa_barrier_or_packet_t *barrier_or = (hsa_barrier_or_packet_t *)packet; 
                                             DEBUG_PRINT("Executing OR barrier\n");
                                             for (i = 0; i < 5; ++i) {
                                                 if (barrier_or->dep_signal[i].handle != 0) {
                                                     hsa_signal_wait_acquire(barrier_or->dep_signal[i], 
                                                             HSA_SIGNAL_CONDITION_EQ,
                                                             0, UINT64_MAX,
                                                             HSA_WAIT_STATE_BLOCKED);
                                                     break;
                                                 }
                                             }
                                             if (barrier_or->completion_signal.handle != 0) {
                                                 hsa_signal_subtract_release(barrier_or->completion_signal, 1);
                                             }
                                             packet_store_release((uint32_t*) barrier_or, create_header(HSA_PACKET_TYPE_INVALID, 0), HSA_PACKET_TYPE_BARRIER_OR);
                                             break;
            case HSA_PACKET_TYPE_BARRIER_AND: ;
                                              hsa_barrier_and_packet_t *barrier = (hsa_barrier_and_packet_t *)packet; 
                                              DEBUG_PRINT("Executing AND barrier\n");
                                              for (i = 0; i < 5; ++i) {
                                                  if (barrier->dep_signal[i].handle != 0) {
                                                      hsa_signal_wait_acquire(barrier->dep_signal[i], 
                                                              HSA_SIGNAL_CONDITION_EQ,
                                                              0, UINT64_MAX,
                                                              HSA_WAIT_STATE_BLOCKED);
                                                  }
                                              }
                                              if (barrier->completion_signal.handle != 0) {
                                                  hsa_signal_subtract_release(barrier->completion_signal, 1);
                                              }
                                              packet_store_release((uint32_t*) barrier, create_header(HSA_PACKET_TYPE_INVALID, 0), HSA_PACKET_TYPE_BARRIER_AND);
                                              break;
            case HSA_PACKET_TYPE_AGENT_DISPATCH: ;
                                                 int num_args = 0;
                                                 for(num_args = 0; num_args < 4; num_args++) { 
                                                     if(packet->arg[num_args] == UINT64_MAX) break;
                                                     //_CN__CPU_kernels[packet->type].ptrs[num_args] = packet->arg[num_args];
                                                 }
                                                 switch(num_args) {
                                                     case 0: ;
                                                             void (*function0) (void) =
                                                                 (void (*)(void)) _CN__CPU_kernels[packet->type].function.function0;
                                                             /*DEBUG_PRINT("Func Ptr: %p Args: NONE\n", 
                                                                     function0
                                                                     );*/
                                                             function0(
                                                                     );
                                                             break;
                                                     case 1: ;
                                                             void (*function1) (uint64_t) =
                                                                 (void (*)(uint64_t)) _CN__CPU_kernels[packet->type].function.function1;
                                                             /*DEBUG_PRINT("Args: %" PRIu64 "\n", 
                                                                     packet->arg[0]
                                                                     );*/
                                                             function1(
                                                                     packet->arg[0]
                                                                     );
                                                             break;
                                                     case 2: ;
                                                             void (*function2) (uint64_t, uint64_t) =
                                                                 (void (*)(uint64_t, uint64_t)) _CN__CPU_kernels[packet->type].function.function2;
                                                             /*DEBUG_PRINT("Args: %" PRIu64 " %" PRIu64 "\n", 
                                                                     packet->arg[0],
                                                                     packet->arg[1]
                                                                     );*/
                                                             function2(
                                                                     packet->arg[0],
                                                                     packet->arg[1]
                                                                     );
                                                             break;
                                                     case 3: ;
                                                             void (*function3) (uint64_t, uint64_t, uint64_t) =
                                                                 (void (*)(uint64_t, uint64_t, uint64_t)) _CN__CPU_kernels[packet->type].function.function3;
                                                             /*DEBUG_PRINT("Args: %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", 
                                                                     packet->arg[0],
                                                                     packet->arg[1],
                                                                     packet->arg[2]
                                                                     );*/
                                                             function3(
                                                                     packet->arg[0],
                                                                     packet->arg[1],
                                                                     packet->arg[2]
                                                                     );
                                                             break;
                                                     case 4: ;
                                                             void (*function4) (uint64_t, uint64_t, uint64_t, uint64_t) =
                                                                 (void (*)(uint64_t, uint64_t, uint64_t, uint64_t)) _CN__CPU_kernels[packet->type].function.function4;
                                                             /*DEBUG_PRINT("Args: %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", 
                                                                     packet->arg[0],
                                                                     packet->arg[1],
                                                                     packet->arg[2],
                                                                     packet->arg[3]
                                                                     );*/
                                                             function4(
                                                                     packet->arg[0],
                                                                     packet->arg[1],
                                                                     packet->arg[2],
                                                                     packet->arg[3]
                                                                     );
                                                             break;
                                                 }
                                                 if (packet->completion_signal.handle != 0) {
                                                     hsa_signal_subtract_release(packet->completion_signal, 1);
                                                 }
                                                 packet_store_release((uint32_t*) packet, create_header(HSA_PACKET_TYPE_INVALID, 0), packet->type);
                                                 break;
        }
        read_index++;
        hsa_queue_store_read_index_release(queue, read_index);
    }

    DEBUG_PRINT("Finished executing agent dispatch\n");

    // Finishing this task may free up more tasks, so issue the wakeup command
    DEBUG_PRINT("Signaling more work\n");
    hsa_signal_store_release(worker_sig, PROCESS_PKT);
    return 0;
}

void *agent_worker(void *agent_args)
{
    agent_t *agent = (agent_t *) agent_args;
    hsa_signal_value_t sig_value = IDLE;
    while (sig_value == IDLE) {
        DEBUG_PRINT("Worker thread sleeping\n");
        sig_value = hsa_signal_wait_acquire(worker_sig, HSA_SIGNAL_CONDITION_LT, IDLE,
                UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
        DEBUG_PRINT("Worker thread waking up\n");

        if (sig_value == FINISH) {
            DEBUG_PRINT("Worker thread received the EXIT SIGNAL\n");
            break;
        }

        if (PROCESS_PKT == hsa_signal_cas_acq_rel(worker_sig,
                    PROCESS_PKT, IDLE) ) {
            int i;
            for (i = 0; i < agent->num_queues; i++) {
                hsa_queue_t *queue = agent->queues[i];
                if (!process_packet(queue)) continue;
            }
        }
        sig_value = IDLE;
    }
    return NULL;
}

void
cpu_agent_init(hsa_agent_t cpu_agent, hsa_region_t cpu_region, 
                const size_t num_queues, const size_t capacity,
                const size_t num_workers) {
    hsa_status_t err;
    agent = (agent_t *)malloc(sizeof(agent_t));
    agent->num_queues = num_queues;
    agent->queues = (hsa_queue_t **)malloc(sizeof(hsa_queue_t*) * num_queues);
    uint32_t i;
    for (i = 0; i < num_queues; i++) {
        hsa_signal_t db_signal;
        err = hsa_signal_create(1, 0, NULL, &db_signal);
        check(Creating a HSA signal for agent dispatch db signal, err);

        err = hsa_soft_queue_create(cpu_region, capacity, HSA_QUEUE_TYPE_SINGLE,
                HSA_QUEUE_FEATURE_AGENT_DISPATCH, db_signal, &(agent->queues[i]));
        check(Creating an agent queue, err);

        hsa_queue_t *q = agent->queues[i];
        /* FIXME: Looks like a nasty HSA bug. The doorbell signal that we pass to the 
         * soft queue creation API never seems to be set. Workaround is to 
         * manually set it again like below.
         */
        q->doorbell_signal = db_signal;
    }

    // signal between the host thread and the CPU tasking queue thread
    err = hsa_signal_create(IDLE, 0, NULL, &worker_sig);
    check(Creating a HSA signal for agent dispatch worker threads, err);

    numWorkers = num_workers;
    DEBUG_PRINT("Spawning %zu CPU execution threads\n",
                 num_workers);

    for (i = 0; i < num_workers; i++) {
        pthread_create(&agent_threads[i], NULL, agent_worker, (void *)agent);
    }
} 

/* FIXME: When and who should call this cleanup funtion? */
void
agent_fini()
{
   #if 1
    DEBUG_PRINT("SIGNALING EXIT\n");
    hsa_signal_store_release(worker_sig, FINISH);

    /* wait for the other threads */
    uint32_t i;
    for (i = 0; i < numWorkers; i++) {
        pthread_join(agent_threads[i], NULL);
    }

    for (i = 0; i < agent->num_queues; i++) {
        hsa_queue_destroy(agent->queues[i]);
    }

    free(agent->queues);
    free(agent);
    #endif
    DEBUG_PRINT("agent_fini completed\n");
}