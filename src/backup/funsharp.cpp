/*
* Copyright (C) 1999 Winston Chang
*                    <winstonc@cs.wisc.edu>
*                    <winston@stdout.org>
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
*/

// This algorithm was taken from the GIMP v2.4.5 sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

/*
The most widely useful method for sharpening an image,
The unsharp mask is a sharpening filter that works
by comparing using the difference of the image and
a blurred version of the image.  It is commonly
used on photographic images, and is provides a much
more pleasing result than the standard sharpen
filter.
Winston Chang <winstonc@cs.wisc.edu>, 1999

	
double  radius; // Radius of gaussian blur (in pixels > 1.0)
double  amount; // Strength of effect
int     threshold; // Threshold (0-255)
	
5.0, // default radius
0.5, // default amount
0    // default threshold
*/	

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterUnsharpMaskTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("UnSharp Filter.\n");
	printf("This algorithm was taken from the GIMP v2.4.5 sourcecodes and adopted for the FreeImage library.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterUnsharpMaskUsage()
{
	printf("Usage : imthreshold-funsharp [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -r N.N  radius (double, optional, default = 5.0)\n");
	printf("          -a N.N  amount (double, optional, default = 0.5)\n");
	printf("          -t N    threshold (int, optional, default = 0)\n");
	printf("          -b      only blur (bool, optional)\n");
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
	double amount = 0.5;
	int threshold = 0;
	bool only_blur = false;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":r:a:t:bh")) != -1)
	{
		switch(opt)
		{
			case 'r':
				radius = atof(optarg);
				break;
			case 'a':
				amount = atof(optarg);
				break;
			case 't':
				threshold = atof(optarg);
				break;
			case 'b':
				only_blur = true;
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
	
	ImthresholdFilterUnsharpMaskTitle();
	
	if(optind + 2 > argc || fhelp || radius == 0)
	{
		ImthresholdFilterUnsharpMaskUsage();
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
			if (radius == 0)
			{
				dst_dib = ImthresholdFilterNone(dib);
			} else {
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned y;

				IMTpixel** p_im;
				p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
				IMTpixel** b_im;
				b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
				IMTpixel** d_im;

				printf("Radius= %f\n", radius);
				printf("Amount= %f\n", amount);
				printf("Threshold= %d\n", threshold);

				ImthresholdGetData(dib, p_im);
				FreeImage_Unload(dib);
				IMTFilterGaussBlur(p_im, b_im, height, width, radius);
				dst_dib = FreeImage_Allocate(width, height, 24);
				if (only_blur)
				{
					printf("Mode= Blur\n");
					ImthresholdSetData(dst_dib, b_im);
				} else {
					printf("Mode= Sharpen\n");
					d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
					for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
					IMTFilterUnsharpMask(p_im, b_im, d_im, height, width, amount, threshold);
					ImthresholdSetData(dst_dib, d_im);
					for (y = 0; y < height; y++){free(d_im[y]);}
					free(d_im);
				}
				for (y = 0; y < height; y++){free(b_im[y]);}
				free(b_im);
				for (y = 0; y < height; y++){free(p_im[y]);}
				free(p_im);
			}
			
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
