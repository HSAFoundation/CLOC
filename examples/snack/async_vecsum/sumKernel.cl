#define INT_TYPE int
__kernel void sum8192Kernel(__global const INT_TYPE * x,  __global INT_TYPE * result) {
   __local INT_TYPE buffer[512];
   int gid=get_local_id(0);
   buffer[gid] = x[gid] + x[gid+512] + x[gid+1024] + x[gid+1536] +
                 x[gid+2048] + x[gid+2560] + x[gid+3072] + x[gid+3584] +
                 x[gid+4096] + x[gid+4608] + x[gid+5120] + x[gid+5632] +
                 x[gid+6144] + x[gid+6656] + x[gid+7168] + x[gid+7680] ;
   barrier(CLK_LOCAL_MEM_FENCE);
   if(gid<256)  buffer[gid] = buffer[gid]+buffer[gid+256]; 
   barrier(CLK_LOCAL_MEM_FENCE);
   if(gid<128)  buffer[gid] = buffer[gid]+buffer[gid+128]; 
   barrier(CLK_LOCAL_MEM_FENCE);
   if(gid<64)   buffer[gid] = buffer[gid]+buffer[gid+64];  
   barrier(CLK_LOCAL_MEM_FENCE);
   if(gid<32)   buffer[gid] = buffer[gid]+buffer[gid+32]; 
   if(gid<16)   buffer[gid] = buffer[gid]+buffer[gid+16];   
   if(gid<8)    buffer[gid] = buffer[gid]+buffer[gid+8]; 
   if(gid<4)    buffer[gid] = buffer[gid]+buffer[gid+4];  
   if(gid<2)    buffer[gid] = buffer[gid]+buffer[gid+2];
   if(gid == 0) result[0] = buffer[0] + buffer[1]; 
}
