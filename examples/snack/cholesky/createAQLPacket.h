void createSyrkAQLPacket(hsa_signal_t signal, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor, float* matrix, int n, int nb, int invocationID, int ordered);
void createGemmAQLPacket(hsa_signal_t signal, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor, float* matrix, int n, int nb, int invocationID, int ordered);
void createTrsmAQLPacket(hsa_signal_t signal, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor, float* matrix, int n, int nb, int invocationID, int ordered);
void createBarrierAQLPacket(hsa_barrier_packet_t* aql, int numDependency, hsa_signal_t* depSignal);
