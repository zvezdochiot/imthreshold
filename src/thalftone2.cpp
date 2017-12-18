//	Zlib license
//
// Halftone thresholding image.
//
//	Copyright (C) 2017:
//	zvezdochiot	<zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTHalftone2Title()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Halftone thresholding image.\n");
	printf("HalftoneARM9: https://github.com/BrenoRM/HalftoneARM9.\n\n");
}

void ImthresholdFilterTHalftone2Usage()
{
	printf("Usage : imthreshold-thalftone2 [options] <input_file> <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -n      norm (bool, optional, default = false)\n");
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
	bool fnorm = false;
	bool freduce = false;
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":nrh")) != -1)
	{
		switch(opt)
		{
			case 'n':
				fnorm = true;
				break;
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
	
	ImthresholdFilterTHalftone2Title();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterTHalftone2Usage();
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
			unsigned width2 = width * 2;
			unsigned height2 = height * 2;
			unsigned y;

			IMTpixel** p_im;
			p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			BYTE** d_im;
			d_im = (BYTE**)malloc(height2 * sizeof(BYTE*));
			for (y = 0; y < height2; y++) {d_im[y] = (BYTE*)malloc(width2 * sizeof(BYTE));}
			BYTE** r_im;
			r_im = (BYTE**)malloc(height * sizeof(BYTE*));
			for (y = 0; y < height; y++) {r_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			if (fnorm)
			{
				threshold = IMTFilterSNorm(p_im, height, width);
				printf("Norm= %d\n", threshold);
			}
			threshold = IMTFilterTHalftone2(p_im, d_im, height, width);
			printf("Threshold= %d\n", threshold);
			for (y = 0; y < height; y++){free(p_im[y]);}
			free(p_im);
			if (freduce)
			{
				threshold = IMTFilterSBWReduce2(d_im, r_im, height2, width2, height, width);
				printf("Reduce= %d\n", threshold);
				dst_dib = FreeImage_Allocate(width, height, 1);
				ImthresholdSetDataBW(dst_dib, r_im);
			} else {
				dst_dib = FreeImage_Allocate(width2, height2, 1);
				ImthresholdSetDataBW(dst_dib, d_im);
			}
			for (y = 0; y < height; y++){free(r_im[y]);}
			free(r_im);
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
