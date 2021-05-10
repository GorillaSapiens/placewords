// Copyright 2021 Adam Wozniak All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: adam@wozniakconsulting.com (Adam Wozniak)

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define S2_ERROR 0 // an invalid location

// a URL style prefix
#define PREFIX "s2pw://"

#include "words.h"

#define BITS_PER_WORD 14 // 16384 words
#define NUM_WORDS 3

#define WORD_MASK ((1 << BITS_PER_WORD) - 1)

#define EXCESS (64 - 4 - BITS_PER_WORD * NUM_WORDS)
#define EXCESS_MASK ((1 << EXCESS) - 1)

#define LFSR_SIZE (BITS_PER_WORD * NUM_WORDS) // center bits of the S2 address
#define LFSR_MASK ((1LL << (LFSR_SIZE)) - 1)
#define POLY 0x2000000004CLL // 4 tap maximal LFSR

// how many times to permute
#define MANY (LFSR_SIZE)

// forward LFSR
static inline uint64_t f(uint64_t n) {
   int lsb = n & 1;
   n >>= 1;
   if (lsb) {
      n ^= POLY;
   }
   return n;
}

// reverse LFSR
static inline uint64_t g(uint64_t n) {
   uint64_t msb = n & (1LL << (LFSR_SIZE - 1));
   if (msb) {
      n ^= POLY;
   }
   n <<= 1;
   if (msb) {
      n |= 1;
   }
   return n;
}


// forward MANY times
static inline uint64_t f_many(uint64_t n) {
   for (int i = 0; i < MANY; i++) {
      n = f(n);
   }
   return n;
}

// reverse MANY times
static inline uint64_t g_many(uint64_t n) {
   for (int i = 0; i < MANY; i++) {
      n = g(n);
   }
   return n;
}

// returns pointer to static memory, overwritten per call
const char *s2_to_placewords(uint64_t s2) {
   static char result[256];

   s2 >>= 1;

   int excess = (s2 & EXCESS_MASK) >> 4;

   uint64_t loci = f_many((s2 >> EXCESS) & LFSR_MASK);

   int ordinal1 = ((loci >> (BITS_PER_WORD << 1)) & WORD_MASK);
   int ordinal2 = ((loci >> BITS_PER_WORD) & WORD_MASK);
   int ordinal3 = (loci & WORD_MASK);

   ordinal1 <<= 1;
   ordinal2 <<= 1;
   ordinal3 <<= 1;

   // top 4 bits contain face information
   if ((s2 >> 60)  & 4) {
      ordinal1 |= 1LL;
   }
   if ((s2 >> 60) & 2) {
      ordinal2 |= 1LL;
   }
   if ((s2 >> 60) & 1) {
      ordinal3 |= 1LL;
   }

   sprintf(result, "%s%s.%s.%s.%s",
      PREFIX,
      ordinal_to_word(ordinal1),
      ordinal_to_word(ordinal2),
      ordinal_to_word(ordinal3),
      ordinal_to_word(excess));

   return result;
}

// assumes url style "something://word.word.word.optionalword"
uint64_t placewords_to_s2(const char *placewords) {
   static char input[256];

   if (strstr(placewords, PREFIX) != placewords) {
      return S2_ERROR;
   }
   if (strlen(placewords) > sizeof(input)) {
      return S2_ERROR;
   }

   strcpy(input, placewords + strlen(PREFIX));

   char *w1 = strtok(input, ".");
   char *w2 = strtok(NULL, ".");
   if (w2 == NULL) {
      return S2_ERROR;
   }
   char *w3 = strtok(NULL, ".");
   if (w3 == NULL) {
      return S2_ERROR;
   }
   char *we = strtok(NULL, ".");

   uint64_t face = 0;

   int ordinal1 = word_to_ordinal(w1);
   if (ordinal1 == -1) {
      return S2_ERROR;
   }
   int ordinal2 = word_to_ordinal(w2);
   if (ordinal2 == -1) {
      return S2_ERROR;
   }
   int ordinal3 = word_to_ordinal(w3);
   if (ordinal3 == -1) {
      return S2_ERROR;
   }

   int excess = we ? word_to_ordinal(we) : 0;
   if (excess == -1) {
      // if we got this far, we're close, don't error
      excess = 0;
   }

   if (ordinal1 & 1) {
      face |=  4;
   }
   if (ordinal2 & 1) {
      face |=  2;
   }
   if (ordinal3 & 1) {
      face |=  1;
   }

   ordinal1 >>= 1;
   ordinal2 >>= 1;
   ordinal3 >>= 1;

   uint64_t loci = ordinal1;
   loci <<= BITS_PER_WORD;
   loci |= ordinal2;
   loci <<= BITS_PER_WORD;
   loci |= ordinal3;

   loci = g_many(loci);

   uint64_t s2 = face;
   s2 <<= (BITS_PER_WORD * 3);
   s2 |= loci;
   s2 <<= EXCESS;

   s2 |= (uint64_t) (excess << 4);

   s2 <<= 1;
   s2 |= 1LL;

   return s2;
}

void init_placewords(char *language) {
   init_words(language);
}
