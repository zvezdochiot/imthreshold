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
 Adaptive Wiener filter for de-noising.

 See:
    
 J. Lim. 2001. *Two-Dimensional Signal Processing.* Englewood
 Cliffs: Prentice Hall.

 *region_size* : default = 5

 The size of the region within which to calculate the filter
 coefficients.

 *noise_variance* : default = -1

 Variance of the noise in the image. If negative, estimated
 automatically as the median of local variances.
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

void ImthresholdFilterWienerTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Adaptive Wiener filter for de-noising.\n");
	printf("This algorithm was taken from the gamera.sf.net sourcecodes and adopted for the FreeImage library.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterWienerUsage()
{
	printf("Usage : imthreshold-fwiener [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -r N    radius (int, optional, default = 5)\n");
	printf("          -n N.N  noise size (double, optional, default = -1.0[auto])\n");
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
	int radius = 5;
	double noise = -1.0;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":r:n:h")) != -1)
	{
		switch(opt)
		{
			case 'r':
				radius = atof(optarg);
				break;
			case 'n':
				noise = atof(optarg);
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
	
	ImthresholdFilterWienerTitle();
	
	if(optind + 2 > argc || fhelp || radius <= 0)
	{
		ImthresholdFilterWienerUsage();
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
			IMTpixel** d_im;
			d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

			printf("Radius= %d\n", radius);
			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			if (noise < 0)
			{
				noise = IMTFilterNoiseVariance (p_im, height, width, radius);
			}
			printf("Noise= %f\n", noise);
			IMTFilterWiener(p_im, d_im, height, width, radius, noise);
			for (y = 0; y < height; y++){free(p_im[y]);}
			free(p_im);
			dst_dib = FreeImage_Allocate(width, height, 24);
			ImthresholdSetData(dst_dib, d_im);
			for (y = 0; y < height; y++){free(d_im[y]);}
			free(d_im);
			
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
