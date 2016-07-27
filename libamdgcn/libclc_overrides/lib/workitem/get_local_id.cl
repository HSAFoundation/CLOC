#include <clc/clc.h>
_CLC_OVERLOAD _CLC_DEF size_t get_local_id(uint dim) { return (get_local_id_ll(dim)); }
