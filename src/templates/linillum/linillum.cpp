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

// Linear illuminate filter image.

// This algorithm was taken from the TerraNoNames (http://mykaralw.narod.ru/)
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <FreeImage.h>
#include "Utilities.h"

FIBITMAP* ProcessFilter(FIBITMAP* src_dib)
{
	unsigned bpp = FreeImage_GetBPP(src_dib);
	unsigned btpp = bpp/8;
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
	unsigned pitch = FreeImage_GetPitch(src_dib);
	
	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, bpp);	
	BYTE* src_bits = (BYTE*)FreeImage_GetBits(src_dib); // The image raster
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib); // The image raster
	BYTE* lines, *lined, *linef;
	
	unsigned y, x, d;
	
	int im;
	double sn, sx, sy, sx2, sy2, sxy, si, six, siy;
	double imd, xd, yd, vt;
	double ki, kx, ky, kn, kt;
	
	sn=0;
	sx=0;
	sy=0;
	sx2=0;
	sy2=0;
	sxy=0;
	si=0;
	six=0;
	siy=0;
	for ( y = 0; y < height; y++ )
	{
		lines = src_bits + y * pitch;
		for ( x = 0; x < width; x++ )
		{
			im = 0;
			for (d=0; d<btpp; d++)
			{
				im += lines[x * btpp + d];
			}
			imd = double(im)/btpp;
			xd = double(x);
			yd = double(y);
			sn++;
			si += imd;
			six += (imd*xd);
			siy += (imd*yd);
			sx += xd;
			sy += yd;
			sx2 += (xd*xd);
			sy2 += (yd*yd);
			sxy += (xd*yd);
		}
	}
	kn = (sn*(sxy*sxy-sx2*sy2)+sx*sx*sy2+sx2*sy*sy-2*sx*sxy*sy);
	if (kn == 0.0)
	{
		kx = 0;
		ky = 0;
		ki = si / sn;
	} else {
		kx = ((sx*sy2-sxy*sy)*si+sn*(sxy*siy-six*sy2)-sx*sy*siy+six*sy*sy)/kn;
		ky = ((sx2*sy-sx*sxy)*si+sn*(sxy*six-sx2*siy)+sx*sx*siy-sx*six*sy)/kn;
		ki = ((sxy*sxy-sx2*sy2)*si+sx*(six*sy2-sxy*siy)+sy*(sx2*siy-sxy*six))/kn;
	}
	printf("kI=%f, kX=%f, kY=%f\n", ki, kx, ky);
	si /= sn;
	for ( y = 0; y < height; y++ )
	{
		lines = src_bits + y * pitch;
		lined = dst_bits + y * pitch;
		for ( x = 0; x < width; x++ )
		{
			kt = kx*x+ky*y+ki;
			if (kt == 0.0)
			{
				kt = 1;
			} else {
				kt = si/kt;
			}
			for (d=0; d<btpp; d++)
			{
				im = lines[x * btpp + d];
				vt = double(im)*kt;
				lined[x * btpp + d] = (BYTE)MIN(MAX((int)0, (int)(vt + 0.5)), (int)255);
			}
		}
	}
	if(bpp == 8)
	{
		RGBQUAD *src_pal = FreeImage_GetPalette(src_dib);
		RGBQUAD *dst_pal = FreeImage_GetPalette(dst_dib);
		memcpy(&dst_pal[0], &src_pal[0], 256 * sizeof(RGBQUAD));
	}
	FreeImage_SetDotsPerMeterX(dst_dib, FreeImage_GetDotsPerMeterX(src_dib));
	FreeImage_SetDotsPerMeterY(dst_dib, FreeImage_GetDotsPerMeterY(src_dib));
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

FIBITMAP* GenericLoader(const char* lpszPathName, int flag)
{	
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	
	fif = FreeImage_GetFileType(lpszPathName, 0);
	
	FIBITMAP* dib;
	
	if(fif == FIF_UNKNOWN)
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
		// ok, let's load the file
		dib = FreeImage_Load(fif, lpszPathName, flag);
		
		// unless a bad file format, we are done !
		if (!dib)
		{
			printf("%s%s%s\n","File \"", lpszPathName, "\" not found.");
			return NULL;
		}
	}	
	
	return dib;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB
	
	// initialize your own FreeImage error handler
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Linear illuminate filter image.\n");
	printf("TerraNoNames: http://mykaralw.narod.ru/.\n\n");
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	if(argc != 3) {
		printf("Usage : imthreshold-flinillum <input_file> <output_file>\n");
		return 0;
	}
	
	
	FIBITMAP *dib = GenericLoader(argv[1], 0);
	
	if (dib)
	{		
		// bitmap is successfully loaded!
		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib = ProcessFilter(dib);
				
				if (dst_dib)
				{					
					// save the filtered bitmap
					const char *output_filename = argv[2];
					
					// first, check the output format from the file name or file extension
					FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
					
					if(out_fif != FIF_UNKNOWN)
					{
						// then save the file
						FreeImage_Save(out_fif, dst_dib, output_filename, 0);
					}
					
					// free the loaded FIBITMAP
					FreeImage_Unload(dst_dib);					
				}
			}
			
			else
				
				printf("%s\n", "Unsupported color mode.");
		}
		
		else // non-FIT_BITMAP images are not supported.
			
			printf("%s\n", "Unsupported color mode.");
		
		FreeImage_Unload(dib);
	}	 
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
	
	return 0;
}