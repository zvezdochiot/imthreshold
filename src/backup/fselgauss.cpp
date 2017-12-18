/* Selective gaussian blur filter for GIMP, version 0.1
* Adapted from the original gaussian blur filter by Spencer Kimball and
* Peter Mattis.
*
* Copyright (C) 1995 Spencer Kimball and Peter Mattis
* Copyright (C) 1999 Thom van Os <thom@vanos.com>
* Copyright (C) 2006 Loren Merritt
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
* To do:
*      - support for horizontal or vertical only blur
*      - use memory more efficiently, smaller regions at a time
*      - integrating with other convolution matrix based filters ?
*      - create more selective and adaptive filters
*      - threading
*      - optimization
*/

// This algorithm was taken from the GIMP v2.4.5 sourcecodes
// and adopted for the FreeImage library
//
//  Copyright (C) 2007-2008:
//  monday2000  monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

// Selective Gaussian Blur

// Blur neighboring pixels, but only in low-contrast areas,
// This filter functions similar to the regular
// gaussian blur filter except that neighbouring
// pixels that differ more than the given maxdelta
// parameter will not be blended with. This way with
// the correct parameters, an image can be smoothed
// out without losing details. However, this filter
// can be rather slow.
// Thom van Os,
// 1999

// double radius    // Radius of gaussian blur (in pixels, > 0)
// int max-delta  // Maximum delta

// 5.0  // default radius
// 50   // default max-delta

void ImthresholdFilterSelGaussTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Selective Gaussian Blur.\n");
	printf("This algorithm was taken from the GIMP v2.4.5 sourcecodes and adopted for the FreeImage library.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSelGaussUsage()
{
	printf("Usage : imthreshold-fselgauss [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -r N.N  radius (double, optional, default = 5.0)\n");
	printf("          -d N    max delta (int, optional, default = 50)\n");
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
	double radius = 5.0;
	int maxdelta = 50;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":r:d:h")) != -1)
	{
		switch(opt)
		{
			case 'r':
				radius = atof(optarg);
				break;
			case 'd':
				maxdelta = atof(optarg);
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
	
	ImthresholdFilterSelGaussTitle();
	
	if(optind + 2 > argc || fhelp || radius == 0 || maxdelta == 0)
	{
		ImthresholdFilterSelGaussUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];		

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

			printf("Radius= %f\n", radius);
			printf("MaxDelta= %d\n", maxdelta);
			
			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			IMTFilterSelGauss(p_im, d_im, height, width, radius, maxdelta);
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

