//	Zlib license
//
// White Rohrer thresholding image.
//
//	Copyright (C) 2017:
//	zvezdochiot	<zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

void ImthresholdFilterTWhiteRohrerTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("White Rohrer thresholding image.\n");
	printf("This algorithm was taken from the gamera.sf.net sourcecodes and adopted for the FreeImage library.\n\n");
}

void ImthresholdFilterTWhiteRohrerUsage()
{
	printf("Usage : imthreshold-twhiterohrer [options] <input_file> <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -x N    x lookahead (int, optional, default = 8)\n");
	printf("          -y N    y lookahead (int, optional, default = 1)\n");
	printf("          -m N    bias mode (int, optional, default = 0)\n");
	printf("          -b N    bias factor (int, optional, default = 50)\n");
	printf("          -f N    f factor (int, optional, default = 50)\n");
	printf("          -g N    g factor (int, optional, default = 50)\n");
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
	int x_lookahead = 8;
	int y_lookahead = 1;
	int bias_mode = 0;
	int bias_factor = 50;
	int f_factor = 50;
	int g_factor = 50;
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":x:y:m:b:f:g:h")) != -1)
	{
		switch(opt)
		{
			case 'x':
				x_lookahead = atof(optarg);
				break;
			case 'y':
				y_lookahead = atof(optarg);
				break;
			case 'm':
				bias_mode = atof(optarg);
				break;
			case 'b':
				bias_factor = atof(optarg);
				break;
			case 'f':
				f_factor = atof(optarg);
				break;
			case 'g':
				g_factor = atof(optarg);
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
	
	ImthresholdFilterTWhiteRohrerTitle();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterTWhiteRohrerUsage();
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

			printf("Xlookahead= %d\n", x_lookahead);
			printf("Ylookahead= %d\n", y_lookahead);
			printf("biasMode= %d\n", bias_mode);
			printf("biasFactor= %d\n", bias_factor);
			printf("Ffactor= %d\n", f_factor);
			printf("Gfactor= %d\n", g_factor);

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			threshold = IMTFilterTWhiteRohrer(p_im, d_im, height, width, x_lookahead, y_lookahead, bias_mode, bias_factor, f_factor, g_factor);
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
