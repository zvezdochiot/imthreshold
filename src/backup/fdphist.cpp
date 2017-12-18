// ==========================================================
// Despeckle histogram filter (for BW only) based on the FreeImage library
//
// This code is taken from the CxImage Library at http://www.xdp.it/cximage.htm
// and adapted for the FreeImage Library
//

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterDphistTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Despeckle histogram filter (for BW only) based on the FreeImage library.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterDphistUsage()
{
	printf("Usage : imthreshold-fdphist [options] <input_file>(BW) <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -a N    aperture size (int, optional, default = 1, range = [1, 2, 3])\n");
	printf("          -i      invert (bool, optional, default = false)\n");
	printf("          -h      this help\n");
}

// ----------------------------------------------------------

int main(int argc, char *argv[])
{
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB
	
	int opt;
	unsigned Ksize = 1;
	double kdp = 0.0;
	bool finv = false;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":a:ih")) != -1)
	{
		switch(opt)
		{
			case 'a':
				Ksize = atof(optarg);
				break;
			case 'i':
				finv = true;
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

	ImthresholdFilterDphistTitle();
	
	if(optind + 2 > argc || fhelp || Ksize < 1)
	{
		ImthresholdFilterDphistUsage();;
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
			FIBITMAP *despeckled;
			if (FreeImage_GetBPP(dib) == 1)
			{
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned y;

				BYTE** p_im;
				p_im = (BYTE**)malloc(height * sizeof(BYTE*));
				for (y = 0; y < height; y++) {p_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}
				
				ImthresholdGetDataBW(dib, p_im);
				if (finv) {IMTFilterInvertBW(p_im, height, width);}
				kdp = IMTFilterDphist(p_im, height, width, Ksize);
				printf("Despeckle= %f\n", kdp);
				despeckled = FreeImage_Allocate(width, height, 1);
				ImthresholdSetDataBW(despeckled, p_im);
				for (y = 0; y < height; y++){free(p_im[y]);}
				free(p_im);
			} else {
				despeckled = ImthresholdFilterNone(dib);
				printf("%s\n", "Unsupported color mode.");
			}
			
			if (despeckled)
			{
				FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
				if(out_fif != FIF_UNKNOWN)
				{
					FreeImage_Save(out_fif, despeckled, output_filename, 0);
					printf("Output= %s\n\n", output_filename);
				}
				FreeImage_Unload(despeckled);
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
