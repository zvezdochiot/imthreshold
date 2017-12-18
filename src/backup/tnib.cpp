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
 niblack_threshold

 Creates a binary image using Niblack's adaptive algorithm.
  
 Niblack, W. 1986. *An Introduction to Digital Image Processing.* Englewood
 Cliffs, NJ: Prentice Hall.
	
 Like the QGAR library, there are two extra global thresholds for
 the lightest and darkest regions.
	  
 *region_size* : default = 15 (*radius* : default = 7)
 The size of the region in which to calculate a threshold.
	
 *sensitivity* : default = -0.2
 The sensitivity weight on the variance.
		  
 *lower bound* : range=(0,255), default=20
 A global threshold beneath which all pixels are considered black.
			
 *upper bound* : range=(0,255), default=150
 A global threshold above which all pixels are considered white.
*/

// This algorithm was taken from the gamera.sf.net sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTNiblackTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Creates a binary image using Niblack's adaptive algorithm.\n");
	printf("This algorithm was taken from the gamera.sf.net sourcecodes and adopted for the FreeImage library.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTNiblackUsage()
{
	printf("Usage : imthreshold-tnib [options] <input_file> <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -r N    radius (int, optional, default = 7)\n");
	printf("          -s N.N  sensitivity (double, optional, default = -0.2)\n");
	printf("          -l N    lower bound (int, optional, default = 20)\n");
	printf("          -u N    upper bound (int, optional, default = 150)\n");
	printf("          -d N.N  delta (double, optional, default = 5.0)\n");
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
	int lower_bound = 20;
	int upper_bound = 150;
	double delta = 5.0;
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":r:s:l:u:d:h")) != -1)
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
			case 'd':
				delta = atof(optarg);
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
	
	ImthresholdFilterTNiblackTitle();
	
	if(optind + 2 > argc || fhelp || radius <= 0)
	{
		ImthresholdFilterTNiblackUsage();
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
			FIBITMAP* dst_dib;
			unsigned width = FreeImage_GetWidth(dib);
			unsigned height = FreeImage_GetHeight(dib);
			unsigned y;

			IMTpixel** p_im;
			p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			BYTE** d_im;
			d_im = (BYTE**)malloc(height * sizeof(BYTE*));
			for (y = 0; y < height; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

			printf("Radius= %d\n", radius);
			printf("Sensitivity= %f\n", sensitivity);
			printf("Lower= %d\n", lower_bound);
			printf("Upper= %d\n", upper_bound);
			printf("Delta= %f\n", delta);

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			threshold = IMTFilterTNiblack(p_im, d_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
			printf("Threshold= %d\n", threshold / 3);
			for (y = 0; y < height; y++){free(p_im[y]);}
			free(p_im);
			dst_dib = FreeImage_Allocate(width, height, 1);
			ImthresholdSetDataBW(dst_dib, d_im);
			for (y = 0; y < height; y++){free(d_im[y]);}
			free(d_im);
			
			if (dst_dib)
			{					
				FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
				if(out_fif != FIF_UNKNOWN)
				{
					FreeImage_Save(out_fif, dst_dib, output_filename, 0);
					printf("Output= %s\n\n", output_filename);
				}
				FreeImage_Unload(dst_dib);					
			}
		} else {
			printf("%s\n", "Unsupported format type.");
			FreeImage_Unload(dib);
		}
	}	 
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
	
	return 0;
}
