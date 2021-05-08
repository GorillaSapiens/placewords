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

#ifndef _INCLUDE_S2_H_
#define _INCLUDE_S2_H_

#include <stdint.h>

extern uint64_t ll_to_s2(int latE6, int lonE6);
extern int *s2_to_ll(uint64_t s2);

#endif
