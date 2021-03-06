//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//	http://www.gnu.org/copyleft/gpl.html

// This algorithm was taken from the C++ Augmented Reality Toolkit sourcecodes
// http://www.dandiggins.co.uk/arlib-1.html
// and adopted for the FreeImage library
//
// Copyright (C) 2007-2008:
// monday2000  monday2000@yandex.ru

#include <math.h>
#include <stdlib.h> 
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

BYTE ImthresholdGet1BitPixel(BYTE *bits, unsigned x)
{
	return (bits[x >> 3] & (0x80 >> (x & 0x07))) != 0 ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdSetPixel(BYTE *bits, unsigned x, BYTE* value)
{
	*value ?  bits[x >> 3] &= (0xFF7F >> (x & 0x7)) : bits[x >> 3] |= (0x80 >> (x & 0x7));
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdGetData(FIBITMAP* dib, IMTpixel** p_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	unsigned bpp = FreeImage_GetBPP(dib);
	BYTE* bits = (BYTE*)FreeImage_GetBits(dib);
	BYTE* lines;
	RGBQUAD *dibpal = FreeImage_GetPalette(dib);
	unsigned x, y, d;
	if (bpp == 24)
	{
		for (y = 0; y < height; y++)
		{
			lines = bits + y * pitch;
			for (x = 0; x < width; x++)
			{
				for (d = 0; d < 3; d++)
				{
					p_im[y][x].c[d] = lines[x * 3 + d];
				}
			}
		}
	} else if (bpp == 8) {
		for ( y = 0; y < height; y++ )
		{
			lines = bits + y * pitch;
			for ( x = 0; x < width; x++ )
			{
				p_im[y][x] = IMTset(dibpal[lines[x]].rgbBlue, dibpal[lines[x]].rgbGreen, dibpal[lines[x]].rgbRed);
			}
		}
	} else {
		for ( y = 0; y < height; y++ )
		{
			lines = bits + y * pitch;
			for ( x = 0; x < width; x++ )
			{
				d = ImthresholdGet1BitPixel(lines, x);
				p_im[y][x] = IMTset(dibpal[d].rgbBlue, dibpal[d].rgbGreen, dibpal[d].rgbRed);
			}
		}
	}
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			p_im[y][x].s = 0;
			for (d = 0; d < 3; d++)
			{
				p_im[y][x].s += (WORD)p_im[y][x].c[d];
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdSetData(FIBITMAP* dib, IMTpixel** p_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	unsigned bpp = FreeImage_GetBPP(dib);
	unsigned btpp = bpp/8;
	BYTE* bits = (BYTE*)FreeImage_GetBits(dib);
	BYTE* lined;
	unsigned y, x, d;
	BYTE val;
	for (y = 0; y < height; y++)
	{
		lined = bits + y * pitch;
		for (x = 0; x < width; x++)
		{
			for (d = 0; d < btpp; d++)
			{
				val = (BYTE)p_im[y][x].c[d];
				lined[x * btpp + d] = val;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdGetDataBW(FIBITMAP* dib, BYTE** p_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	BYTE* bits = (BYTE*)FreeImage_GetBits(dib);
	BYTE* lines;
	RGBQUAD *pal = FreeImage_GetPalette(dib);
	int i;
	unsigned x, y;
	for ( y = 0; y < height; y++ )
	{
		lines = bits + y * pitch;
		for ( x = 0; x < width; x++ )
		{
			i = ImthresholdGet1BitPixel(lines, x);
			p_im[y][x] = pal[i].rgbRed;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdSetDataBW(FIBITMAP* dib, BYTE** d_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	BYTE* bits = (BYTE*)FreeImage_GetBits(dib);
	RGBQUAD *pal = FreeImage_GetPalette(dib);
	pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 255;
	pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 0;
	BYTE* lined;
	unsigned y, x;
	BYTE val;
	for (y = 0; y < height; y++)
	{
		lined = bits + y * pitch;
		for (x = 0; x < width; x++)
		{
			val = d_im[y][x];
			ImthresholdSetPixel(lined, x, &val);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* ImthresholdFilterNone(FIBITMAP* src_dib)
{
	if (!src_dib) return false;
	FIBITMAP *tmp = FreeImage_Clone(src_dib);
	return tmp;
}

////////////////////////////////////////////////////////////////////////////////
/**
FreeImage error handler
@param fif Format / Plugin responsible for the error 
@param message Error message
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	printf("\n*** "); 
	printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	printf(message);
	printf(" ***\n");
}

////////////////////////////////////////////////////////////////////////////////

/** Generic image loader

  @param lpszPathName Pointer to the full file name
  @param flag Optional load flag constant
  @return Returns the loaded dib if successful, returns NULL otherwise
*/

FIBITMAP* ImthresholdGenericLoader(const char* lpszPathName, int flag)
{	
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	
	fif = FreeImage_GetFileType(lpszPathName, 0);
	
	FIBITMAP* dib;
	
	if(fif == FIF_UNKNOWN)
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
		// ok, let's load the file
		dib = FreeImage_Load(fif, lpszPathName, flag);
		
		// unless a bad file format, we are done !
		if (!dib)
		{
			printf("%s%s%s\n","File \"", lpszPathName, "\" not found.");
			return NULL;
		}
	}	
	
	return dib;
}

////////////////////////////////////////////////////////////////////////////////
