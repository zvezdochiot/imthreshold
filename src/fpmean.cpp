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

// PMean filter image.
// mode 0-similar: D = sum(I/abs(I-J))/sum(1/abs(I-J))
// mode 1-wbselect:  D = (sum(I>mean) * nw / n + sum(I<mean) * nb / n) * 2 / n
// mode 2-radial:  D = sum(I/dist2(I,J))/sum(1/dist2(I,J))
// mode 3-simple:  D = sum(I)/n
// Default mode: p.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterPMeanTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("PMean filter image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterPMeanUsage()
{
	printf("Usage : imthreshold-fpmean [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -r N.N  radius (double, optional, default = 3.0)\n");
	printf("          -m str  mode {similar, nlm, bilateral, wbselect, radial, simple} (str, optional, default = similar)\n");
	printf("          -n      neared (bool, optional, default = false)\n");
	printf("          -h      this help\n");
}

int main(int argc, char *argv[])
{
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB

	int opt;
	double radius = 3.0;
	int fmode = 0;
	bool fhelp = false;
	bool fneared = false;
	int iradius;
	while ((opt = getopt(argc, argv, ":r:m:nh")) != -1)
	{
		switch(opt)
		{
			case 'r':
				radius = atof(optarg);
				break;
			case 'm':
				if (strcmp(optarg, "wbselect") == 0) {fmode = 1;}
				if (strcmp(optarg, "radial") == 0) {fmode = 2;}
				if (strcmp(optarg, "simple") == 0) {fmode = 3;}
				if (strcmp(optarg, "nlm") == 0) {fmode = 4;}
				if (strcmp(optarg, "bilateral") == 0) {fmode = 5;}
				break;
			case 'n':
				fneared = true;
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
	
	ImthresholdFilterPMeanTitle();
	
	if(optind + 2 > argc || fhelp)
	{
		ImthresholdFilterPMeanUsage();
		return 0;
	}
	const char *src_filename = argv[optind];
	const char *output_filename = argv[optind + 1];
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	printf("Input= %s\n", src_filename);
	FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
	if (radius < 0) {iradius = (int)(-radius);} else {iradius = (int)radius;}
	printf("Radius= %f (%d)\n", radius, iradius);
	if (fmode == 0) {printf("Mode= similar\n");}
	if (fmode == 1) {printf("Mode= wbselect\n");}
	if (fmode == 2) {printf("Mode= radial\n");}
	if (fmode == 3) {printf("Mode= simple\n");}
	if (fmode == 4) {printf("Mode= nlm\n");}
	if (fmode == 5) {printf("Mode= bilateral\n");}
	if (fneared > 0) {printf("Neared= true\n");}
	if (dib)
	{
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			FIBITMAP* dst_dib;
			if (radius == 0)
			{
				dst_dib = ImthresholdFilterNone(dib);
			} else {
				unsigned width = FreeImage_GetWidth(dib);
				unsigned height = FreeImage_GetHeight(dib);
				unsigned y;
				
				IMTpixel** p_im;
				p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
				IMTpixel** d_im;
				d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
				for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

				ImthresholdGetData(dib, p_im);
				FreeImage_Unload(dib);
				IMTFilterPMean(p_im, d_im, height, width, radius, fmode, fneared);
				for (y = 0; y < height; y++){free(p_im[y]);}
				free(p_im);
				dst_dib = FreeImage_Allocate(width, height, 24);
				ImthresholdSetData(dst_dib, d_im);
				for (y = 0; y < height; y++){free(d_im[y]);}
				free(d_im);
			}
			
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
