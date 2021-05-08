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
