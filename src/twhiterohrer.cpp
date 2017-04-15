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

////////////////////////////////////////////////////////////////////////////////

/*
White Rohrer thresholding. This implementation uses code from
the XITE library. According to its license, it may be freely included
into Gamera (a GPL licensed software), provided the following
notice is included into the code:

 Permission to use, copy, modify and distribute this software and its
 documentation for any purpose and without fee is hereby granted, 
 provided that this copyright notice appear in all copies and that 
 both that copyright notice and this permission notice appear in supporting
 documentation and that the name of B-lab, Department of Informatics or
 University of Oslo not be used in advertising or publicity pertaining 
 to distribution of the software without specific, written prior permission.
 
  B-LAB DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL B-LAB
  BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
  OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  
  Important notice: this implementation only works with 8-bit greyscale
  images because the maximal value 255 for white is hard coded!!
*/

/*
 Creates a binary image using White and Rohrer's dynamic thresholding
 algorithm. It is the first of the two algorithms described in:

 J. M. White and G. D. Rohrer. 1983. Image thresholding for optical
 character recognition and other applications requiring character
 image extraction.  *IBM J. Res. Dev.* 27(4), pp. 400-411

 The algorithm uses a 'running' average instead of true average of
 the gray values in the neighborhood.  The lookahead parameter
 gives the number of lookahead pixels used in the biased running
 average that is used in deciding the threshold at each pixel
 location.

 *x_lookahead*
 the number of lookahead pixels in the horizontal direction for
 computing the running average. White and Rohrer suggest a value
 of 8 for a 240 dpi scanning resolution.

 *y_lookahead*
 number of lines used for further averaging from the horizontal
 averages.

 The other parameters are for calculating biased running average.
 Without bias the thresholding decision would be determined by
 noise fluctuations in uniform areas.

 This implementation uses code from XITE:

 http://www.ifi.uio.no/forskning/grupper/dsb/Software/Xite/

 Parameters:

 int "x lookahead", default=8
 int "y lookahead", default=1
 int "bias mode", default=0
 int "bias factor", default=100
 int "f factor",default=100
 int "g factor",default=100
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
/*
* OneBit white_rohrer_threshold(GreyScale src, 
*                          int x_lookahead,
*                          int y_lookahead, 
*                          int bias_mode,
*                          int bias_factor,
*                          int f_factor
*                          int g_factor);
*/

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
