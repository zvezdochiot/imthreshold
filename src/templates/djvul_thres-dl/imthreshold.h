/*
 DjVuL thresholds an image (Multi-scale binarization).

 *binarization*
 A preliminary binarization of the image.

 *foreground*
 Estimated foreground of the image.

 *background*
 Estimated background of the image.

 Own parameters:
				"b"     base block size (int, optional, default = 3)
				"f"     foreground divide (int, optional, default = 2)
				"l"     level (int, optional, default = 10)
				"r"     reverse FG/BG (bool, optional, default = no[0], recommended = yes[1])
*/

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/) sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <FreeImage.h>
#include "Utilities.h"
#include <unistd.h>

typedef struct mpixel
{
	BYTE c[3];
	WORD s;
}
IMTpixel;

////////////////////////////////////////////////////////////////////////////////

BYTE ImthresholdGet1BitPixel(BYTE *bits, unsigned x);
void ImthresholdSetPixel(BYTE *bits, unsigned x, BYTE* value);
void ImthresholdGetData(FIBITMAP* dib, IMTpixel** p_im);
IMTpixel IMTset(BYTE c0, BYTE c1, BYTE c2);
double IMTmean(IMTpixel** IMTim, unsigned height, unsigned width);
double IMTwb(IMTpixel** IMTim, double immean, unsigned height, unsigned width);
double IMTdist(IMTpixel IMTim0, IMTpixel IMTim1);
IMTpixel IMTmeanIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTmaxIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTminIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTaverageIc(IMTpixel** IMTim, IMTpixel IMTima, unsigned y0, unsigned x0, unsigned y1, unsigned x1, double part);
FIBITMAP* ImthresholdFilterNone(FIBITMAP* src_dib);
void IMTFilterDjVuL(IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, int wbmode, double anisotropic);
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);
FIBITMAP* ImthresholdGenericLoader(const char* lpszPathName, int flag);
