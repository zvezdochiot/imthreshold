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

// Delta linear/cubic interpolant filter image.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <FreeImage.h>
#include "Utilities.h"
#include <unistd.h>

typedef struct mpixel
{
	BYTE c[3];
	WORD s;
}
IMTpixel;

////////////////////////////////////////////////////////////////////////////////

int ImthresholdGetData(FIBITMAP* dib, IMTpixel** p_im)
{
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	unsigned bpp = FreeImage_GetBPP(dib);
	unsigned btpp = bpp/8;
	BYTE* bits = (BYTE*)FreeImage_GetBits(dib);
	BYTE* lines;
	RGBQUAD *dibpal = FreeImage_GetPalette(dib);
	unsigned x, y, d;
	if (bpp == 24)
	{
		for (y = 0; y < height; y++)
		{
			lines = bits + y * pitch;
			for (x = 0; x < width; x++)
			{
				for (d = 0; d < 3; d++)
				{
					p_im[y][x].c[d] = lines[x * 3 + d];
				}
			}
		}
	} else {
		for ( y = 0; y < height; y++ )
		{
			lines = bits + y * pitch;
			for ( x = 0; x < width; x++ )
			{
				p_im[y][x].c[0] = dibpal[lines[x]].rgbBlue;
				p_im[y][x].c[1] = dibpal[lines[x]].rgbGreen;
				p_im[y][x].c[2] = dibpal[lines[x]].rgbRed;
			}
		}
	}
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			p_im[y][x].s = 0;
			for (d = 0; d < 3; d++)
			{
				p_im[y][x].s += (WORD)p_im[y][x].c[d];
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* ImthresholdFilterNone(FIBITMAP* src_dib)
{
	if (!src_dib) return false;
	FIBITMAP *tmp = FreeImage_Clone(src_dib);
	printf("Copy image.\n");
	return tmp;
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* ImthresholdFilterDeltaInterp(FIBITMAP* src_dib, double thres, int percent, int fcub)
{
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
	unsigned pitch = FreeImage_GetPitch(src_dib);
	
	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, 24);	
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib);
	BYTE* lined;
	
	unsigned d, n;
	int y, x, yf, xf, i, j, k;
	
	int im, imf;
	double imx, imxf, imfx, imd, imdm, nx;
	
	int filtn;
	double filtd;
	double filtk[5];
	double filtdm, filtt;
	double filtkm[25];
	
    IMTpixel** p_im;
    p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
    for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
	
	ImthresholdGetData(src_dib, p_im);
	
	if (fcub == 0)
	{
		filtn = 3;
		filtd = 3.0;
		double filtk[5] = {1.0,1.0,1.0};
	} else {
		filtn = 5;
		filtd = 16.0;
		double filtk[5] = {-1.0,5.0,8.0,5.0,-1.0};
	}
	
	int filtr = (filtn - 1) / 2;
	
	if (percent > 0)
	{
		printf("Threshold= %d%%\n", (int)(thres));
	} else {
		printf("Threshold= %f\n", thres);
	}
	if (fcub > 0)
	{
		printf("Interpolant= cubic\n");
	} else {
		printf("Interpolant= linear\n");
	}
	
	filtdm = 0.0;
	k = 0;
	for ( i = -filtr; i <= filtr; i++ )
	{
		for ( j = -filtr; j <= filtr; j++ )
		{
			filtt = filtk[i + filtr];
			filtt *= filtk[j + filtr];
			filtkm[k] = filtt;
			filtdm += filtt;
			k++;
		}
	}
	k = 0;
	for ( i = -filtr; i <= filtr; i++ )
	{
		for ( j = -filtr; j <= filtr; j++ )
		{
			filtkm[k] /= filtdm;
			k++;
		}
	}
	if (thres < 0) {thres = 0;}
	
	if (percent == 0)
	{
		imdm = 1.0;
	} else{
		imdm = 0;
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				im = p_im[y][x].s;
				imx = (double)im / 3.0;
				imfx = 0.0;
				k = 0;
				for (i = -filtr; i <= filtr; i++)
				{
					yf = y + i;
					if (yf < 0) {yf = -yf;}
					if (yf >= height) {yf = 2 * (height - 1) - yf;}
					for (j = -filtr; j <= filtr; j++)
					{
						xf = x + j;
						if (xf < 0) {xf = -xf;}
						if (xf >= width) {xf = 2 * (width - 1) - xf;}
						imf = p_im[yf][xf].s;
						imxf = (double)imf / 3.0;
						imfx += (imxf * filtkm[k]);
						k++;
					}
				}
				imd = (imfx - imx);
				if (imd < 0) {imd = -imd;}
				imdm += (imd * imd);
			}
		}
		if (imdm > 0)
		{
			imdm = sqrt((double)(width * height) / imdm) * 256.0;
		} else {
			imdm = 1.0;
		}
		printf("Power= %f\n", imdm);
	}
	n = 0;
	for (y = 0; y < height; y++)
	{
		lined = dst_bits + y * pitch;
		for ( x = 0; x < width; x++ )
		{
			im = p_im[y][x].s;
			imx = (double)im / 3.0;
			imfx = 0.0;
			k = 0;
			for (i = -filtr; i <= filtr; i++)
			{
				yf = y + i;
				if (yf < 0) {yf = -yf;}
				if (yf >= height) {yf = 2 * (height - 1) - yf;}
				for (j = -filtr; j <= filtr; j++)
				{
					xf = x + j;
					if (xf < 0) {xf = -xf;}
					if (xf >= width) {xf = 2 * (width - 1) - xf;}
					imf = p_im[yf][xf].s;
					imxf = (double)imf / 3.0;
					imfx += (imxf * filtkm[k]);
					k++;
				}
			}
			imd = (imfx - imx) * imdm;
			imfx += 1.0;
			imfx /= (imx + 1.0);
			if (imd < 0) {imd = -imd;}
			for (d = 0; d < 3; d++)
			{
				im = p_im[yf][xf].c[d];
				if (imd > thres)
				{
					lined[x * 3 + d] = im;
				} else {
					imx = imfx;
					imx *= im;
					n++;
					lined[x * 3 + d] = (BYTE)MIN(MAX((int)0, (int)(imx + 0.5)), (int)255);
				}
			}
		}
	}
	nx = double(n);
	nx /= double(width);
	nx /= double(height);
	nx /= 3.0; 
	printf("Modify= %d (%f)\n", n, nx);
	FreeImage_SetDotsPerMeterX(dst_dib, FreeImage_GetDotsPerMeterX(src_dib));
	FreeImage_SetDotsPerMeterY(dst_dib, FreeImage_GetDotsPerMeterY(src_dib));
	
    for (y = 0; y < height; y++) {free(p_im[y]);}
	free(p_im);
	return dst_dib;
 }

 ////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------
/**
FreeImage error handler
@param fif Format / Plugin responsible for the error 
@param message Error message
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** "); 
	printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	printf(message);
	printf(" ***\n");
}

// ----------------------------------------------------------

/** Generic image loader

  @param lpszPathName Pointer to the full file name
  @param flag Optional load flag constant
  @return Returns the loaded dib if successful, returns NULL otherwise
*/

FIBITMAP* ImthresholdGenericLoader(const char* lpszPathName, int flag)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(lpszPathName, 0);
	FIBITMAP* dib;
	if(fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, lpszPathName, flag);
		if (!dib)
		{
			printf("%s%s%s\n","File \"", lpszPathName, "\" not found.");
			return NULL;
		}
	}
	
	return dib;
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterDeltaInterpTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Delta linear/cubic interpolant filter image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
}

void ImthresholdFilterDeltaInterpUsage()
{
	printf("Usage : imthreshold-fdeltainterp [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -t N.N  threshold (double, optional, default = 10.0)\n");
	printf("          -p      percent (bool, optional)\n");
	printf("          -c      cubic (bool, optional)\n");
	printf("          -h      this help\n");
}

int main(int argc, char *argv[])
{
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB
	
	int opt;
	double thres = 10;
	int percent = 0;
	int fcub = 0;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":cpt:h")) != -1)
	{
		switch(opt)
		{
			case 'c':
				fcub = 1;
				break;
			case 'p':
				percent = 1;
				break;
			case 't':
				thres = atof(optarg);
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
	
	ImthresholdFilterDeltaInterpTitle();
	
	if(optind + 2 < argc)
	{
		thres = atof(argv[optind + 2]);
	}
	if(optind + 3 < argc)
	{
		percent = atof(argv[optind + 3]);
	}
	if(optind + 4 < argc)
	{
		fcub = atof(argv[optind + 4]);
	}
	if(optind + 2 > argc || fhelp > 0)
	{
		ImthresholdFilterDeltaInterpUsage();
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
			if (FreeImage_GetBPP(dib) == 1 || FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib;
				if (FreeImage_GetBPP(dib) == 1)
				{
					dst_dib = ImthresholdFilterNone(dib);
				}
				else
				{
					dst_dib = ImthresholdFilterDeltaInterp(dib, thres, percent, fcub);
				}
				
				if (dst_dib)
				{
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
					if(out_fif != FIF_UNKNOWN)
					{
						FreeImage_Save(out_fif, dst_dib, output_filename, 0);
					}
					
					FreeImage_Unload(dst_dib);
					printf("Output= %s\n\n", output_filename);
				}
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

