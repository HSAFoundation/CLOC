#include <clc/clc.h>

#define IMPL(TYPE) \
_CLC_OVERLOAD _CLC_DEF TYPE atom_inc(local TYPE *p) { \
  return atom_add(p, 1); \
}

IMPL(int)
IMPL(unsigned int)
