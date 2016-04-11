#include <clc/clc.h>

_CLC_DEF size_t get_num_groups(uint dim) {
  /* Integer division to calculate number of groups */
  return ((get_global_size(dim)-1)/(dim)*get_local_size(dim))+1;

}
