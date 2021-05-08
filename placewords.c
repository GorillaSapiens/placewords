#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define S2_ERROR (1LL << 63) // an invalid location

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
uint64_t f(uint64_t n) {
   int lsb = n & 1;
   n >>= 1;
   if (lsb) {
      n ^= POLY;
   }
   return n;
}

// reverse LFSR
uint64_t g(uint64_t n) {
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
uint64_t f_many(uint64_t n) {
   for (int i = 0; i < MANY; i++) {
      n = f(n);
   }
   return n;
}

// reverse MANY times
uint64_t g_many(uint64_t n) {
   for (int i = 0; i < MANY; i++) {
      n = g(n);
   }
   return n;
}

// returns pointer to static memory, overwritten per call
const char *s2_to_placewords(uint64_t s2) {
   static char result[256];

   printf("s2=%lX %ld\n", s2, s2);

   s2 >>= 1;

   printf("loci2=%lld\n", (s2 >> EXCESS) & LFSR_MASK);
   uint64_t loci = f_many((s2 >> EXCESS) & LFSR_MASK);
   printf("loci=%ld\n", loci);

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

   printf("ords=%d %d %d\n", ordinal1, ordinal2, ordinal3);

   sprintf(result, "%s%s.%s.%s",
      PREFIX,
      ordinal_to_word(ordinal1),
      ordinal_to_word(ordinal2),
      ordinal_to_word(ordinal3));

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

   printf("ords=%d %d %d\n", ordinal1, ordinal2, ordinal3);

   if (ordinal1 & 1) {
      face |=  4;
   }
   if (ordinal2 & 1) {
      face |=  2;
   }
   if (ordinal3 & 1) {
      face |=  1;
   }

   printf("face=%ld\n", face);

   ordinal1 >>= 1;
   ordinal2 >>= 1;
   ordinal3 >>= 1;

   uint64_t loci = ordinal1;
   loci <<= BITS_PER_WORD;
   loci |= ordinal2;
   loci <<= BITS_PER_WORD;
   loci |= ordinal3;

   printf("loci=%ld\n", loci);

   loci = g_many(loci);

   printf("loci2=%ld\n", loci);

   uint64_t s2 = face;
   s2 <<= (BITS_PER_WORD * 3);
   s2 |= loci;
   s2 <<= EXCESS;

   s2 <<= 1;
   s2 |= 1LL;

   printf("s2=%lX %ld\n", s2, s2);

   return s2;
}

void init_placewords(char *language) {
   init_words(language);
}

#ifdef TEST
int main(void) {
   printf("%d %d %d %d\n",
         find_word("aaaa"),
         find_word("mygd"),
         find_word("myaa"),
         find_word("fuck"));

   printf("%011lx %011lx\n", f(1), g(f(1)));
   printf("%011lx %011lx\n", f_many(1), g_many(f_many(1)));

   for (int i = 0; i < 65536; i++) {
      printf("%ld %ld\n", f_many(g_many(i)), g_many(f_many(i)));
   }

   for (uint64_t i = 0; i < 65536; i++) {
      const char *p = s2_to_placewords(i << EXCESS);

      printf("%s %ld\n", p, placewords_to_s2(p) >> EXCESS);
   }

   return 0;
}
#endif
