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

//
// Multiscale Retinex.
//
// Algorithm based on the original work of Jobson et al.
// "A multiscale Retinex for bridging the gap between color images
// and the human observations of scenes"
//
// @author Catalina Sbert <catalina.sbert@uib.es/>
// @author Ana Belén Petro <anabelen.petro@uib.es/>
// 2013 IPOL Image Processing On Line http://www.ipol.im/
//

//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <FreeImage.h>
#include "Utilities.h"
#include "MSR_original_lib.h"
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

FIBITMAP* ImthresholdFilterMSR(FIBITMAP* src_dib, double sf1, double sf2, double sf3)
{
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
	
	printf("Sigma1= %f\n", sf1);
	printf("Sigma2= %f\n", sf2);
	printf("Sigma3= %f\n", sf3);

	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, 24);	
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib);
	unsigned pitch = FreeImage_GetPitch(dst_dib);
	BYTE* lined;
	
	unsigned y, x, d, k;
	
	int im;
	int imsz = height * width;
	double imx, imxn, kim, kims;
	double imxs, imxns;
	double immin, immax, imdt, immed;
	
	int nscales = 3;
	double w, scale[3];
	scale[0] = sf1;
	scale[1] = sf2;
	scale[2] = sf3;
	w = 1.0 / (double)nscales;
	double* greyI = new double[imsz];
	double* greyO = new double[imsz];
	double greymin, greymax, greydt, greym;
	imxs = 0.0;
	immin = 1.0;
	immax = 0.0;
	k = 0;
	
    IMTpixel** p_im;
    p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
    for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
	
	ImthresholdGetData(src_dib, p_im);
	
	for ( y = 0; y < height; y++ )
	{
		for ( x = 0; x < width; x++ )
		{
			im = p_im[y][x].s;
			imx = (double)(im + 1) / 3.0 / 256.0;
			if ( imx < immin) {immin = imx;}
			if ( imx > immax) {immax = imx;}
			imxs += imx;
			greyI[k] = imx;
			k++;
		}
	}
	imdt = immax - immin;
	if ( imdt == 0.0)
	{
		immed = 0.5;
		imdt = 1.0;
	} else {
		immed = -immin / imdt;
		imdt = 1.0 / imdt;
	}
	imxs /= imsz;
	imxs = (imxs * 256.0 - 1.0) / 255.0;
	printf("I= %f\n", imxs);
	MSRetinex(greyO, greyI, scale, nscales, w, width, height);
	greymin = greyO[0];
	greymax = greyO[0];
	greym = 0;
	for ( k = 0; k < imsz; k++ )
	{
		imx=greyO[k];
		if ( imx < greymin) {greymin = imx;}
		if ( imx > greymax) {greymax = imx;}
		greym += imx;
	}
	greym /= imsz;
	greydt = greymax - greymin;
	if ( greydt == 0.0)
	{
		greym = 0.5;
		greydt = 1.0;
	} else {
		greym = 0.5 - (greym / greydt);
		greydt = 1.0 / greydt;
	}
	kims = 0.0;
	imxns = 0.0;
	k = 0;
	for ( y = 0; y < height; y++ )
	{
		lined = dst_bits + y * pitch;
		for ( x = 0; x < width; x++ )
		{
			im = p_im[y][x].s;
			imx = (double)(im + 1) / 3.0 / 256.0;
			imx = (imx * imdt) + immed;
			imx = (imx * 255.0 + 1.0) / 256.0;
			imxn = (greyO[k] * greydt) + greym + 1.0 / 255.0;
			imxn = (imxn * 255.0 + 1.0) / 256.0;
			k++;
			imxns += imxn;
			if (imxn < 0.0) {imxn = 0.0;}
			if (imxn > 1.0) {imxn = 1.0;}
			kim = imxn/imx;
			kims += kim;
			for (d = 0; d < 3; d++)
			{
				im = p_im[y][x].c[d];
				imx = (double)(im);
				im = MIN(MAX((int)0, (int)(imx * kim + 0.5)), (int)255);
				lined[x * 3 + d] = (BYTE)(im);
			}
		}
	}
	kims /= imsz;
	printf("k= %f\n", kims);
	imxns /= imsz;
	imxns = (imxns * 256.0 - 1.0) / 255.0;
	printf("N= %f\n", imxns);
	FreeImage_SetDotsPerMeterX(dst_dib, FreeImage_GetDotsPerMeterX(src_dib));
	FreeImage_SetDotsPerMeterY(dst_dib, FreeImage_GetDotsPerMeterY(src_dib));

	delete [] greyI;
	delete [] greyO;
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

void ImthresholdFilterMSRTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Multiscale Retinex.\n");
	printf("IPOL Image Processing On Line http://www.ipol.im/\n\n");
}

void ImthresholdFilterMSRUsage()
{
	printf("Usage : imthreshold-fmsr [options] <input_file> <output_file>\n\n");
	printf("options:\n");
	printf("          -1 N.N  sigma 1 (double, optional, default = 15.0)\n");
	printf("          -2 N.N  sigma 2 (double, optional, default = 80.0)\n");
	printf("          -3 N.N  sigma 3 (double, optional, default = 250.0)\n");
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
	double sf1 = 15.0;
	double sf2 = 80.0;
	double sf3 = 250.0;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":1:2:3:h")) != -1)
	{
		switch(opt)
		{
			case '1':
				sf1 = atof(optarg);
				break;
			case '2':
				sf2 = atof(optarg);
				break;
			case '3':
				sf3 = atof(optarg);
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
	
	ImthresholdFilterMSRTitle();
	
	if(optind + 2 > argc || fhelp > 0)
	{
		ImthresholdFilterMSRUsage();
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
					dst_dib = ImthresholdFilterMSR(dib, sf1, sf2, sf3);
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
