
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

FIBITMAP* FI_filter(FIBITMAP* src_dib, int f[3][3], int weight, int add)
{
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

	int* pval = new int[btpp];

	int* psum = new int[btpp];
	
	
	for (row = 0; row < height; row++)
	{
		lines = src_bits + row * pitch;
		
		lined = dst_bits + row * dst_pitch;
		
		for (col = 0; col < width; col++)
		{
			memset(psum, 0, sizeof(int)*btpp);

			// kernel processing
			for (i = 0; i < 3; i++)
				for (j = 0; j < 3; j++)
				{					
					linek = lines + (i-1) * pitch;

					if (linek < src_bits) linek = src_bits;
					if (linek > src_end_row) linek = src_end_row;

					for (d=0; d<btpp; d++)
					{
					k_index = col+j-1;

					if (k_index < 0) k_index = 0;
					if (k_index > end_col) k_index = end_col;

					psum[d] += linek[k_index * btpp + d] * f[i][j];
					}
				}
				
				for (d=0; d<btpp; d++)
				{
				pval[d] = psum[d] / weight + add;
				
				// clamp and place result in destination pixel
				lined[col * btpp + d] = (BYTE)MIN(MAX((int)0, (int)(pval[d] + 0.5)), (int)255);
				}
	
				//col += btpp;		
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

	delete [] pval;

	delete [] psum;
	
	return dst_dib;
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* run_filter(FIBITMAP* dib, int choice)
{
	int f[3][3]; // filter
	
	int weight = 1; // divider
	
	int add = 0; // value sometimes added to the divider
	
	switch (choice)
	{
	case 1:
		{			
			//*********************************************
			// 1. Blur:
			
			f[0][0] = 1; f[0][1] = 1; f[0][2] = 1;
			
			f[1][0] = 1; f[1][1] = 1; f[1][2] = 1;
			
			f[2][0] = 1; f[2][1] = 1; f[2][2] = 1;  
			
			weight = 9;
			
			add = 0;
			
			//*********************************************
			return FI_filter(dib, f, weight, add);	
		}

	case 2:
		{			
			//*********************************************
			// 2. Gaussian Blur:
			
			f[0][0] = 0; f[0][1] = 1; f[0][2] = 0;
			
			f[1][0] = 1; f[1][1] = 4; f[1][2] = 1;
			
			f[2][0] = 0; f[2][1] = 1; f[2][2] = 0;  
			
			weight = 8;
			
			add = 0;
			
			//*********************************************
			return FI_filter(dib, f, weight, add);	
		}

	case 3:
		{			
			//*********************************************
			// 3. Sharpening:
			
			f[0][0] = 0; f[0][1] = -1; f[0][2] = 0;
			
			f[1][0] = -1; f[1][1] = 9; f[1][2] = -1;
			
			f[2][0] = 0; f[2][1] = -1; f[2][2] = 0;  
			
			weight = 5;
			
			add = 0;
			
			//*********************************************
			return FI_filter(dib, f, weight, add);	
		}

	case 4:
		{			
			//*********************************************
			// 4. Laplasian: (contour selection)
			
			f[0][0] = -1; f[0][1] = -1; f[0][2] = -1;
			
			f[1][0] = -1; f[1][1] = 8; f[1][2] = -1;
			
			f[2][0] = -1; f[2][1] = -1; f[2][2] = -1;  
			
			weight = 1;
			
			add = 128;
			
			//*********************************************
			return FI_filter(dib, f, weight, add);	
		}

	case 5:
		{			
			//*********************************************
			// 5. Emboss 135 degrees
			
			f[0][0] = 1; f[0][1] = 0; f[0][2] = 0;
			
			f[1][0] = 0; f[1][1] = 0; f[1][2] = 0;
			
			f[2][0] = 0; f[2][1] = 0; f[2][2] = -1;  
			
			weight = 1;
			
			add = 128;
			
			//*********************************************
			return FI_filter(dib, f, weight, add);	
		} 

	case 6:
		{			
			//*********************************************
			// 6. Emboss 90 degrees, 50%
			
			f[0][0] = 0; f[0][1] = 1; f[0][2] = 0;
			
			f[1][0] = 0; f[1][1] = 0; f[1][2] = 0;
			
			f[2][0] = 0; f[2][1] = -1; f[2][2] = 0;  
			
			weight = 2;
			
			add = 128;
			
			//*********************************************
			return FI_filter(dib, f, weight, add);	
		} 
	}
	return NULL;
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
	printf("Filters: 1. Blur, 2. Gaussian Blur, 3. Sharpening, 4. Laplasian: (contour selection), 5. Emboss 135 degrees, 6. Emboss 90 degrees, 50%%.\n\n");
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
	if(argc != 4) {
		printf("Usage : imthreshold-fconv <input_file> <output_file> <filter_number>\n");
		return 0;
	}
	
	FIBITMAP *dib = GenericLoader(argv[1], 0);
	
	int choice = atoi(argv[3]);
	
	if (dib)
	{		
		// bitmap is successfully loaded!
		
		if (FreeImage_GetImageType(dib) == FIT_BITMAP)
		{
			if (FreeImage_GetBPP(dib) == 8 || FreeImage_GetBPP(dib) == 24)
			{
				FIBITMAP* dst_dib = run_filter(dib, choice);
				
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


