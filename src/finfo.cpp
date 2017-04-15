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

// Info image.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterInfoTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Info image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterInfoUsage()
{
	printf("Usage : imthreshold-finfo [options] <input_file> [output_file]\n\n");
	printf("options:\n");
	printf("          -q      quiet, copy only (bool, optional)\n");
	printf("          -h      this help\n");
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB
	
	int opt;
	bool fcpo = false;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":qh")) != -1)
	{
		switch(opt)
		{
			case 'q':
				fcpo = true;
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
	
	ImthresholdFilterInfoTitle();
	
	if(optind + 1 > argc || fhelp)
	{
		ImthresholdFilterInfoUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];		

	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	int res = 0;
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			printf("Name= %s\n", src_filename);
			if (!fcpo)
			{
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned bpp = FreeImage_GetBPP(dib);
				IMTinfo p_info;
				unsigned y;

				IMTpixel** p_im;
				p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

				ImthresholdGetData(dib, p_im);
				FreeImage_Unload(dib);
				
				p_info = IMTFilterInfo(p_im, height, width, bpp);
				printf("Height= %d\n", p_info.height);
				printf("Width= %d\n", p_info.width);
				printf("Bits= %d\n", p_info.bpp);
				printf("Min= %d\n", p_info.min);
				printf("Max= %d\n", p_info.max);
				printf("Mid= %f\n", p_info.mid);
				printf("Mean= %f\n", p_info.mean);
				printf("Std= %f\n", p_info.std);
				printf("W/B= %f\n", p_info.wb);
				
				for (y = 0; y < height; y++){free(p_im[y]);}
				free(p_im);
			}
			if (output_filename)
			{
				FIBITMAP* dst_dib;
				dst_dib = ImthresholdFilterNone(dib);
				if (dst_dib)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, dst_dib, output_filename, 0);
						printf("Copy= %s\n", output_filename);
					}
					FreeImage_Unload(dst_dib);
				}
			}
		} else {
			printf("%s\n", "Unsupported format type.");
			FreeImage_Unload(dib);
		}
	}
	printf("\n");
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
	
	return res;
}

