#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Word;

typedef struct Word {
   char *word;
   int ordinal;
   struct Word *next_ord;
   struct Word *next_hash;
} Word;

#define HASH_SIZE 1024
#define HASH_POLY 0x3FC
static Word *by_ordinal[32768];
static Word *by_hash[HASH_SIZE];

static int line = 0;

static inline void trim(char *p) {
   while (*p >= ' ' && *p != '#') {
      p++;
   }
   *p = 0;
}

static int hash_fn(char *p) {
   int hash = *p++;
   while (*p) {
      hash <<= 8;
      hash |= *p;
      do {
         int lsb = hash & 1;
         hash >>= 1;
         if (lsb) {
            hash ^= HASH_POLY;
         }
      } while (hash > HASH_SIZE);
      p++;
   }
   return hash;
}

static void new_word(int ordinal, char *p) {
   if (ordinal < 0 || ordinal > sizeof(by_ordinal) / sizeof(by_ordinal[0])) {
      fprintf(stderr, "Bad ordinal %d at line %d\n", ordinal, line);
      exit(-1);
   }

   Word *next_ord = by_ordinal[ordinal];
   by_ordinal[ordinal] = (Word *) malloc(sizeof(Word));
   by_ordinal[ordinal]->word = strdup(p);
   by_ordinal[ordinal]->ordinal = ordinal;
   by_ordinal[ordinal]->next_ord = next_ord;

   int hash = hash_fn(p);
   by_ordinal[ordinal]->next_hash = by_hash[hash];
   by_hash[hash] = by_ordinal[ordinal];
}

int init_words(const char *language) {
   memset(by_ordinal, 0, sizeof(by_ordinal));
   memset(by_hash, 0, sizeof(by_hash));

   char buf[256];
   sprintf(buf, "words/%s.txt", language);
   FILE *f = fopen(buf, "r");
   if (f) {
      while (fgets(buf, sizeof(buf), f)) {
         line++;
         trim(buf);
         char *p = strchr(buf, ' ');
         if (p) {
            *p = 0;
            p++;

            int ordinal = atoi(buf);
            for (p = strtok(p, ","); p; p = strtok(NULL, ",")) {
               new_word(ordinal, p);
            }
         }
      }
      fclose(f);
   }
   else {
      fprintf(stderr, "Unable to open %s for reading\n", buf);
      exit(-1);
   }

   // sanity check on words
   for (int i = 0; i < sizeof(by_ordinal) / sizeof(by_ordinal[0]); i++) {
      if (!by_ordinal[i]) {
         fprintf(stderr, "Missing ordinal %d\n", i);
         exit(-1);
      }
   }
}

char *ordinal_to_word(int ordinal) {
   if (ordinal < 0 || ordinal > sizeof(by_ordinal) / sizeof(by_ordinal[0])) {
      fprintf(stderr, "Bad ordinal %d in %s\n", ordinal, __FUNCTION__);
      exit(-1);
   }
   return by_ordinal[ordinal]->word;
}

int word_to_ordinal(char *word) {
   int hash = hash_fn(word);
   for (Word *w = by_hash[hash]; w; w = w->next_hash) {
      if (!strcmp(word, w->word)) {
         return w->ordinal;
      }
   }
   return -1;
}

#ifdef TEST
void main(void) {
   init_words("en");
   printf("%s %d\n", ordinal_to_word(11285), word_to_ordinal("coppa"));
}
#endif
