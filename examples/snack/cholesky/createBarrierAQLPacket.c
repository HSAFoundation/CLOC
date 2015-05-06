#include "choleskyTile.h"

extern hsa_agent_t device;
extern hsa_status_t err;

void packet_type_store_release(hsa_packet_header_t* header, hsa_packet_type_t type) {
    __atomic_store_n((uint8_t*)header, (uint8_t)type, __ATOMIC_RELEASE);
}

void createBarrierAQLPacket(hsa_barrier_packet_t* aql, int numDeps, hsa_signal_t* depSignal)
{
   aql->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;

   assert(numDeps < 6); // max dependency supporteed is 5 (or 6?)
   int signalIter;
   // Add dependencies
   for (signalIter = 0; signalIter < numDeps; signalIter++) {
       aql->dep_signal[signalIter] = depSignal[signalIter];
   }
   //packet_type_store_release(&aql->header, HSA_PACKET_TYPE_BARRIER);
   aql->header.type = HSA_PACKET_TYPE_BARRIER;
}
