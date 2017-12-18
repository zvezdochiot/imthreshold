// AForge Image Processing Library
// AForge.NET framework
//
// Copyright ©
//   Mladen Prajdic  (spirit1_fe@yahoo.com),
//   Andrew Kirillov (andrew.kirillov@gmail.com)
// 2005-2008
//
// This algorithm was taken from the AForge.NET sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

// Flat field correction filter.

// The goal of flat-field correction is to remove artifacts from 2-D images that
// are caused by variations in the pixel-to-pixel sensitivity of the detector and/or by distortions
// in the optical path. The filter requires two images for the input - source image, which represents
// acquisition of some objects (using microscope, for example), and background image, which is taken
// without any objects presented. The source image is corrected using the formula: src = bgMean * src / bg,
// where src - source image's pixel value, bg - background image's pixel value, bgMean - mean
// value of background image.
// 
// If background image is not provided, then it will be automatically generated on each filter run
// from source image. The automatically generated background image is produced running Gaussian Blur on the
// original image with (kernel size is set to 5). Before blurring the original image
// is resized to 1/3 of its original size and then the result of blurring is resized back to the original size.

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

/////////////////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterIllumCorrTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Flat field correction filter.\n");
	printf("This algorithm was taken from the AForge.NET sourcecodes and adopted for the FreeImage library.\n\n");
}

void ImthresholdFilterIllumCorrUsage()
{
	printf("Usage : imthreshold-fillumc [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -r N.N  radius (double, optional, default = 5.0)\n");
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
	unsigned threshold = 0;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":r:h")) != -1)
	{
		switch(opt)
		{
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
	
	ImthresholdFilterIllumCorrTitle();
	
	if(optind + 2 > argc || fhelp || radius == 0)
	{
		ImthresholdFilterIllumCorrUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];		

	// initialize your own FreeImage error handler
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
			IMTpixel domin;

			IMTpixel** p_im;
			p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			IMTpixel** b_im;
			b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			IMTpixel** d_im;
			d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

			printf("Radius= %f\n", radius);
			printf("Threshold= %d\n", threshold);

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			if (radius == 0)
			{
				IMTFilterCopy(p_im, b_im, height, width);
			} else {

				IMTFilterGaussBlur(p_im, b_im, height, width, radius);
			}
			domin = IMTFilterIllumCorr(p_im, b_im, d_im, height, width);
			printf("Dominante= %d,%d,%d\n", domin.c[0], domin.c[1], domin.c[2]);
			for (y = 0; y < height; y++){free(b_im[y]);}
			free(b_im);
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

/////////////////////////////////////////////////////////////////////////////////////////////
