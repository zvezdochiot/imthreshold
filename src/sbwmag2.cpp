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

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

// Magnification BW image.

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSBWMag2Title()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Magnification BW image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterSBWMag2Usage()
{
	printf("Usage : imthreshold-sbwmag2 [options] <input_file>(BW) <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -r      reduce (bool, optional, default = false)\n");
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
	bool freduce = 0;
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":rh")) != -1)
	{
		switch(opt)
		{
			case 'r':
				freduce = true;
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
	
	ImthresholdFilterSBWMag2Title();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterSBWMag2Usage();
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
			if (FreeImage_GetBPP(dib) == 1)
			{
				FIBITMAP* dst_dib;
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned width2, height2;
				unsigned y;
				if (freduce)
				{
					width2 = (width + 1) / 2;
					height2 = (height + 1) / 2;
				} else {
					width2 = width * 2;
					height2 = height * 2;
				}
				printf("Width= %d\n", width2);
				printf("Height= %d\n", height2);
				BYTE** d_im;
				d_im = (BYTE**)malloc(height * sizeof(BYTE*));
				for (y = 0; y < height; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}
				BYTE** r_im;
				r_im = (BYTE**)malloc(height2 * sizeof(BYTE*));
				for (y = 0; y < height2; y++) {r_im[y] = (BYTE*)malloc(width2 * sizeof(BYTE));}

				ImthresholdGetDataBW(dib, d_im);
				FreeImage_Unload(dib);
				if (freduce)
				{
					threshold = IMTFilterSBWReduce2(d_im, r_im, height, width, height2, width2);
					printf("Reduce= %d\n", threshold);
				} else {
					threshold = IMTFilterSBWMag2(d_im, r_im, height, width, height2, width2);
					printf("Mag= %d\n", threshold);
				}

				for (y = 0; y < height; y++){free(d_im[y]);}
				free(d_im);
				dst_dib = FreeImage_Allocate(width2, height2, 1);
				ImthresholdSetDataBW(dst_dib, r_im);
				for (y = 0; y < height2; y++){free(r_im[y]);}
				free(r_im);
				
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
				printf("%s\n", "Unsupported color mode.");
				FreeImage_Unload(dib);
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
