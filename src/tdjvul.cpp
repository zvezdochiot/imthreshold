//	Zlib license
//
// DjVuL thresholds an image (Multi-scale binarization).
//
//	Copyright (C) 2017:
//	zvezdochiot	<zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdIMTFilterDjVuLTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("DjVuL thresholds an image (Multi-scale binarization).\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdIMTFilterDjVuLUsage()
{
	printf("Usage : imthreshold-tdjvul [options] <input_file> <output_file>(BW) [fg_file] [bg_file] [fg_mask] [bg_mask]\n\n");
	printf("options:\n");
	printf("          -a N.N  anisotropic (double, optional, default = 0.0)\n");
	printf("          -c N    clean fg, bg blur radius (int, optional, default = 0)\n");
	printf("          -d N    despeckle aperture size (int, optional, default = 0)\n");
	printf("          -b N    base block size (int, optional, default = 3)\n");
	printf("          -f N    foreground divide (int, optional, default = 2)\n");
	printf("          -i      invert station (bool, optional, default = false)\n");
	printf("          -l N    level (int, optional, default = 10)\n");
	printf("          -o N.N  overlay (double, optional, default = 0.5)\n");
	printf("          -p N    posterize fg (int, optional, default = 0)\n");
	printf("          -w N    w/b mode (int, optional, default = 0 [auto], >0-white, <0-black)\n");
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
	double anisotropic = 0.0;
	double doverlay = 0.5;
	int bgs = 3;
	int fgs = 2;
	int level = 10;
	int wbmode = 0;
	int fclean = 0;
	int fdespeckle = 0;
	unsigned fposter = 0;
	bool finvs = false;
	bool fhelp = false;
	while ((opt = getopt(argc, argv, ":a:c:d:b:f:il:p:o:w:h")) != -1)
	{
		switch(opt)
		{
			case 'b':
				bgs = atof(optarg);
				break;
			case 'f':
				fgs = atof(optarg);
				break;
			case 'l':
				level = atof(optarg);
				break;
			case 'i':
				finvs = true;
				break;
			case 'w':
				wbmode = atof(optarg);
				break;
			case 'a':
				anisotropic = atof(optarg);
				break;
			case 'o':
				doverlay = atof(optarg);
				break;
			case 'c':
				fclean = atof(optarg);
				break;
			case 'd':
				fdespeckle = atof(optarg);
				break;
			case 'p':
				fposter = atof(optarg);
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
	
	ImthresholdIMTFilterDjVuLTitle();
	
	if(optind + 2 > argc || fhelp || bgs < 1 || fgs < 1 || level < 1)
	{
		ImthresholdIMTFilterDjVuLUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];
	const char *fg_filename;
	if(optind + 2 < argc)
	{
		fg_filename = argv[optind + 2];
	}
	const char *bg_filename;
	if(optind + 3 < argc)
	{
		bg_filename = argv[optind + 3];
	}
	const char *fg_maskname;
	if(optind + 4 < argc)
	{
		fg_maskname = argv[optind + 4];
	}
	const char *bg_maskname;
	if(optind + 5 < argc)
	{
		bg_maskname = argv[optind + 5];
	}
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	if (fclean < 0) {fclean = -fclean;}
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			FIBITMAP* dst_dib;
			FIBITMAP* fg_dib;
			FIBITMAP* bg_dib;
			FIBITMAP* fgm_dib;
			FIBITMAP* bgm_dib;
			if (FreeImage_GetBPP(dib) == 1)
			{
				dst_dib = ImthresholdFilterNone(dib);
			}
			else
			{
				printf("Level= %d\n", level);
				printf("Anisotropic= %f\n", anisotropic);
				printf("Overlay= %f\n", doverlay);
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned widthbg = (width + bgs - 1) / bgs;
				unsigned heightbg = (height + bgs - 1) / bgs;
				unsigned widthfg = (widthbg + fgs - 1) / fgs;
				unsigned heightfg = (heightbg + fgs - 1) / fgs;
				unsigned y;
				
				IMTpixel** p_im;
				p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
				BYTE** m_im;
				m_im = (BYTE**)malloc(height * sizeof(BYTE*));
				for (y = 0; y < height; y++) {m_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}
				IMTpixel** fg_im;
				fg_im = (IMTpixel**)malloc(heightfg * sizeof(IMTpixel*));
				for (y = 0; y < heightfg; y++) {fg_im[y] = (IMTpixel*)malloc(widthfg * sizeof(IMTpixel));}
				IMTpixel** bg_im;
				bg_im = (IMTpixel**)malloc(heightbg * sizeof(IMTpixel*));
				for (y = 0; y < heightbg; y++) {bg_im[y] = (IMTpixel*)malloc(widthbg * sizeof(IMTpixel));}
				BYTE** fgm_im;
				fgm_im = (BYTE**)malloc(heightfg * sizeof(BYTE*));
				for (y = 0; y < heightfg; y++) {fgm_im[y] = (BYTE*)malloc(widthfg * sizeof(BYTE));}
				BYTE** bgm_im;
				bgm_im = (BYTE**)malloc(heightbg * sizeof(BYTE*));
				for (y = 0; y < heightbg; y++) {bgm_im[y] = (BYTE*)malloc(widthbg * sizeof(BYTE));}
				
				ImthresholdGetData(dib, p_im);
				FreeImage_Unload(dib);
				if (fposter != 0)
				{
					printf("Posterize= %d\n", fposter);
				}
				wbmode = IMTFilterTDjVuL(p_im, m_im, fg_im, bg_im, height, width, bgs, fgs, level, wbmode, anisotropic, doverlay, fposter);
				if (wbmode > 0)
				{
					printf("Mode= white\n");
				} else {
					printf("Mode= black\n");
				}
				for (y = 0; y < height; y++){free(p_im[y]);}
				free(p_im);
				
				if (fdespeckle > 0)
				{
					printf("Despeckle= %d\n", fdespeckle);
					IMTFilterDespeck2(m_im, height, width, fdespeckle);
				}
				IMTReduceBW(m_im, fgm_im, height, width, heightfg, widthfg, bgs * fgs, 0, 255);
				IMTReduceBW(m_im, bgm_im, height, width, heightbg, widthbg, bgs, 255, 0);
				dst_dib = FreeImage_Allocate(width, height, 1);
				if (finvs)
				{
					printf("InvertStation= true\n");
					IMTFilterInvertBW (m_im, height, width);
				}
				ImthresholdSetDataBW(dst_dib, m_im);
				for (y = 0; y < height; y++){free(m_im[y]);}
				free(m_im);
				if (fclean > 0)
				{
					printf("Blur= %d\n", fclean);
					if (fposter == 0)
					{
						IMTBlurMask(fg_im, fgm_im, heightfg, widthfg, fclean);
					}
					IMTBlurMask(bg_im, bgm_im, heightbg, widthbg, fclean);
				}
				if (finvs)
				{
					fg_dib = FreeImage_Allocate(widthbg, heightbg, 24);
					ImthresholdSetData(fg_dib, bg_im);
					bg_dib = FreeImage_Allocate(widthfg, heightfg, 24);	
					ImthresholdSetData(bg_dib, fg_im);
				} else {	
					fg_dib = FreeImage_Allocate(widthfg, heightfg, 24);
					ImthresholdSetData(fg_dib, fg_im);
					bg_dib = FreeImage_Allocate(widthbg, heightbg, 24);	
					ImthresholdSetData(bg_dib, bg_im);
				}
				for (y = 0; y < heightfg; y++){free(fg_im[y]);}
				free(fg_im);
				for (y = 0; y < heightbg; y++){free(bg_im[y]);}
				free(bg_im);
				if (finvs)
				{
					fgm_dib = FreeImage_Allocate(widthbg, heightbg, 1);
					ImthresholdSetDataBW(fgm_dib, bgm_im);
					bgm_dib = FreeImage_Allocate(widthfg, heightfg, 1);
					ImthresholdSetDataBW(bgm_dib, fgm_im);
				} else {
					fgm_dib = FreeImage_Allocate(widthfg, heightfg, 1);
					ImthresholdSetDataBW(fgm_dib, fgm_im);
					bgm_dib = FreeImage_Allocate(widthbg, heightbg, 1);
					ImthresholdSetDataBW(bgm_dib, bgm_im);
				}
				for (y = 0; y < heightfg; y++){free(fgm_im[y]);}
				free(fgm_im);
				for (y = 0; y < heightbg; y++){free(bgm_im[y]);}
				free(bgm_im);

			}
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
			if (fg_dib)
			{
				if(optind + 2 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(fg_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, fg_dib, fg_filename, 0);
						printf("Output= %s\n", fg_filename);
					}
				}
				FreeImage_Unload(fg_dib);					
			}
			if (bg_dib)
			{
				if(optind + 3 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(bg_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, bg_dib, bg_filename, 0);
						printf("Output= %s\n", bg_filename);
					}
				}
				FreeImage_Unload(bg_dib);					
			}
			if (fgm_dib)
			{
				if(optind + 4 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(fg_maskname);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, fgm_dib, fg_maskname, 0);
						printf("Output= %s\n", fg_maskname);
					}
				}
				FreeImage_Unload(fgm_dib);					
			}
			if (bgm_dib)
			{
				if(optind + 5 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(bg_maskname);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, bgm_dib, bg_maskname, 0);
						printf("Output= %s\n", bg_maskname);
					}
				}
				FreeImage_Unload(bgm_dib);					
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
