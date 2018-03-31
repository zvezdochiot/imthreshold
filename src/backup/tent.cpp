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

/*
* DAVID
* 
* This file modified by David Foster to improve clarity.
*/

/**
* Automatic thresholding technique based on the entopy of the histogram.
* See: P.K. Sahoo, S. Soltani, K.C. Wong and, Y.C. Chen "A Survey of
* Thresholding Techniques", Computer Vision, Graphics, and Image
* Processing, Vol. 41, pp.233-260, 1988.
*
* @author Jarek Sacha
*/

/*
This plugin does automatic thresholding based on the entopy of the histogram.
The method is very similar to Otsu's method. Rather than maximising
the inter-class variance, it maximises the inter-class entropy.
Entropy is a measure of the uncertainity of an event taking place.
You can calculate it as: S = -(sum)p*log2(p) so it is very
straightforward to do using the histogram data.
(p is the probability of a pixel greyscale value in the image,
and (sum) is the greek capital sigma. It is customary to use log in base 2. 
*/

// This algorithm was taken from the ImageJ sourcecodes
// http://rsb.info.nih.gov/ij/plugins/entropy.html
// and adopted for the FreeImage library
//
// Copyright (C) 2007-2008:
// monday2000  monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTEntTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Automatic thresholding technique based on the entopy of the histogram.\n");
	printf("This algorithm was taken from the ImageJ sourcecodes http://rsb.info.nih.gov/ij/plugins/entropy.html and adopted for the FreeImage library.\n\n");
}

void ImthresholdFilterTEntUsage()
{
	printf("Usage : imthreshold-tent [options] <input_file> <output_file>(BW)\n\n");
	printf("options:\n");
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
	while ((opt = getopt(argc, argv, ":h")) != -1)
	{
		switch(opt)
		{
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
	
	ImthresholdFilterTEntTitle();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterTEntUsage();
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

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			threshold = IMTFilterTEnt(p_im, d_im, height, width);
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
