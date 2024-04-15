/*
This file is part of LICO, a fast lossless image compressor.

Copyright (c) 2023, Noushin Azami, Martin Burtscher, and Travis Law

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

URL: The latest version of this code is available at https://github.com/burtscher/LICO.

Publication: This work is described in detail in the following paper.
Noushin Azami, Rain Lawson, and Martin Burtscher. "LICO: An Effective, High-Speed, Lossless Compressor for Images." Proceedings of the 2024 Data Compression Conference. Snowbird, UT. March 2024.

Sponsor: This code is based upon work supported by the U.S. Department of Energy, Office of Science, Office of Advanced Scientific Research (ASCR), under contract DE-SC0022223.
*/


#ifndef H_COMMON
#define H_COMMON

// CSTD
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

// STL
#include <algorithm>
#include <array>
#include <vector>

// Extern libraries

#ifdef USE_HIGHWAY
#include <hwy/highway.h>
#endif

// LICO headers
#include "h_Common_Defines.h"
#include "h_BMP_BIT.h"
#include "h_ZERE_1.h"
#include "h_ZERE_4.h"

#endif