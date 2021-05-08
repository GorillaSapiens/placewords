#ifndef _INCLUDE_S2_H_
#define _INCLUDE_S2_H_

#include <stdint.h>

extern uint64_t ll_to_s2(int latE6, int lonE6);
extern int *s2_to_ll(uint64_t s2);

#endif
