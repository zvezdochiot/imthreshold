/*
*
* Copyright (C) 2005 John Ashley Burgoyne and Ichiro Fujinaga
*               2007 Uma Kompella and Christoph Dalitz
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
* 
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
 Level image using Niblack's adaptive algorithm.

 Creates a binary image using Niblack's adaptive algorithm.
  
 Niblack, W. 1986. *An Introduction to Digital Image Processing.* Englewood
 Cliffs, NJ: Prentice Hall.
	
 Like the QGAR library, there are two extra global thresholds for
 the lightest and darkest regions.
	  
 *region_size* : default = 15 (*radius* : default = 7)
 The size of the region in which to calculate a threshold.
	
 *sensitivity* : default = -0.2
 The sensitivity weight on the variance.
		  
 *lower bound* : range=(0,255), default=32
 A global threshold beneath which all pixels are considered black.
			
 *upper bound* : range=(0,255), default=223
 A global threshold above which all pixels are considered white.
*/

// This algorithm was taken from the gamera.sf.net sourcecodes
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
	} else {
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

void MFilterMean(unsigned width, unsigned height, int radius, double** p_im, double** p_mean)
{
	int x, y, i, j, xt, yt;
	unsigned t;
	double sum;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			sum = 0;
			t = 0;
			for (i = -radius; i <= radius; i++)
			{
				yt = y + i;
				if (y >= 0 && yt < height)
				{
					for (j = -radius; j <= radius; j++)
					{			
						xt = x + j;
						if (xt >= 0 && xt < width)
						{
							sum += p_im[yt][xt];
							t++;
						}
					}
				}
			}
			if (t > 0)
			{
				p_mean[y][x] = sum / t;
			} else {
				p_mean[y][x] = sum;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void MFilterVariance(unsigned width, unsigned height, int radius, double** p_im, double** p_mean, double** p_var)
{
	int y, x, i, j, xt, yt;
	unsigned t;
	double sum;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			sum = 0;
			t = 0;
			for (i = -radius; i <= radius; i++)
			{
				yt = y + i;
				if (y >= 0 && yt < height)
				{
					for (j = -radius; j <= radius; j++)
					{			
						xt = x + j;
						if (xt >= 0 && xt < width)
						{
							sum += p_im[yt][xt] * p_im[yt][xt];
							t++;
						}
					}
				}
			}
			if (t > 0)
			{
				p_var[y][x] = sum / t - p_mean[y][x] * p_mean[y][x];
			} else {
				p_var[y][x] = sum - p_mean[y][x] * p_mean[y][x];
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

FIBITMAP* ImthresholdFilterLevelNiblack(FIBITMAP* src_dib, int radius, double sensitivity, int lower_bound, int upper_bound, double kbx)
{
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
	
	if (radius < 0) {radius = -radius;}
	printf("Radius= %d\n", radius);
	printf("Sensitivity= %f\n", sensitivity);
	printf("Lower= %d\n", lower_bound);
	printf("Upper= %d\n", upper_bound);
	printf("Kbound= %f\n", kbx);
	
	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, 24);
	
	unsigned dst_pitch = FreeImage_GetPitch(dst_dib);
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib);
	BYTE* lined;

	unsigned x, y, d;
	double imx, threshold, mean, deviation;
	BYTE val;	
	int d_bound = upper_bound - lower_bound;
	double valt, valk, lbx, ubx;
	lbx = (1.0 + kbx) * (double)lower_bound;
	ubx = 255.0 - (1.0 + kbx) * (255.0 - (double)upper_bound);

    IMTpixel** c_im;
    c_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
    for (y = 0; y < height; y++) {c_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
	
	ImthresholdGetData(src_dib, c_im);
	
    double** p_im;
    p_im = (double**)malloc(height * sizeof(double*));
    for (y = 0; y < height; y++) {p_im[y] = (double*)malloc(width * sizeof(double));}
    double** p_mean;
    p_mean = (double**)malloc(height * sizeof(double*));
    for (y = 0; y < height; y++) {p_mean[y] = (double*)malloc(width * sizeof(double));}
    double** p_var;
    p_var = (double**)malloc(height * sizeof(double*));
    for (y = 0; y < height; y++) {p_var[y] = (double*)malloc(width * sizeof(double));}
	
    for (y = 0; y < height; y++)
	{
        for (x = 0; x < width; x++)
		{
			imx = (double)c_im[y][x].s / 3;
			p_im[y][x] = imx;
		}
	}

	MFilterMean(width, height, radius, p_im, p_mean);	
	MFilterVariance(width, height, radius, p_im, p_mean, p_var);	
	
    for (y = 0; y < height; y++)
	{
		lined = dst_bits + y * dst_pitch;
        for (x = 0; x < width; x++)
		{
			mean = p_mean[y][x];
			deviation = sqrt(p_var[y][x]);
			imx = p_im[y][x];
			if (imx < lower_bound)
			{
				val = 0;
			} else if (imx > upper_bound)
			{
				val = 255;
			} else {				
				threshold = (mean + sensitivity * deviation);
				val = (BYTE) ( ( imx >= threshold ) ? 255 : 0 );
			}
			valk = 1;
			if (imx > 0)
			{
				if (val == 0)
				{
					if (imx < lbx)
					{
						imx /= (1.0 + kbx);
						imx /= lower_bound;
						imx = imx * imx;
						imx *= kbx;
						imx *= lower_bound;						
						imx += lower_bound;
					}
					if (imx > ubx)
					{
						imx -= 255.0;
						imx /= (1.0 + kbx);
						imx /= (255.0 - upper_bound);
						imx = imx * imx;
						imx *= kbx;
						imx *= (255.0 - upper_bound);
						imx = upper_bound - imx;
					}
				}
				imx -= lower_bound;
				imx *= 255;
				imx /= d_bound;
				valk = imx / p_im[y][x];
			}
			for (d = 0; d < 3; d++)
			{
				valt = (double)c_im[y][x].c[d];
				valt *= valk;
				if (valt < 0) {valt = 0;}
				if (valt > 255) {valt = 255;}
				lined[x * 3 + d] = (BYTE)(valt + 0.5);
			}
        }
    }
	
	FreeImage_SetDotsPerMeterX(dst_dib, FreeImage_GetDotsPerMeterX(src_dib));
	FreeImage_SetDotsPerMeterY(dst_dib, FreeImage_GetDotsPerMeterY(src_dib));
	
    for (y = 0; y < height; y++) {free(c_im[y]);}
	free(c_im);
    for (y = 0; y < height; y++) {free(p_im[y]);}
	free(p_im);
    for (y = 0; y < height; y++) {free(p_mean[y]);}
	free(p_mean);
    for (y = 0; y < height; y++) {free(p_var[y]);}
	free(p_var);
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

void ImthresholdFilterLevelNiblackTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Level image using Niblack's adaptive algorithm.\n");
	printf("This algorithm was taken from the gamera.sf.net sourcecodes and adopted for the FreeImage library.\n\n");
}

void ImthresholdFilterLevelNiblackUsage()
{
	printf("Usage : imthreshold-flevelnib [options] <input_file> <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -r N    radius (int, optional, default = 7)\n");
	printf("          -s N.N  sensitivity (double, optional, default = -0.2)\n");
	printf("          -l N    lower bound (int, optional, default = 16)\n");
	printf("          -u N    upper bound (int, optional, default = 239)\n");
	printf("          -k N.N  k bound (int, optional, default = 0.25)\n");
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
	int radius = 7;
	double sensitivity = -0.2;
	int lower_bound = 16;
	int upper_bound = 239;
	double kbx = 0.25;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":r:s:l:u:k:h")) != -1)
	{
		switch(opt)
		{
			case 'r':
				radius = atof(optarg);
				break;
			case 's':
				sensitivity = atof(optarg);
				break;
			case 'l':
				lower_bound = atof(optarg);
				break;
			case 'u':
				upper_bound = atof(optarg);
				break;
			case 'k':
				kbx = atof(optarg);
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
	
	ImthresholdFilterLevelNiblackTitle();
	
	if(optind + 2 > argc || fhelp > 0 || radius <= 0)
	{
		ImthresholdFilterLevelNiblackUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	if (upper_bound < lower_bound)
	{
		upper_bound += lower_bound;
		lower_bound = upper_bound - lower_bound;
		upper_bound -= lower_bound;
	}
	
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 1 || FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib;
				if (FreeImage_GetBPP(dib) == 1)
				{
					dst_dib = ImthresholdFilterNone(dib);
				}
				else
				{
					dst_dib = ImthresholdFilterLevelNiblack(dib, radius, sensitivity, lower_bound, upper_bound, kbx);
				}
				
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
