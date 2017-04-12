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

// Level image using mean background filter.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterLevelMeanTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Level image using mean background filter.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterLevelMeanUsage()
{
	printf("Usage : imthreshold-flevelmean [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -r N    radius (int, optional, default = 7)\n");
	printf("          -c N.N  contour factor (double, optional, default = -1[auto])\n");
	printf("          -t N.N  threshold (double, optional, default = 0.0)\n");
	printf("          -l N    lower bound (int, optional, default = 32)\n");
	printf("          -u N    upper bound (int, optional, default = 223)\n");
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
	double contour = -1.0;
	double thres = 0.0;
	int lower_bound = 32;
	int upper_bound = 223;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":r:c:t:l:u:h")) != -1)
	{
		switch(opt)
		{
			case 'r':
				radius = atof(optarg);
				break;
			case 'c':
				contour = atof(optarg);
				break;
			case 't':
				thres = atof(optarg);
				break;
			case 'l':
				lower_bound = atof(optarg);
				break;
			case 'u':
				upper_bound = atof(optarg);
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
	
	ImthresholdFilterLevelMeanTitle();
	
	if(optind + 2 > argc || fhelp > 0 || radius <= 0)
	{
		ImthresholdFilterLevelMeanUsage();
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
			
			printf("Radius= %d\n", radius);
			printf("Lower= %d\n", lower_bound);
			printf("Upper= %d\n", upper_bound);
			
			IMTpixel** p_im;
			p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			IMTpixel** d_im;
			d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			contour = IMTFilterLevelMean(p_im, d_im, height, width, radius, contour, thres, lower_bound, upper_bound);
			printf("Contour= %f\n", contour);
			printf("Threshold= %f\n", thres);
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
