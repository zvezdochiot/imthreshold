/*
 DjVuL thresholds an image (Multi-scale binarization).

 *binarization*
 A preliminary binarization of the image.

 *foreground*
 Estimated foreground of the image.

 *background*
 Estimated background of the image.

 Own parameters:
				"b"     base block size (int, optional, default = 3)
				"f"     foreground divide (int, optional, default = 2)
				"l"     level (int, optional, default = 10)
				"r"     reverse FG/BG (bool, optional, default = no[0], recommended = yes[1])
*/

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/) sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include "imthreshold.h"

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
	printf("          -b N    base block size (int, optional, default = 3)\n");
	printf("          -f N    foreground divide (int, optional, default = 2)\n");
	printf("          -l N    level (int, optional, default = 10)\n");
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
	int bgs = 3;
	int fgs = 2;
	int level = 10;
	int wbmode = 0;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":a:b:f:l:w:h")) != -1)
	{
		switch(opt)
		{
			case 'a':
				anisotropic = atof(optarg);
				break;
			case 'b':
				bgs = atof(optarg);
				break;
			case 'f':
				fgs = atof(optarg);
				break;
			case 'l':
				level = atof(optarg);
				break;
			case 'w':
				wbmode = atof(optarg);
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
	
	ImthresholdIMTFilterDjVuLTitle();
	
	if(optind + 2 > argc || fhelp > 0 || bgs < 1 || fgs < 1 || level < 1)
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
	
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 1 || FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib;
				FIBITMAP* fg_dib;
				FIBITMAP* bg_dib;
				FIBITMAP* fgm_dib;
				FIBITMAP* bgm_dib;
				RGBQUAD *pal;
				if (FreeImage_GetBPP(dib) == 1)
				{
					dst_dib = ImthresholdFilterNone(dib);
				}
				else
				{
					unsigned width = FreeImage_GetWidth(dib);
					unsigned height = FreeImage_GetHeight(dib);
					unsigned widthbg = (width + bgs - 1) / bgs;
					unsigned heightbg = (height + bgs - 1) / bgs;
					unsigned widthfg = (widthbg + fgs - 1) / fgs;
					unsigned heightfg = (heightbg + fgs - 1) / fgs;
					unsigned y, x, d, y0, x0, y1, x1, i, j;
					dst_dib = FreeImage_Allocate(width, height, 1);	
					unsigned dst_pitch = FreeImage_GetPitch(dst_dib);
					BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib);
					pal = FreeImage_GetPalette(dst_dib);
					pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
					pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
					fg_dib = FreeImage_Allocate(widthfg, heightfg, 24);	
					unsigned fg_pitch = FreeImage_GetPitch(fg_dib);
					BYTE* fg_bits = (BYTE*)FreeImage_GetBits(fg_dib);
					bg_dib = FreeImage_Allocate(widthbg, heightbg, 24);	
					unsigned bg_pitch = FreeImage_GetPitch(bg_dib);
					BYTE* bg_bits = (BYTE*)FreeImage_GetBits(bg_dib);
					fgm_dib = FreeImage_Allocate(widthfg, heightfg, 1);	
					unsigned fgm_pitch = FreeImage_GetPitch(fgm_dib);
					BYTE* fgm_bits = (BYTE*)FreeImage_GetBits(fgm_dib);
					pal = FreeImage_GetPalette(fgm_dib);
					pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
					pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
					bgm_dib = FreeImage_Allocate(widthbg, heightbg, 1);	
					unsigned bgm_pitch = FreeImage_GetPitch(bgm_dib);
					BYTE* bgm_bits = (BYTE*)FreeImage_GetBits(bgm_dib);
					pal = FreeImage_GetPalette(bgm_dib);
					pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
					pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
					BYTE* lined;
					BYTE val;
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
					
					ImthresholdGetData(dib, p_im);
					
					IMTFilterDjVuL(p_im, m_im, fg_im, bg_im, height, width, bgs, fgs, level, wbmode, anisotropic);
					
					for (y = 0; y < height; y++)
					{
						lined = dst_bits + y * dst_pitch;
						for (x = 0; x < width; x++)
						{
							val = (BYTE)m_im[y][x];
							ImthresholdSetPixel(lined, x, &val);
						}
					}
					for (y = 0; y < heightfg; y++)
					{
						lined = fgm_bits + y * fgm_pitch;
						y0 = y * bgs * fgs;
						y1 = y0 + bgs * fgs;
						if (y1 > height) {y1 = height;}
						for (x = 0; x < widthfg; x++)
						{
							x0 = x * bgs * fgs;
							x1 = x0 + bgs * fgs;
							if (x1 > width) {x1 = width;}
							val = 0;
							for (i = y0; i < y1; i++)
							{
								for (j = x0; j < x1; j++)
								{
									if (m_im[i][j] == 0) {val = 255;}
								}
							}
							/*
							if (val > 0)
							{
								fg_im[y][x] = IMTmeanIc(p_im, y0, x0, y1, x1);
							}
							*/
							ImthresholdSetPixel(lined, x, &val);
						}
					}
					for (y = 0; y < heightbg; y++)
					{
						lined = bgm_bits + y * bgm_pitch;
						y0 = y * bgs;
						y1 = y0 + bgs;
						if (y1 > height) {y1 = height;}
						for (x = 0; x < widthbg; x++)
						{
							x0 = x * bgs;
							x1 = x0 + bgs;
							if (x1 > width) {x1 = width;}
							val = 255;
							for (i = y0; i < y1; i++)
							{
								for (j = x0; j < x1; j++)
								{
									if (m_im[i][j] == 0) {val = 0;}
								}
							}
							/*
							if (val > 0)
							{
								bg_im[y][x] = IMTmeanIc(p_im, y0, x0, y1, x1);
							}
							*/
							ImthresholdSetPixel(lined, x, &val);
						}
					}
					for (y = 0; y < heightfg; y++)
					{
						lined = fg_bits + y * fg_pitch;
						for (x = 0; x < widthfg; x++)
						{
							for (d = 0; d < 3; d++)
							{
								val = (BYTE)fg_im[y][x].c[d];
								lined[x * 3 + d] = val;
							}
						}
					}
					for (y = 0; y < heightbg; y++)
					{
						lined = bg_bits + y * bg_pitch;
						for (x = 0; x < widthbg; x++)
						{
							for (d = 0; d < 3; d++)
							{
								val = (BYTE)bg_im[y][x].c[d];
								lined[x * 3 + d] = val;
							}
						}
					}

					for (y = 0; y < height; y++){free(p_im[y]);}
					free(p_im);
					for (y = 0; y < height; y++){free(m_im[y]);}
					free(m_im);
					for (y = 0; y < heightfg; y++){free(fg_im[y]);}
					free(fg_im);
					for (y = 0; y < heightbg; y++){free(bg_im[y]);}
					free(bg_im);
				}
				if (dst_dib)
				{					
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, dst_dib, output_filename, 0);
					}
					FreeImage_Unload(dst_dib);					
					printf("Output= %s\n", output_filename);
				}
				if (fg_dib)
				{
					if(optind + 2 < argc)
					{
						FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(fg_filename);
						if(out_fif != FIF_UNKNOWN)
						{
							FreeImage_Save(out_fif, fg_dib, fg_filename, 0);
						}
						printf("Output= %s\n", fg_filename);
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
						}
						printf("Output= %s\n", bg_filename);
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
						}
						printf("Output= %s\n", fg_maskname);
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
						}
						printf("Output= %s\n", bg_maskname);
					}
					FreeImage_Unload(bgm_dib);					
				}
				printf("\n");
			} else {
				printf("%s\n", "Unsupported color mode.");
			}
		} else {
			printf("%s\n", "Unsupported color mode.");
		}
		FreeImage_Unload(dib);
	}	 
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
	
	return 0;
}
