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

// Equalizer filter.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
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

inline BYTE ImthresholdGet1BitPixel(BYTE *bits, unsigned x)
{
	return (bits[x >> 3] & (0x80 >> (x & 0x07))) != 0 ? 255 : 0;
}

////////////////////////////////////////////////////////////////////////////////

inline void ImthresholdSetPixel(BYTE *bits, unsigned x, BYTE* value)
{
	*value ? bits[x >> 3] |= (0x80 >> (x & 0x7)) : bits[x >> 3] &= (0xFF7F >> (x & 0x7));
}

////////////////////////////////////////////////////////////////////////////////

int ImthresholdGetData(FIBITMAP* dib, IMTpixel** p_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	unsigned bpp = FreeImage_GetBPP(dib);
	unsigned btpp = bpp/8;
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
				p_im[y][x].c[0] = dibpal[lines[x]].rgbBlue;
				p_im[y][x].c[1] = dibpal[lines[x]].rgbGreen;
				p_im[y][x].c[2] = dibpal[lines[x]].rgbRed;
			}
		}
	} else {
		for ( y = 0; y < height; y++ )
		{
			lines = bits + y * pitch;
			for ( x = 0; x < width; x++ )
			{
				if (ImthresholdGet1BitPixel(lines, x))
				{
					p_im[y][x].c[0] = 255;
					p_im[y][x].c[1] = 255;
					p_im[y][x].c[2] = 255;
				} else {
					p_im[y][x].c[0] = 0;
					p_im[y][x].c[1] = 0;
					p_im[y][x].c[2] = 0;
				}
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

FIBITMAP* ImthresholdFilterNone(FIBITMAP* src_dib)
{
	if (!src_dib) return false;
	FIBITMAP *tmp = FreeImage_Clone(src_dib);
	printf("Copy image.\n");
	return tmp;
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* ImthresholdFilterEqv5(FIBITMAP* src_dib, double a1, double a2, double a3, double a4, double a5)
{
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
		
	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, 24);
	
	unsigned dst_pitch = FreeImage_GetPitch(dst_dib);
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib);
	BYTE* lined;
	int x, y, i, j;
	unsigned d;
	int im;
	double imx, imd, fr, sir, sr, km, val;
	double a[5] = {0};
	double hist[768] = {0};
	
    IMTpixel** p_im;
    p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
    for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
    
    printf("Eqv5= %f %f %f %f %f\n", a1, a2, a3, a4, a5);

	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	a[3] = a4;
	a[4] = a5;
		
	for (j = 0; j < 766; j++)
	{
		imx = (double)j / 765.0 * 4.0;
		sir = 0;
		sr = 0;
		for (i = 0; i < 5; i++)
		{
			imd = imx - (double)i;
			if (imd < 0) {imd = -imd;}
			fr = 1.0 / (imd + 0.1);
			sir += a[i] * fr;
			sr += fr;
		}
		hist[j] = sir /sr;
	}
	
	ImthresholdGetData(src_dib, p_im);

    for (y = 0; y < height; y++)
	{
		lined = dst_bits + y * dst_pitch;
        for (x = 0; x < width; x++)
		{
			im = p_im[y][x].s;
			km = hist[im];
			for (d = 0; d < 3; d++)
			{
				val = p_im[y][x].c[d];
				val *= km;
				if (val < 0) {val = 0;}
				if (val > 255) {val = 255;}
				lined[x * 3 + d] = (BYTE)(val + 0.5);
			}
        }
    }
	
	FreeImage_SetDotsPerMeterX(dst_dib, FreeImage_GetDotsPerMeterX(src_dib));
	FreeImage_SetDotsPerMeterY(dst_dib, FreeImage_GetDotsPerMeterY(src_dib));

    for (y = 0; y < height; y++) {free(p_im[y]);}
	free(p_im);
	return dst_dib;
}

////////////////////////////////////////////////////////////////////////////////
/**
FreeImage error handler
@param fif Format / Plugin responsible for the error 
@param message Error message
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
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
	fif = FreeImage_GetFileType(lpszPathName, 0);
	FIBITMAP* dib;
	if(fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, lpszPathName, flag);
		if (!dib)
		{
			printf("%s%s%s\n","File \"", lpszPathName, "\" not found.");
			return NULL;
		}
	}	
	return dib;
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterEqv5Title()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Equalizer 5 filter.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterEqv5Usage()
{
	printf("Usage : imthreshold-feqv5 [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -1 N.N  eq value 1 (double, optional, default = 1.0)\n");
	printf("          -2 N.N  eq value 2 (double, optional, default = 1.0)\n");
	printf("          -3 N.N  eq value 3 (double, optional, default = 1.0)\n");
	printf("          -4 N.N  eq value 4 (double, optional, default = 1.0)\n");
	printf("          -5 N.N  eq value 5 (double, optional, default = 1.0)\n");
	printf("          -h      this help\n");
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB
	
	int opt;
	double a1 = 1.0;
	double a2 = 1.0;
	double a3 = 1.0;
	double a4 = 1.0;
	double a5 = 1.0;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":1:2:3:4:5:h")) != -1)
	{
		switch(opt)
		{
			case '1':
				a1 = atof(optarg);
				break;
			case '2':
				a2 = atof(optarg);
				break;
			case '3':
				a3 = atof(optarg);
				break;
			case '4':
				a4 = atof(optarg);
				break;
			case '5':
				a5 = atof(optarg);
				break;
			case 'h':
				fhelp = 1;
				break;
			case ':':
				printf("option needs a value\n");
				break;
			case '?':
				printf("unknown option: %c\n", optopt);
				break;
		}
	}
	
	ImthresholdFilterEqv5Title();
	
	if(optind + 2 > argc || fhelp > 0)
	{
		ImthresholdFilterEqv5Usage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 1 || FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib;
				dst_dib = ImthresholdFilterEqv5(dib, a1, a2, a3, a4, a5);
				
				if (dst_dib)
				{					
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, dst_dib, output_filename, 0);
					}
					FreeImage_Unload(dst_dib);					
					printf("Output= %s\n\n", output_filename);
				}
			} else {
				printf("%s\n", "Unsupported color mode.");
			}
		} else {
			printf("%s\n", "Unsupported color mode.");
		}
		FreeImage_Unload(dib);
	}	 
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
	
	return 0;
}
