#include <stdio.h>
#include <stdint.h>

#define LFSR_SIZE 52
#define LFSR_MASK ((1LL << (LFSR_SIZE)) - 1)
#define POLY 0x8000000000FBFULL // 12 taps!

// forward LFSR
uint64_t f(uint64_t n) {
   uint64_t tmp = n ^ POLY;
   int count = 0;
   while (tmp) {
      if (tmp & 1) { count++; }
      tmp >>= 1;
   }
   n = (n << 1) | (count & 1);
   n &= LFSR_MASK;
   return n;
}

// forward LFSR_SIZE times
uint64_t f_many(uint64_t n) {
   for (int i = 0; i < LFSR_SIZE; i++) {
      n = f(n);
   }
   return n;
}

// reverse LFSR
uint64_t g(uint64_t n) {
   uint64_t parity = n & 1;
   n >>= 1;
   uint64_t tmp = n ^ POLY;
   int count = 0;
   while (tmp) {
      if (tmp & 1) { count++; }
      tmp >>= 1;
   }
   if ((count & 1) ^ parity) {
      n |= (1LL << (LFSR_SIZE - 1));
   }
   return n;
}

// reverse LFSR_SIZE times
uint64_t g_many(uint64_t n) {
   for (int i = 0; i < LFSR_SIZE; i++) {
      n = g(n);
   }
   return n;
}
