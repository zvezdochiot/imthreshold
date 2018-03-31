
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

//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <FreeImage.h>
#include "Utilities.h"

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* FI_filter(FIBITMAP* src_dib, int choice, int size_k)
{
	// choice = 1: Erosion

	// choice = 2: Dilation	

	// choice = 3: Eroded Contour

	// choice = 4: Dilated Contour

	// size_k: kernel size = 3...7

	unsigned width = FreeImage_GetWidth(src_dib);
	
	unsigned height = FreeImage_GetHeight(src_dib);
	
	unsigned pitch = FreeImage_GetPitch(src_dib);
	
	unsigned bpp = FreeImage_GetBPP(src_dib);

	unsigned btpp = bpp/8;
	
	unsigned row, col, i, j, d;
	
	int k_index;
	
	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, bpp);

	unsigned dst_pitch = FreeImage_GetPitch(dst_dib);
	
	BYTE* src_bits = (BYTE*)FreeImage_GetBits(src_dib); // The image raster
	
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib); // The image raster

	BYTE* src_end_row = src_bits + (height-1) * pitch;

	int end_col = width - 1;
	
	BYTE* lines, *linek, *lined;

	BYTE* pval1 = new BYTE[btpp];

	BYTE* pval2 = new BYTE[btpp];

	
	for (row = 0; row < height; row++)
	{
		lines = src_bits + row * pitch;
		
		lined = dst_bits + row * dst_pitch;
		
		for (col = 0; col < width; col++)
		{
			memset(pval1, 0xFF, sizeof(BYTE)*btpp); // biggest possible

			memset(pval2, 0x00, sizeof(BYTE)*btpp); // smallest possible		

			// kernel processing
			for (i = 0; i < size_k; i++)
				for (j = 0; j < size_k; j++)
				{
					linek = lines + (i-1) * pitch;

					if (linek < src_bits) linek = src_bits;
					if (linek > src_end_row) linek = src_end_row;

					for (d=0; d<btpp; d++)
					{
					k_index = col+j-1;

					if (k_index < 0) k_index = 0;
					if (k_index > end_col) k_index = end_col;

					pval1[d] = MIN(pval1[d],linek[k_index * btpp + d]); // erosion

					pval2[d] = MAX(pval2[d],linek[k_index * btpp + d]); // dilation

					}
				}

				for (d=0; d<btpp; d++)
				{
				switch (choice)
				{
				case 1:

				lined[col * btpp + d] =  pval1[d]; // Erosion

				break;

				case 2:

				lined[col * btpp + d] =  pval2[d]; // Dilation

				break;			

				case 3:

				lined[col * btpp + d] =  lines[col * btpp + d] - pval1[d]; // Eroded Contour

				break;

				case 4:

				lined[col * btpp + d] =  pval2[d] - lines[col * btpp + d]; // Dilated Contour
				}
				}
		}
	}

	if(bpp == 8)
	{
		// copy the original palette to the destination bitmap
		
		RGBQUAD *src_pal = FreeImage_GetPalette(src_dib);
		RGBQUAD *dst_pal = FreeImage_GetPalette(dst_dib);
		memcpy(&dst_pal[0], &src_pal[0], 256 * sizeof(RGBQUAD));		
	}
	
	// Copying the DPI...
	
	FreeImage_SetDotsPerMeterX(dst_dib, FreeImage_GetDotsPerMeterX(src_dib));
	
	FreeImage_SetDotsPerMeterY(dst_dib, FreeImage_GetDotsPerMeterY(src_dib));

	delete [] pval1;

	delete [] pval2;
	
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
	printf("Morphological filter: 1: Erosion, 2: Dilation, 3: Eroded Contour, 4: Dilated Contour.\n\n");
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	if(argc != 5) {
		printf("Usage : imthreshold-fmorph <input_file> <output_file> <filter_number> <kernel_size>\n");
		return 0;
	}
	
	FIBITMAP *dib = GenericLoader(argv[1], 0);
	
	int choice = atoi(argv[3]);
	
	int size_k = atoi(argv[4]);
	
	if (dib)
	{		
		// bitmap is successfully loaded!
		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib = FI_filter(dib, choice, size_k);
				
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


