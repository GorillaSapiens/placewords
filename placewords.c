#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define S2_ERROR (1LL << 63) // an invalid location

// a URL style prefix
#define PREFIX "s2pw://"

const char const *words[16384+16384+1] = {
#include "words.h"
   NULL
};

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

   uint64_t loci = f_many((s2 >> EXCESS) & LFSR_MASK);
   int word1 = (loci >> (BITS_PER_WORD << 1)) & WORD_MASK;
   int word2 = (loci >> BITS_PER_WORD) & WORD_MASK;
   int word3 = loci & WORD_MASK;

   if (s2 & 4) {
      word1 += (1 << BITS_PER_WORD);
   }
   if (s2 & 2) {
      word2 += (1 << BITS_PER_WORD);
   }
   if (s2 & 1) {
      word3 += (1 << BITS_PER_WORD);
   }

   sprintf(result, "%s%s.%s.%s",
      PREFIX, words[word1], words[word2], words[word3]);

   return result;
}

int find_word(const char *word) {
   // binary search, could be sped up with hashtable on words
   int begin = 0;
   int end = (sizeof(words) / sizeof(words[0])) - 1;

   while ((end - begin) > 2) {
      int middle = (begin + end) >> 1;
      int result = strcmp(word, words[middle]);
      if (result == 0) {
         return middle;
      }
      else if (result < 0) {
         end = middle;
      }
      else {
         begin = middle;
      }
   }

   while (begin <= end) {
      if (!strcmp(word, words[begin])) {
         return begin;
      }
      begin++;
   }

   return -1;
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

   int word1 = find_word(w1);
   if (word1 == -1) {
      return S2_ERROR;
   }
   int word2 = find_word(w2);
   if (word2 == -1) {
      return S2_ERROR;
   }
   int word3 = find_word(w3);
   if (word3 == -1) {
      return S2_ERROR;
   }

   if (word1 > 16384) {
      word1 -= 16384;
      face |=  4;
   }
   if (word2 > 16384) {
      word2 -= 16384;
      face |=  2;
   }
   if (word3 > 16384) {
      word3 -= 16384;
      face |=  1;
   }

   uint64_t loci = word1;
   loci <<= BITS_PER_WORD;
   loci |= word2;
   loci <<= BITS_PER_WORD;
   loci |= word3;

   loci = g_many(loci);

   uint64_t s2 = face;
   s2 <<= (BITS_PER_WORD * 3);
   s2 |= loci;
   s2 <<= EXCESS;

   return s2;
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

   for (int i = 0; i < 8192; i++) {
      const char *p = s2_to_placewords(i << EXCESS);

      printf("%s %ld\n", p, placewords_to_s2(p) >> EXCESS);
   }

   return 0;
}
#endif
