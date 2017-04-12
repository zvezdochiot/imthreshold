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

// Half Reverse Interpolate Scale image (HRIS).

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSHRISTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Half Reverse Interpolate Scale image (HRIS).\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterSHRISUsage()
{
	printf("Usage : imthreshold-shris [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -m      mode (int, optional, default = 2, {2,3})\n");
	printf("          -r      reduce scale (bool, optional)\n");
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
	int smode = 2;
	bool reduce = false;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":m:rh")) != -1)
	{
		switch(opt)
		{
			case 'm':
				smode = atof(optarg);
				break;
			case 'r':
				reduce = true;
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
	
	ImthresholdFilterSHRISTitle();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterSHRISUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];		

	if (smode < 2) {smode = 2;}
	if (smode > 3) {smode = 3;}

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
			unsigned width2;
			unsigned height2;
			unsigned y;
			printf("Mode= %d\n", smode);
			width2 = width * smode;
			height2 = height * smode;
			if (reduce > 0)
			{
				width2 = (width + smode - 1) / smode;
				height2 = (height + smode - 1) / smode;
			} else {
				width2 = width * smode;
				height2 = height * smode;
			} 
			printf("Width= %d\n", width2);
			printf("Height= %d\n", height2);
			IMTpixel** p_im;
			p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			IMTpixel** d_im;
			d_im = (IMTpixel**)malloc(height2 * sizeof(IMTpixel*));
			for (y = 0; y < height2; y++) {d_im[y] = (IMTpixel*)malloc(width2 * sizeof(IMTpixel));}

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			if (reduce)
			{
				printf("Scale= Reduce.\n");
				IMTFilterSReduce(p_im, d_im, height, width, smode);
			} else {
				printf("Scale= Up.\n");
				IMTFilterSHRIS(p_im, d_im, height, width, smode);
			} 

			for (y = 0; y < height; y++){free(p_im[y]);}
			free(p_im);
			dst_dib = FreeImage_Allocate(width2, height2, 24);
			ImthresholdSetData(dst_dib, d_im);
			for (y = 0; y < height2; y++){free(d_im[y]);}
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

