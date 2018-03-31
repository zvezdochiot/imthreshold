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

#include <FreeImage.h>
#include "Utilities.h"
#include <unistd.h>

// Grey separated image.

#define MAXVAL 256
typedef struct mpixel
{
	BYTE c[3];
	WORD s;
}
IMTpixel;

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTset(BYTE c0, BYTE c1, BYTE c2)
{
	IMTpixel im;
	
	im.c[0] = (BYTE)c0;
	im.c[1] = (BYTE)c1;
	im.c[2] = (BYTE)c2;
	im.s = (WORD)c0 + (WORD)c1 + (WORD)c2;
		
	return im;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiMod(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius)
{
	int y, x, y0, x0, yk, xk, i, j;
	unsigned im, imf;
	double T, Tw, Tb, Tn, TG, TM, iw, ib, ST = 0, TN = 0;
	BYTE val;
	
	if (radius < 0) {radius = -radius;}
	
	int threshold = 0;

	TG = 384.0;
	Tn = 0;
	while ( TG != Tn )
	{
		Tn = TG;
		Tb = 0;
		Tw = 0;
		ib = 0;
		iw = 0;
		for (y = 0; y < height; y++ )
		{
			for (x = 0; x < width; x++)
			{
				im = p_im[y][x].s;
				if ( im > TG)
				{
					Tw += im;
					iw++;
				} else {
					Tb += im;
					ib++;
				}
			}
		}
		if (iw == 0 && ib == 0)
		{
			 TG = Tn;
		} else if (iw == 0) {
			TG = Tb/ib;
		} else if (ib == 0) {
			TG = Tw/iw;
		} else {
			TG = ((Tw/iw) + (Tb/ib)) / 2.0;
		}
	}
	threshold = (int)(TG+0.5);
			
	if (radius == 0)
	{
		T = TG;
		for (y = 0; y < height; y++ )
		{
			for (x = 0; x < width; x++)
			{
				im = p_im[y][x].s;
				val = (BYTE) ( ( im >= T ) ? 255 : 0 );
				d_im[y][x] = val;
			}
		}
	} else {
		for ( y = 0; y < height; y++ )
		{
			y0 = y - radius;
			if (y0 < 0) {y0 = 0;}
			yk = y + radius;
			if (yk > height - 1) {yk = height - 1;}
			for ( x = 0; x < width; x++)
			{
				x0 = x - radius;
				if (x0 < 0) {x0 = 0;}
				xk = x + radius;
				if (xk > width - 1) {xk = width - 1;}
				T = double(MAXVAL)/2.0;
				Tn = 0;
				while ( T != Tn )
				{
					for ( i = y0; i <= yk; i++ )
					{
						for ( j = x0; j <= xk; j++ )
						{
							imf = p_im[i][j].s;
							Tn = T;
							Tb = 0;
							Tw = 0;
							ib = 0;
							iw = 0;
							if ( imf > T)
							{
								Tw += imf;
								iw++;
							} else {
								Tb += imf;
								ib++;
							}
						}
					}
					if (iw == 0 && ib == 0)
					{
						T = Tn;
					} else if (iw == 0) {
						T= Tb/ib;
					} else if (ib == 0) {
						T = Tw/iw;
					} else {
						T = ((Tw/iw) + (Tb/ib)) / 2.0;
					}
				}
				ST += T;
				TN++;
				TM = (T+TG)/2.0;
				threshold = (int)(TM+0.5);
				im = p_im[y][x].s;
				val = (BYTE) ( ( im >= TM ) ? 255 : 0 );
				d_im[y][x] = val;
			}
		}
		if (TN > 0)
		{
			threshold = (int)(ST/TN+0.5);
		} else {
			threshold = (int)(TG+0.5);
		}
	}
	return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTSimple(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int threshold)
{
	unsigned y, x, T, im;
	BYTE val;
	
	T = threshold;
	for (y = 0; y < height; y++ )
	{
		for (x = 0; x < width; x++)
		{
			im = p_im[y][x].s;
			val = (BYTE) (( im >= T ) ? 255 : 0 );
			d_im[y][x] = val;
		}
	}
	return threshold;
}

////////////////////////////////////////////////////////////////////////////////

void IMTGreyMask(IMTpixel** p_im, unsigned height, unsigned width)
{
	unsigned y, x, d;
	int ct, cm, cs;
	for (y = 0; y < height; y++ )
	{
		for (x = 0; x < width; x++)
		{
			cm = p_im[y][x].s;
			cs = 0;
			for (d = 0; d < 3; d++)
			{
				ct = p_im[y][x].c[d];
				ct *= 3;
				ct -= cm;
				if (ct < 0) {ct = -ct;}
				cs += ct;
			}
			p_im[y][x].s = cs;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterGreySep(IMTpixel** p_im, IMTpixel** f_im, IMTpixel** b_im, unsigned height, unsigned width, int threshold)
{
	unsigned y, x, d;
	BYTE val;
	BYTE** d_im;
	d_im = (BYTE**)malloc(height * sizeof(BYTE*));
	for (y = 0; y < height; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}
	
	IMTGreyMask(p_im, height, width);
	
	if (threshold > 0)
	{
		threshold = IMTFilterTSimple(p_im, d_im, height, width, threshold);
	} else {
		threshold = IMTFilterTBiMod(p_im, d_im, height, width, 0);
	}
	
	for (y = 0; y < height; y++ )
	{
		for (x = 0; x < width; x++)
		{
			if (d_im[y][x] > 0)
			{
				f_im[y][x] = IMTset(255, 255, 255);
				b_im[y][x] = p_im[y][x];
			} else {
				f_im[y][x] = p_im[y][x];
				b_im[y][x] = IMTset(255, 255, 255);
			}
		}
	}
	return threshold;

	for (y = 0; y < height; y++){free(d_im[y]);}
	free(d_im);
}

////////////////////////////////////////////////////////////////////////////////

inline BYTE ImthresholdGet1BitPixel(BYTE *bits, unsigned x)
{
	return (bits[x >> 3] & (0x80 >> (x & 0x07))) != 0 ? 255 : 0;
}

////////////////////////////////////////////////////////////////////////////////

inline void ImthresholdSetPixel(BYTE *bits, unsigned x, BYTE* value)
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
	BYTE tpal;
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
				if (ImthresholdGet1BitPixel(lines, x))
				{
					tpal = dibpal[1].rgbBlue;
				} else {
					tpal = dibpal[0].rgbBlue;
				}
				p_im[y][x] = IMTset(tpal, tpal, tpal);
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

void ImthresholdSetDataBW(FIBITMAP* dib, BYTE** d_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	unsigned bpp = FreeImage_GetBPP(dib);
	unsigned btpp = bpp/8;
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

FIBITMAP* ImthresholdFilterNone(FIBITMAP* src_dib)
{
	if (!src_dib) return false;
	FIBITMAP *tmp = FreeImage_Clone(src_dib);
	printf("Copy image.\n");
	return tmp;
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

void ImthresholdFilterGraySepTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Grey separated image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterGraySepUsage()
{
	printf("Usage : imthreshold-fgreysep [options] <input_file> <output_file_FG> <output_file_BG>\n\n");
	printf("options:\n");
	printf("          -t N    threshold (int, optional, default = 0[auto])\n");
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
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":t:h")) != -1)
	{
		switch(opt)
		{
			case 't':
				threshold = atof(optarg);
				break;
			case 'h':
				fhelp = true;
				break;
			case ':':
				printf("option needs a value\n");
				break;
			case '?':
				printf("unknown option: %c\n", optopt);
				break;
		}
	}
	
	ImthresholdFilterGraySepTitle();
	
	if(optind + 3 > argc || fhelp)
	{
		ImthresholdFilterGraySepUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *outputfg_filename = argv[optind + 1];
	const char *outputbg_filename = argv[optind + 2];
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 1 || FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* fg_dib;
				FIBITMAP* bg_dib;
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned y;
				fg_dib = FreeImage_Allocate(width, height, 24);
				bg_dib = FreeImage_Allocate(width, height, 24);
				IMTpixel** p_im;
				p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
				IMTpixel** f_im;
				f_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {f_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
				IMTpixel** b_im;
				b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

				ImthresholdGetData(dib, p_im);
				threshold = IMTFilterGreySep(p_im, f_im, b_im, height, width, threshold);
				printf("Threshold= %d\n", threshold);
				ImthresholdSetData(fg_dib, f_im);
				ImthresholdSetData(bg_dib, b_im);
				
				for (y = 0; y < height; y++){free(b_im[y]);}
				free(b_im);
				for (y = 0; y < height; y++){free(f_im[y]);}
				free(f_im);
				for (y = 0; y < height; y++){free(p_im[y]);}
				free(p_im);
				
				if (fg_dib)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(outputfg_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, fg_dib, outputfg_filename, 0);
						printf("Output= %s\n", outputfg_filename);
					}
					FreeImage_Unload(fg_dib);
				}
				if (bg_dib)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(outputbg_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, bg_dib, outputbg_filename, 0);
						printf("Output= %s\n", outputbg_filename);
					}
					FreeImage_Unload(bg_dib);
				}
				printf("\n");
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
