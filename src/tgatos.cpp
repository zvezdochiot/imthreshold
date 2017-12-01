/*
*
* Copyright (C) 2005 John Ashley Burgoyne and Ichiro Fujinaga
*               2007 Uma Kompella and Christoph Dalitz
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
* 
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
 Thresholds an image according to Gatos et al.'s method. See:

 Gatos, Basilios, Ioannis Pratikakis, and Stavros
 J. Perantonis. 2004. An adaptive binarization technique for low
 quality historical documents. *Lecture Notes in Computer
 Science* 3163: 102-113.

 *background*
 Estimated background of the image.

 *binarization*
 A preliminary binarization of the image.

 Use the default settings for the other parameters unless you know
 what you are doing.

 Uses Gatos Background as an estimated background of the image.

 Uses Niblack Thresholding as a preliminary binarization

 Own parameters: double "q", default=0.6
                 double "p1", default=0.5
                 double "p2", default=0.8
*/

// This algorithm was taken from the gamera.sf.net sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTGatosTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Thresholds an image according to Gatos et al.'s method.\n");
	printf("This algorithm was taken from the gamera.sf.net sourcecodes and adopted for the FreeImage library.\n\n");
}

void ImthresholdFilterTGatosUsage()
{
	printf("Usage : imthreshold-tgatos [options] <input_file> <output_file>(BW) [bg_file] [niblack_file](BW)\n\n");
	printf("options:\n");
	printf("          -m str  mode {niblack, sauvola, chistian, bimod, dalg, default = niblack)\n");
	printf("          -r N    radius (int, optional, default = 7)\n");
	printf("          -s N.N  sensitivity (double, optional, default = -0.2)\n");
	printf("          -f N    dynamic range (int, optional, default = 128)\n");
	printf("          -l N    lower bound (int, optional, default = 20)\n");
	printf("          -u N    upper bound (int, optional, default = 150)\n");
	printf("          -d N.N  delta (double, optional, default = 0.0)\n");
	printf("          -q N.N  q (double, optional, default = 0.6)\n");
	printf("          -1 N.N  p1 (double, optional, default = 0.5)\n");
	printf("          -2 N.N  p2 (double, optional, default = 0.8)\n");
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
	int fmode = 0;
	int radius = 7;
	double sensitivity = -0.2;
	int dynamic_range = 128;
	int lower_bound = 20;
	int upper_bound = 150;
	double delta = 0.0;
	double q = 0.6;
	double p1 = 0.5;
	double p2 = 0.8;
	bool fhelp = false;
	int threshold = 0;
	while ((opt = getopt(argc, argv, ":m:r:s:f:l:u:d:q:1:2:h")) != -1)
	{
		switch(opt)
		{
			case 'm':
				if (strcmp(optarg, "niblack") == 0) {fmode = 0;}
				if (strcmp(optarg, "sauvola") == 0) {fmode = 1;}
				if (strcmp(optarg, "chistian") == 0) {fmode = 2;}
				if (strcmp(optarg, "bimod") == 0) {fmode = 3;}
				if (strcmp(optarg, "dalg") == 0) {fmode = 4;}
				break;
			case 'r':
				radius = atof(optarg);
				break;
			case 's':
				sensitivity = atof(optarg);
				break;
			case 'f':
				dynamic_range = atof(optarg);
				break;
			case 'l':
				lower_bound = atof(optarg);
				break;
			case 'u':
				upper_bound = atof(optarg);
				break;
			case 'd':
				delta = atof(optarg);
				break;
			case 'q':
				q = atof(optarg);
				break;
			case '1':
				p1 = atof(optarg);
				break;
			case '2':
				p2 = atof(optarg);
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
	
	ImthresholdFilterTGatosTitle();
	
	if(optind + 2 > argc || fhelp || radius <= 0)
	{
		ImthresholdFilterTGatosUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];
	const char *bg_filename;
	if(optind + 2 < argc)
	{
		bg_filename = argv[optind + 2];
	}
	const char *bin_filename;
	if(optind + 3 < argc)
	{
		bin_filename = argv[optind + 3];
	}
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	printf("Input= %s\n", src_filename);
	if (fmode == 0) {printf("Mode= Niblack\n");}
	if (fmode == 1) {printf("Mode= Sauvola\n");}
	if (fmode == 2) {printf("Mode= Chistian\n");}
	if (fmode == 3) {printf("Mode= BiMod\n");}
	if (fmode == 4) {printf("Mode= D-alg\n");}
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (dib)
	{		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			FIBITMAP* bin_dib;
			FIBITMAP* bg_dib;
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
			IMTpixel** bg_im;
			bg_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
			for (y = 0; y < height; y++) {bg_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
			BYTE** g_im;
			g_im = (BYTE**)malloc(height * sizeof(BYTE*));
			for (y = 0; y < height; y++) {g_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

			switch(fmode)
			{
				case 1:
					printf("Dynamic= %d\n", dynamic_range);
				case 0:
				case 2:
					printf("Sensitivity= %f\n", sensitivity);
					printf("Lower= %d\n", lower_bound);
					printf("Upper= %d\n", upper_bound);
				case 4:
					printf("Radius= %d\n", radius);
				case 3:
					printf("Delta= %f\n", delta);
			}

			ImthresholdGetData(dib, p_im);
			FreeImage_Unload(dib);
			switch(fmode)
			{
				case 0:	
					threshold = IMTFilterTNiblack(p_im, d_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
					break;
				case 1:	
					threshold = IMTFilterTSauvola(p_im, d_im, height, width, radius, sensitivity, dynamic_range, lower_bound, upper_bound, delta);
					break;
				case 2:	
					threshold = IMTFilterTChistian(p_im, d_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
					break;
				case 3:	
					threshold = IMTFilterTBiMod(p_im, d_im, height, width, delta);
					break;
				case 4:	
					threshold = IMTFilterTDalg(p_im, d_im, height, width, radius, delta);
					break;
			}
			threshold = IMTFilterGatosBG(p_im, d_im, bg_im, height, width, radius);
			printf("q= %f\n", q);
			printf("p1= %f\n", p1);
			printf("p2= %f\n", p2);
			threshold = IMTFilterTGatos(p_im, d_im, bg_im, g_im, height, width, q, p1, p2);
			printf("Threshold= %d\n", threshold / 3);
			for (y = 0; y < height; y++){free(p_im[y]);}
			free(p_im);
			bin_dib = FreeImage_Allocate(width, height, 1);
			ImthresholdSetDataBW(bin_dib, d_im);
			for (y = 0; y < height; y++){free(d_im[y]);}
			free(d_im);
			bg_dib = FreeImage_Allocate(width, height, 24);
			ImthresholdSetData(bg_dib, bg_im);
			for (y = 0; y < height; y++){free(bg_im[y]);}
			free(bg_im);
			dst_dib = FreeImage_Allocate(width, height, 1);				
			ImthresholdSetDataBW(dst_dib, g_im);
			for (y = 0; y < height; y++){free(g_im[y]);}
			free(g_im);
			
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
			if (bg_dib)
			{
				if(optind + 2 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(bg_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, bg_dib, bg_filename, 0);
						printf("Output= %s\n", bg_filename);
					}
				}
				FreeImage_Unload(bg_dib);					
				printf("\n");
			}
			if (bin_dib)
			{
				if(optind + 3 < argc)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(bin_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, bin_dib, bin_filename, 0);
						printf("Output= %s\n", bin_filename);
					}
				}
				FreeImage_Unload(bin_dib);					
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
