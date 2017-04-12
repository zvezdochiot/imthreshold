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

// Text thresholding image.

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTTextTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Text thresholding image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterTTextUsage()
{
	printf("Usage : imthreshold-ttext [options] <input_file> <output_file>(BW) [textg_file]\n\n");
	printf("options:\n");
	printf("          -c N    contour amplitude (int, optional, default = 5)\n");
	printf("          -r N    radius (int, optional, default = 5)\n");
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
	int contour = 5;
	int radius = 5;
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":c:r:h")) != -1)
	{
		switch(opt)
		{
			case 'c':
				contour = atof(optarg);
				break;
			case 'r':
				radius = atof(optarg);
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
	
	ImthresholdFilterTTextTitle();
	
	if(optind + 2 > argc || contour < 1 || radius < 1 || fhelp)
	{
		ImthresholdFilterTTextUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];
	const char *txtg_filename;
	if(optind + 2 < argc)
	{
		txtg_filename = argv[optind + 2];
	}
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			FIBITMAP* dst_dib;
			FIBITMAP* txt_dib;
			unsigned width = FreeImage_GetWidth(dib);
			unsigned height = FreeImage_GetHeight(dib);
			unsigned y, x;
			IMTpixel** p_im;
			p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			BYTE** d_im;
			d_im = (BYTE**)malloc(height * sizeof(BYTE*));
			for (y = 0; y < height; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

			printf("Contour= %d\n", contour);
			printf("Radius= %d\n", radius);
			
			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			threshold = IMTFilterTText(p_im, d_im, height, width, contour, radius);
			printf("Threshold= %d\n", threshold / 3);
			dst_dib = FreeImage_Allocate(width, height, 1);
			ImthresholdSetDataBW(dst_dib, d_im);
			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					if (d_im[y][x] == 0)
					{
						p_im[y][x] = IMTset(255, 255, 255);
					}
				}
			}
			for (y = 0; y < height; y++){free(d_im[y]);}
			free(d_im);
			txt_dib = FreeImage_Allocate(width, height, 24);
			ImthresholdSetData(txt_dib, p_im);
			for (y = 0; y < height; y++){free(p_im[y]);}
			free(p_im);
			
			if (dst_dib)
			{
				FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
				if(out_fif != FIF_UNKNOWN)
				{
					FreeImage_Save(out_fif, dst_dib, output_filename, 0);
					printf("Output= %s\n", output_filename);
				}
				FreeImage_Unload(dst_dib);
			}
			if (txt_dib)
			{
				if(optind + 2 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(txtg_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, txt_dib, txtg_filename, 0);
						printf("Output= %s\n", txtg_filename);
					}
				}
				FreeImage_Unload(txt_dib);					
			}
			printf("\n");
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
