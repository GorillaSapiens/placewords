#include <stdio.h>
#include <stdlib.h>

#include "s2.h"
#include "placewords.h"

int main(int argc, char **argv) {
   init_placewords("en");

   switch(argc) {
      case 3:
         {
            int latE6 = atoi(argv[1]);
            int lonE6 = atoi(argv[2]);
            uint64_t s2 = ll_to_s2(latE6, lonE6);
            const char *result = s2_to_placewords(s2);
            printf("%s\n", result);
         }
         break;
      case 2:
         {
            uint64_t s2 = placewords_to_s2(argv[1]);
            int *result = s2_to_ll(s2);
            printf("%d %d\n", result[0], result[1]);
         }
         break;
   }
   return 0;
}
