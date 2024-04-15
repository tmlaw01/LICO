/*
This file is part of LICO, a fast lossless image compressor.

Copyright (c) 2023, Noushin Azami and Martin Burtscher

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


#ifndef bmp_bit
#define bmp_bit

#include <cstdio>
#include <cassert>

#include <vector>
#include <array>

#include "h_Common_Defines.h"

/// @brief This function takes a byte array as input and returns a 16-bit value
/// by appending the first two elements of the array. The first element
/// is treated as the least significant byte and the second element is
/// treated as the most significant byte.
/// @param data The byte array from which the value is retrieved.
/// @return The 16-bit value obtained from the byte array.
static inline int32_t h_BMP_BIT_get2(const byte data [])
{
  return (0 | (data[1] << 8) | data[0]);
}

/// @brief This function takes a byte array as input and returns a 32-bit value
/// by appending the first two elements of the array. The first element
/// is treated as the least significant byte and the second element is
/// treated as the most significant byte.
/// @param data The byte array from which the value is retrieved.
/// @return The 32-bit value obtained from the byte array.
static inline int32_t h_BMP_BIT_get4(const byte data [])
{
  return ((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
}

/// @brief Sets the first 2 bytes of the given byte array with the specified value.
/// The value is split into 2 bytes and stored in little-endian format.
/// @param data The byte array to set the values in.
/// @param val The value to set.
static inline void h_BMP_BIT_set2(byte data [], const int32_t val)
{
  data[0] = val;
  data[1] = val >> 8;
}

/// @brief Sets the first 4 bytes of the given byte array with the specified value.
/// The value is split into 4 bytes and stored in little-endian format.
/// @param data The byte array to set the values in.
/// @param val The value to set.
static inline void h_BMP_BIT_set4(byte data [], const int32_t val)
{
  data[0] = val;
  data[1] = val >> 8;
  data[2] = val >> 16;
  data[3] = val >> 24;
}

static inline void h_BMP_BIT(int32_t& size, byte*& data)
{
  assert(sizeof(uint64_t) == 8);

  if (size < 54) {
    printf("h_BMP_BIT: WARNING file size is too small for a BMP image\n");
  } else {
    const int32_t w = h_BMP_BIT_get4(&data[18]); // width
    const int32_t h = h_BMP_BIT_get4(&data[22]); // height
    const int32_t pad = ((w * 3 + 3) & ~3) - (w * 3);
    const int32_t width = w * 3 + pad;
    if ((data[0] != 'B') || 
        (data[1] != 'M') || 
        (h_BMP_BIT_get4(&data[2]) != 54 + h * width) || // Size sanity check
        (h_BMP_BIT_get4(&data[10]) != 54) ||            // Offset to image data
        (h_BMP_BIT_get4(&data[14]) != 40) ||            // Header size
        (h_BMP_BIT_get2(&data[26]) != 1) ||             // Color planes (must be 1)
        (h_BMP_BIT_get2(&data[28]) != 24) ||            // Bits per pixel (only 24 supported)
        (h_BMP_BIT_get4(&data[30]) != 0) ||             // Compression method (only 0 supported)
        (h_BMP_BIT_get4(&data[34]) != h * width) ||     // Image size
        (h_BMP_BIT_get4(&data[46]) != 0) ||             // Number of colors or 0
        (h_BMP_BIT_get4(&data[50]) != 0) ||             // Important colors or 0
        (size != 54 + h * width) ||                     // File size sanity check again
        (w < 1) || (h < 1)) {                           // Width and height sanity check
      printf("h_BMP_BIT: WARNING: not a supported BMP format\n");
    } else {
      data[0] = data[0] - 'B';                                                           // B
      data[1] = data[1] - 'M';                                                           // M
      h_BMP_BIT_set4(&data[2], h_BMP_BIT_get4(&data[2]) - (h * width + 54));             // size in bytes
      //h_BMP_BIT_set4(&data[6], h_BMP_BIT_get4(&data[6]));                              // 2 reserved values (0, 0)
      h_BMP_BIT_set4(&data[10], h_BMP_BIT_get4(&data[10]) - 54);                         // offset to image data (54)
      h_BMP_BIT_set4(&data[14], h_BMP_BIT_get4(&data[14]) - 40);                         // header size (40)
      //h_BMP_BIT_set4(&data[18], w);                                                    // width
      //h_BMP_BIT_set4(&data[22], h);                                                    // height
      h_BMP_BIT_set2(&data[26], h_BMP_BIT_get2(&data[26]) - 1);                          // color planes (must be 1)
      h_BMP_BIT_set2(&data[28], h_BMP_BIT_get2(&data[28]) - 24);                         // bits per pixel (only 24 supported)
      //h_BMP_BIT_set4(&data[30], h_BMP_BIT_get4(&data[30]) - 0);                        // compression method (only 0 supported)
      h_BMP_BIT_set4(&data[34], h_BMP_BIT_get4(&data[34]) - (h * width));                // image size
      //h_BMP_BIT_set4(&data[38], h_BMP_BIT_get4(&data[38]));                            // horizontal resolution
      h_BMP_BIT_set4(&data[42], h_BMP_BIT_get4(&data[42]) - h_BMP_BIT_get4(&data[38]));  // vertical resolution [same as previous?]
      //h_BMP_BIT_set4(&data[46], h_BMP_BIT_get4(&data[46]) - 0);                        // number of colors or 0
      //h_BMP_BIT_set4(&data[50], h_BMP_BIT_get4(&data[50]) - 0);                        // important colors or 0

      const int32_t newsize = w * h * sizeof(byte);
      uint64_t* const temp = new uint64_t [(newsize * 3 + 7) / 8]; // 3 channels, 8 bits per channel, plus padding to insure flooring doesn't happen.
      byte* const tmp = (byte*)temp;
      byte* const bmp = (byte*)&data[54];

      // encode
      #pragma omp parallel for default(none) shared(width, w, h, bmp, tmp, newsize)
      for (int32_t y = 0; y < h; y++) {
        int32_t p0 = 0, p1 = 0, p2 = 0;
        if (y > 0) {  // pixel y DIFF
          p0 = bmp[(y - 1) * width + 0];
          p1 = bmp[(y - 1) * width + 1];
          p2 = bmp[(y - 1) * width + 2];
        }
        for (int32_t x = 0; x < w; x++) {
          // read and split into color channels
          const int32_t n0 = bmp[y * width + x * 3 + 0];
          const int32_t n1 = bmp[y * width + x * 3 + 1];
          const int32_t n2 = bmp[y * width + x * 3 + 2];

          // pixel x DIFF
          int32_t v0 = n0 - p0;
          int32_t v1 = n1 - p1;
          int32_t v2 = n2 - p2;
          p0 = n0;
          p1 = n1;
          p2 = n2;

          // color-channel DIFF
          v0 -= v1;
          v2 -= v1;

          // Two's Complement Magnitude Sign (TCMS)
          v0 = ((v0 << 1) ^ ((v0 << 24) >> 31)) & 0xff; // 0bxxxxxxxx,xxxxxxxx,xxxxxxxx,xxxxxxxx
          v1 = ((v1 << 1) ^ ((v1 << 24) >> 31)) & 0xff; // 0bxxxxxxxx,xxxxxxxx,xxxxxxxx,xxxxxxx0
          v2 = ((v2 << 1) ^ ((v2 << 24) >> 31)) & 0xff; // 0b00000000,00000000,00000000,0000000y ^
                                                        // 0bxxxxxxxx,xxxxxxxx,xxxxxxxx,xxxxxxxy
                                                        // 0b00000000,00000000,00000000,11111111 &
                                                        // 0b00000000,00000000,00000000,xxxxxxxy
                                                        
          // transpose, separate channels TUPL3, write encoded values ??? 
          
          // Tuples are not a concept in C++, so I'm assuming this was supposed to refer to a RGB32 pixel format/struct. Which in the case
          // that it is, I would refactor this to use a specific struct with functions to handle the encoding instead.

          // Transposes matrix and write TCMS encoded channel values to their position in the tmp pixel buffer.
          tmp[0 * newsize + y + x * h] = v0;
          tmp[1 * newsize + y + x * h] = v1;
          tmp[2 * newsize + y + x * h] = v2;
        }
      }

      // BIT_1
      const int32_t csize = newsize * 3;
      const int32_t extra = csize % 8; // Was (8 * 8 / 8) ??? Would have been optimized out, but I find it odd that it's here.
      const int32_t esize = csize - extra;
      #pragma omp parallel for default(none) shared(esize, bmp, temp)
      for (int32_t pos = 0; pos < esize; pos += 8) {
        uint64_t t, x; // This is basically poor man's SIMD
        x = temp[pos / 8];
        t = (x ^ (x >> 7)) & 0x00AA00AA00AA00AAULL;  // b00000000,10101010,00000000,10101010,00000000,10101010,00000000,10101010
        x = x ^ t ^ (t << 7);                        // b01010101,10101010,01010101,10101010,01010101,10101010,01010101,10101010
        t = (x ^ (x >> 14)) & 0x0000CCCC0000CCCCULL; // b00000000,00000000,11001100,11001100,00000000,00000000,11001100,11001100
        x = x ^ t ^ (t << 14);                       // b00110011,00110011,11001100,11001100,00110011,00110011,11001100,11001100
        t = (x ^ (x >> 28)) & 0x00000000F0F0F0F0ULL; // b00000000,00000000,00000000,00000000,11110000,11110000,11110000,11110000
        x = x ^ t ^ (t << 28);                       // b00001111,00001111,00001111,00001111,11110000,11111000,11111000,11110000
        for (int32_t i = 0; i < 8; i++) {
          bmp[pos / 8 + i * (esize / 8)] = x >> (i * 8);
        }
      }

      // copy leftover bytes
      for (int32_t i = 0; i < extra; i++) {
        bmp[csize - extra + i] = tmp[csize - extra + i];
      }

      // handle padding (if any)
      for (int32_t i = newsize * 3; i < width * h; i++) {
        bmp[i] = 0;
      }

      delete [] temp;
    }
  }
}


static inline void h_iBMP_BIT(int32_t& size, byte*& data)
{
  assert(sizeof(uint64_t) == 8);

  if (size < 54) {
    printf("h_BMP_BIT: WARNING file size is too small for a BMP image\n");
  } else {
    const int32_t w = h_BMP_BIT_get4(&data[18]);
    const int32_t h = h_BMP_BIT_get4(&data[22]);
    const int32_t pad = ((w * 3 + 3) & ~3) - (w * 3);
    const int32_t width = w * 3 + pad;
    if ((data[0] != 0) || (data[1] != 0) || (h_BMP_BIT_get4(&data[2]) != 0) || (h_BMP_BIT_get4(&data[10]) != 0) || (h_BMP_BIT_get4(&data[14]) != 0) || (h_BMP_BIT_get2(&data[26]) != 0) || (h_BMP_BIT_get2(&data[28]) != 0) || (h_BMP_BIT_get4(&data[30]) != 0) || (h_BMP_BIT_get4(&data[34]) != 0) || (h_BMP_BIT_get4(&data[46]) != 0) || (h_BMP_BIT_get4(&data[50]) != 0) || (w < 1) || (h < 1)) {
      printf("h_BMP_BIT: WARNING not a supported BMP format\n");
    } else {
      data[0] = data[0] + 'B';  // B
      data[1] = data[1] + 'M';  // M
      h_BMP_BIT_set4(&data[2], h_BMP_BIT_get4(&data[2]) + (h * width + 54));  // size in bytes
      //h_BMP_BIT_set4(&data[6], h_BMP_BIT_get4(&data[6]));  // 2 reserved values (0, 0)
      h_BMP_BIT_set4(&data[10], h_BMP_BIT_get4(&data[10]) + 54);  // offset to image data (54)
      h_BMP_BIT_set4(&data[14], h_BMP_BIT_get4(&data[14]) + 40);  // header size (40)
      //h_BMP_BIT_set4(&data[18], w);  // width
      //h_BMP_BIT_set4(&data[22], h);  // height
      h_BMP_BIT_set2(&data[26], h_BMP_BIT_get2(&data[26]) + 1);  // color planes (must be 1)
      h_BMP_BIT_set2(&data[28], h_BMP_BIT_get2(&data[28]) + 24);  // bits per pixel (only 24 supported)
      //h_BMP_BIT_set4(&data[30], h_BMP_BIT_get4(&data[30]) + 0);  // compression method (only 0 supported)
      h_BMP_BIT_set4(&data[34], h_BMP_BIT_get4(&data[34]) + (h * width));  // image size
      //h_BMP_BIT_set4(&data[38], h_BMP_BIT_get4(&data[38]));  // horizontal resolution
      h_BMP_BIT_set4(&data[42], h_BMP_BIT_get4(&data[42]) + h_BMP_BIT_get4(&data[38]));  // vertical resolution [same as previous?]
      //h_BMP_BIT_set4(&data[46], h_BMP_BIT_get4(&data[46]) + 0);  // number of colors or 0
      //h_BMP_BIT_set4(&data[50], h_BMP_BIT_get4(&data[50]) + 0);  // important colors or 0

      const int32_t newsize = w * h * sizeof(byte);
      uint64_t* const temp = new uint64_t [(newsize * 3 + 7) / 8];
      byte* const tmp = (byte*)temp;
      byte* const bmp = (byte*)&data[54];

      // iBIT_1
      const int32_t csize = newsize * 3;
      const int32_t extra = csize % (8 * 8 / 8);
      const int32_t esize = csize - extra;
      #pragma omp parallel for default(none) shared(esize, bmp, temp)
      for (int32_t pos = 0; pos < esize; pos += 8) {
        uint64_t t, x = 0;
        for (int32_t i = 0; i < 8; i++) x |= (uint64_t)bmp[pos / 8 + i * (esize / 8)] << (i * 8); 
        t = (x ^ (x >> 7)) & 0x00AA00AA00AA00AAULL; // MSB
        x = x ^ t ^ (t << 7);
        t = (x ^ (x >> 14)) & 0x0000CCCC0000CCCCULL; // 2nd MSB
        x = x ^ t ^ (t << 14);
        t = (x ^ (x >> 28)) & 0x00000000F0F0F0F0ULL; // 3rd MSB
        x = x ^ t ^ (t << 28);
        temp[pos / 8] = x;
      }

      // copy leftover bytes
      for (int32_t i = 0; i < extra; i++) {
        tmp[csize - extra + i] = bmp[csize - extra + i];
      }

      // decode
      int32_t prev0 = 0, prev1 = 0, prev2 = 0;
      for (int32_t y = 0; y < h; y++) {
        // read values, combine channels iTUPL3, inverse transpose
        int32_t v0 = tmp[0 * newsize + y];
        int32_t v1 = tmp[1 * newsize + y];
        int32_t v2 = tmp[2 * newsize + y];

        // inverse TCMS
        v0 = (v0 >> 1) ^ ((v0 << 31) >> 31);
        v1 = (v1 >> 1) ^ ((v1 << 31) >> 31);
        v2 = (v2 >> 1) ^ ((v2 << 31) >> 31);

        // inverse color-channel DIFF
        v0 += v1;
        v2 += v1;

        // inverse pixel y DIFF
        v0 += prev0;
        v1 += prev1;
        v2 += prev2;

        // write decoded values
        bmp[y * width + 0] = prev0 = v0;
        bmp[y * width + 1] = prev1 = v1;
        bmp[y * width + 2] = prev2 = v2;
      }

      #pragma omp parallel for default(none) shared(width, w, h, tmp, bmp, newsize)
      for (int32_t y = 0; y < h; y++) {
        int32_t prev0 = bmp[y * width + 0];
        int32_t prev1 = bmp[y * width + 1];
        int32_t prev2 = bmp[y * width + 2];
        for (int32_t x = 1; x < w; x++) {
          // read values, combine channels iTUPL3, inverse transpose
          int32_t v0 = tmp[0 * newsize + y + x * h];
          int32_t v1 = tmp[1 * newsize + y + x * h];
          int32_t v2 = tmp[2 * newsize + y + x * h];

          // inverse TCMS
          v0 = (v0 >> 1) ^ ((v0 << 31) >> 31);
          v1 = (v1 >> 1) ^ ((v1 << 31) >> 31);
          v2 = (v2 >> 1) ^ ((v2 << 31) >> 31);

          // inverse color-channel DIFF
          v0 += v1;
          v2 += v1;

          // inverse pixel x DIFF
          v0 += prev0;
          v1 += prev1;
          v2 += prev2;

          // write decoded values
          bmp[y * width + x * 3 + 0] = prev0 = v0;
          bmp[y * width + x * 3 + 1] = prev1 = v1;
          bmp[y * width + x * 3 + 2] = prev2 = v2;
        }
      }

      // set padding bytes (if any) to zero
      if (pad > 0) {
        for (int32_t y = 0; y < h; y++) {
          for (int32_t x = w * 3; x < width; x++) {
            bmp[y * width + x] = 0;
          }
        }
      }

      delete [] temp;
    }
  }
}


#endif
