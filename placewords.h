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

#ifndef _INCLUDE_PLACEWORDS_
#define _INCLUDE_PLACEWORDS_

#include <stdint.h>

// returns pointer to static memory, overwritten per call
extern const char *s2_to_placewords(uint64_t s2);

// assumes url style "s2pw://word.word.word"
extern uint64_t placewords_to_s2(const char *placewords);

extern void init_placewords(char *language);

#endif
