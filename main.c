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
#include <stdlib.h>
#include <string.h>

#include "s2.h"
#include "placewords.h"

static inline uint64_t dehex(char *p) {
   uint64_t result;

   while (*p) {
      result <<= 4;
      switch (*p) {
         case '0' ... '9':
            result |= (uint64_t) (*p - '0');
            break;
         case 'a' ... 'f':
            result |= (uint64_t) (*p - 'a' + 10);
            break;
         case 'A' ... 'F':
            result |= (uint64_t) (*p - 'A' + 10);
            break;
         default:
            printf("Unrecognized character '%c'\n", *p);
            exit(-1);
            break;
      }
      p++;
   }
   return result;
}

int main(int argc, char **argv) {
   char *arg0 = argv[0];

   if (argc > 1 && argv[1][0] == '=') {
      init_placewords(argv[1] + 1); // TODO FIX sanitize?
      argv++;
      argc--;
   }
   else {
      // default to english
      init_placewords("en");
   }

   switch(argc) {
      case 3:
         {
            int latE6 = atoi(argv[1]);
            int lonE6 = atoi(argv[2]);
            uint64_t s2 = ll_to_s2(latE6, lonE6);
            const char *result = s2_to_placewords(s2);
            printf("%016lX\n%s\n", s2, result);
         }
         break;
      case 2:
         {
            if (strchr(argv[1], ':') != NULL) {
               uint64_t s2 = placewords_to_s2(argv[1]);
               int *result = s2_to_ll(s2);
               printf("%016lX\n%d %d\n", s2, result[0], result[1]);
            }
            else {
               uint64_t s2 = dehex(argv[1]);
               int *result = s2_to_ll(s2);
               printf("%016lX\n%d %d\n", s2, result[0], result[1]);
               const char *words = s2_to_placewords(s2);
               printf("%s\n", words);
            }
         }
         break;
      default:
         printf("Usage: %s [=LANG] <latE6> <lonE6>\n"
                "   or: %s [=LANG] <s2pw://some.words.here>\n"
                "   or: %s [=LANG] <[0-9a-fA-F]{16}>\n",
                arg0, arg0, arg0);
   }
   return 0;
}
