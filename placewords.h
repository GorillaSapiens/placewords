#ifndef _INCLUDE_PLACEWORDS_
#define _INCLUDE_PLACEWORDS_

#include <stdint.h>

// returns pointer to static memory, overwritten per call
extern const char *s2_to_placewords(uint64_t s2);

// assumes url style "s2pw://word.word.word"
extern uint64_t placewords_to_s2(const char *placewords);

extern void init_placewords(char *language);

#endif
