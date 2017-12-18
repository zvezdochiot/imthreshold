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

// Level Sigma filter image.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterLevelSigmaTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Level Sigma filter image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterLevelSigmaUsage()
{
	printf("Usage : imthreshold-flevelsigma [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -a N.N  average (double, optional, default = 0.5)\n");
	printf("          -p N.N  part sigma (double, optional, default = 0)\n");
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
	double thres = 0.5;
	double fpart = 0.0;
	bool fhelp = false;
	double ks = 0;
	while ((opt = getopt(argc, argv, ":a:p:h")) != -1)
	{
		switch(opt)
		{
			case 'a':
				thres = atof(optarg);
				break;
			case 'p':
				fpart = atof(optarg);
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
	
	ImthresholdFilterLevelSigmaTitle();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterLevelSigmaUsage();
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

			printf("Average= %f\n", thres);
			printf("Part= %f\n", fpart);

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			ks = IMTFilterLevelSigma(p_im, d_im, height, width, thres, fpart);
			printf("Multy= %f\n", ks);
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

