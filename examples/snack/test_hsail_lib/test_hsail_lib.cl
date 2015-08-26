#include "../../mathdemo_hsaillib/mathdemo_hsaillib.h"
__kernel void testkernel( __global float * outfval , __global const float  * fval) {
   int i = get_global_id(0);
   outfval[i] =  __sin(fval[i]);
}
