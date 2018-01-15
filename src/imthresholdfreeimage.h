//    Zlib license
//
// ImThreshold header library (freeimage api).
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#ifndef IMTHRESHOLD_FREEIMAGE_H
#define IMTHRESHOLD_FREEIMAGE_H

#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <sstream>
#include "imthreshold.h"

// ==========================================================
//   Bitmap palette and pixels alignment
// ==========================================================

#define FIBITMAP_ALIGNMENT    16    // We will use a 16 bytes alignment boundary

// Memory allocation on a specified alignment boundary
// defined in BitmapAccess.cpp

void* FreeImage_Aligned_Malloc(size_t amount, size_t alignment);
void FreeImage_Aligned_Free(void* mem);

// ==========================================================
//   File I/O structs
// ==========================================================

// these structs are for file I/O and should not be confused with similar
// structs in FreeImage.h which are for in-memory bitmap handling

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif // _WIN32

typedef struct tagFILE_RGBA {
  unsigned char r,g,b,a;
} FILE_RGBA;

typedef struct tagFILE_BGRA {
  unsigned char b,g,r,a;
} FILE_BGRA;

typedef struct tagFILE_RGB {
  unsigned char r,g,b;
} FILE_RGB;

typedef struct tagFILE_BGR {
  unsigned char b,g,r;
} FILE_BGR;

#ifdef _WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif // _WIN32

// ==========================================================
//   Utility functions
// ==========================================================

#ifndef _WIN32
inline char*
i2a(unsigned i, char *a, unsigned r) {
    if (i/r > 0) a = i2a(i/r,a,r);
    *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i%r];
    return a+1;
}

/**
 Transforms integer i into an ascii string and stores the result in a;
 string is encoded in the base indicated by r.
 @param i Number to be converted
 @param a String result
 @param r Base of value; must be in the range 2 - 36
 @return Returns a
*/
inline char *
_itoa(int i, char *a, int r) {
    r = ((r < 2) || (r > 36)) ? 10 : r;
    if(i < 0) {
        *a = '-';
        *i2a(-i, a+1, r) = 0;
    }
    else *i2a(i, a, r) = 0;
    return a;
}

#endif // !_WIN32

inline unsigned char
HINIBBLE (unsigned char byte) {
    return byte & 0xF0;
}

inline unsigned char
LOWNIBBLE (unsigned char byte) {
    return byte & 0x0F;
}

inline int
CalculateUsedBits(int bits) {
    int bit_count = 0;
    unsigned bit = 1;

    for (unsigned i = 0; i < 32; i++) {
        if ((bits & bit) == bit) {
            bit_count++;
        }

        bit <<= 1;
    }

    return bit_count;
}

inline int
CalculateLine(int width, int bitdepth) {
    return ((width * bitdepth) + 7) / 8;
}

inline int
CalculatePitch(int line) {
    return ((line + 3) & ~3);
}

inline int
CalculateUsedPaletteEntries(int bit_count) {
    if ((bit_count >= 1) && (bit_count <= 8))
        return 1 << bit_count;

    return 0;
}

inline unsigned char *
CalculateScanLine(unsigned char *bits, unsigned pitch, int scanline) {
    return (bits + (pitch * scanline));
}

inline void
ReplaceExtension(char *result, const char *filename, const char *extension) {
    for (int i = strlen(filename) - 1; i > 0; --i) {
        if (filename[i] == '.') {
            memcpy(result, filename, i);
            result[i] = '.';
            memcpy(result + i + 1, extension, strlen(extension) + 1);
            return;
        }
    }

    memcpy(result, filename, strlen(filename));
    result[strlen(filename)] = '.';
    memcpy(result + strlen(filename) + 1, extension, strlen(extension) + 1);
}

// ==========================================================
//   Big Endian / Little Endian utility functions
// ==========================================================

inline void
SwapShort(WORD *sp) {
    BYTE *cp = (BYTE *)sp, t = cp[0]; cp[0] = cp[1]; cp[1] = t;
}

inline void
SwapLong(DWORD *lp) {
    BYTE *cp = (BYTE *)lp, t = cp[0]; cp[0] = cp[3]; cp[3] = t;
    t = cp[1]; cp[1] = cp[2]; cp[2] = t;
}

// ==========================================================
//   Greyscale conversion
// ==========================================================

#define GREY(r, g, b) (BYTE)(((WORD)r * 77 + (WORD)g * 150 + (WORD)b * 29) >> 8)    // .299R + .587G + .114B
/*
#define GREY(r, g, b) (BYTE)(((WORD)r * 169 + (WORD)g * 256 + (WORD)b * 87) >> 9)    // .33R + 0.5G + .17B
*/

// ==========================================================
//   Template utility functions
// ==========================================================

/// INPLACESWAP adopted from codeguru.com
template <class T> void INPLACESWAP(T& a, T& b) {
    a ^= b; b ^= a; a ^= b;
}


////////////////////////////////////////////////////////////////////////////////

BYTE ImthresholdGet1BitPixel(BYTE *bits, unsigned x);
void ImthresholdSetPixel(BYTE *bits, unsigned x, BYTE* value);
void ImthresholdGetData(FIBITMAP* dib, IMTpixel** p_im);
void ImthresholdSetData(FIBITMAP* dib, IMTpixel** p_im);
void ImthresholdGetDataBW(FIBITMAP* dib, BYTE** p_im);
void ImthresholdSetDataBW(FIBITMAP* dib, BYTE** d_im);
FIBITMAP* ImthresholdFilterNone(FIBITMAP* src_dib);
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);
FIBITMAP* ImthresholdGenericLoader(const char* lpszPathName, int flag);

////////////////////////////////////////////////////////////////////////////////

#endif // IMTHRESHOLD_FREEIMAGE_H
