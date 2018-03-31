/*
*
* Copyright (C) 2001-2005 Ichiro Fujinaga, Michael Droettboom, and Karl MacMillan
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*
 Color-based thresholding using the algorithm from DjVu image
 compression.

 See Section 5.1 in:

   Bottou, L., P. Haffner, P. G. Howard, P. Simard, Y. Bengio and
   Y. LeCun.  1998.  High Quality Document Image Compression with
   DjVu.  AT&T Labs, Lincroft, NJ.

   http://research.microsoft.com/~patrice/PDF/jei.pdf

 This implementation features an additional extension to the
 algorithm described above.  Once the background and foreground
 colors are determined for each block, the image is thresholded by
 interpolating the foreground and background colors between the
 blocks.  This prevents "blockiness" along boundaries of strong
 color change.

 *smoothness* : default = 0.2
   The amount of effect that parent blocks have on their children
   blocks.  Higher values will result in more smoothness between
   blocks.  Expressed as a percentage between 0.0 and 1.0.

 *max_block_size* : default = 512
   The size of the largest block to determine a threshold.

 *min_block_size* : default = 16
   The size of the smallest block to determine a threshold.

 *block_factor* : default = 2
   The number of child blocks (in each direction) per parent block.
   For instance, a *block_factor* of 2 results in 4 children per
   parent.
*/

// This algorithm was taken from the gamera.sf.net sourcecodes
// and adopted for the FreeImage library
//
//	Copyright (C) 2007-2008:
//	monday2000	monday2000@yandex.ru

#include <FreeImage.h>
#include "Utilities.h"
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////

class RGBD
{
public:
	double r;
	double g;
	double b;
	RGBD(){};
	RGBD(RGBQUAD*);
	RGBD(BYTE*);
}; 

RGBD::RGBD(RGBQUAD* p_rgb)
{
	r = (double)p_rgb->rgbRed;
	g = (double)p_rgb->rgbGreen;
	b = (double)p_rgb->rgbBlue;
}

RGBD::RGBD(BYTE* lines)
{
	r = (double)lines[FI_RGBA_RED];
	g = (double)lines[FI_RGBA_GREEN];
	b = (double)lines[FI_RGBA_BLUE];
}

////////////////////////////////////////////////////////////////////////////////

inline void SetPixel(BYTE *bits, unsigned x, BYTE* value)
{   // this function is simplified from FreeImage_SetPixelIndex
	
	*value ? bits[x >> 3] |= (0x80 >> (x & 0x7)) : bits[x >> 3] &= (0xFF7F >> (x & 0x7));
}

////////////////////////////////////////////////////////////////////////////////

inline double ImthresholdFilterDjvuDistance(RGBD& fg, RGBD& bg)
{
	// This approximates YUV distance, which is far more natural
	// than RGB distance.
	double r = fg.r - bg.r;
	double g = fg.g - bg.g;
	double b = fg.b - bg.b;
	
	return (0.75*r*r + g*g + 0.5*b*b);		
}

////////////////////////////////////////////////////////////////////////////////

#define CONVERGE_THRESHOLD 2

inline BOOL ImthresholdFilterDjvuConverged(RGBD& fg, RGBD& bg)
{
	return (ImthresholdFilterDjvuDistance(fg, bg) < CONVERGE_THRESHOLD);
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

void ImthresholdFilterDjvuThresholdRecurse(FIBITMAP* src_dib, unsigned upper, unsigned left,
							unsigned lower, unsigned right,
							const double smoothness,
							const unsigned min_block_size,
							FIBITMAP* fg_image, FIBITMAP* bg_image,
							RGBD fg_init, // black_color
							RGBD bg_init, // max_color
							const unsigned block_size)
{
	unsigned width = right - left;
	unsigned height = lower - upper;
	unsigned ul_x = left;
	unsigned ul_y = upper;
	unsigned lr_x = right;
	unsigned lr_y = lower;
	
	unsigned src_pitch = FreeImage_GetPitch(src_dib);
	unsigned bpp = FreeImage_GetBPP(src_dib);
	unsigned btpp = bpp/8;
	unsigned g_pitch = FreeImage_GetPitch(fg_image);
	
	BYTE* src_bits = (BYTE*)FreeImage_GetBits(src_dib) + upper * src_pitch + left*btpp;
	BYTE* fg_bits = (BYTE*)FreeImage_GetBits(fg_image);
	BYTE* bg_bits = (BYTE*)FreeImage_GetBits(bg_image);
	BYTE* lines, *linef, *lineb;
	
	RGBD fg = fg_init; // black_color
	RGBD bg = bg_init; // max_color
	RGBD last_fg, last_bg;
	
	BOOL fg_converged = false, bg_converged = false;
	
	RGBD fg_init_scaled;
	fg_init_scaled.r = fg_init.r * smoothness;
	fg_init_scaled.g = fg_init.g * smoothness;
	fg_init_scaled.b = fg_init.b * smoothness;	
	
	RGBD bg_init_scaled;
	bg_init_scaled.r = bg_init.r * smoothness;
	bg_init_scaled.g = bg_init.g * smoothness;
	bg_init_scaled.b = bg_init.b * smoothness;
	
	unsigned r, c, x, y;	
	unsigned fg_count, bg_count;
	
	RGBD fg_avg, bg_avg, rgbdlines;
	
	do
	{
		last_fg = fg;
		last_bg = bg;
		
		fg_avg.r = 0; fg_avg.g = 0; fg_avg.b = 0;
		bg_avg.r = 0; bg_avg.g = 0; bg_avg.b = 0;
		fg_count = 0; bg_count = 0;
		
		for (y = 0; y < height; y++)
		{
			lines = src_bits + y * src_pitch;
			for (x = 0; x < width; x++)				
			{				
				RGBD rgbdlines = RGBD(lines);
				double fg_dist = ImthresholdFilterDjvuDistance(rgbdlines, fg);
				double bg_dist = ImthresholdFilterDjvuDistance(rgbdlines, bg);
				if (fg_dist <= bg_dist)
				{
					fg_avg.r += (double)lines[FI_RGBA_RED];
					fg_avg.g += (double)lines[FI_RGBA_GREEN];
					fg_avg.b += (double)lines[FI_RGBA_BLUE];
					fg_count++;
				} else {
					bg_avg.r += (double)lines[FI_RGBA_RED];
					bg_avg.g += (double)lines[FI_RGBA_GREEN];
					bg_avg.b += (double)lines[FI_RGBA_BLUE];
					bg_count++;
				}				
				lines += btpp;
			}			
		}
		
		if (fg_count)
		{
			fg.r = (((fg_avg.r / fg_count) * (1.0 - smoothness)) + fg_init_scaled.r);
			fg.g = (((fg_avg.g / fg_count) * (1.0 - smoothness)) + fg_init_scaled.g);
			fg.b = (((fg_avg.b / fg_count) * (1.0 - smoothness)) + fg_init_scaled.b);
			fg_converged = ImthresholdFilterDjvuConverged(fg, last_fg);
		} else {
			fg_converged = true;
		}
		
		if (bg_count)
		{
			bg.r = (((bg_avg.r / bg_count) * (1.0 - smoothness)) + bg_init_scaled.r);
			bg.g = (((bg_avg.g / bg_count) * (1.0 - smoothness)) + bg_init_scaled.g);
			bg.b = (((bg_avg.b / bg_count) * (1.0 - smoothness)) + bg_init_scaled.b);
			bg_converged = ImthresholdFilterDjvuConverged(bg, last_bg);
		} else {
			bg_converged = true;
		}
	} while (!(fg_converged && bg_converged));
	
	if (block_size < min_block_size)
	{
		linef = fg_bits + (ul_y / min_block_size) * g_pitch;
		lineb = bg_bits + (ul_y / min_block_size) * g_pitch;
		
		(linef+(ul_x/min_block_size)*btpp)[FI_RGBA_RED] = (BYTE)fg.r;
		(linef+(ul_x/min_block_size)*btpp)[FI_RGBA_GREEN] = (BYTE)fg.g;
		(linef+(ul_x/min_block_size)*btpp)[FI_RGBA_BLUE] = (BYTE)fg.b;
		
		(lineb+(ul_x/min_block_size)*btpp)[FI_RGBA_RED] = (BYTE)bg.r;
		(lineb+(ul_x/min_block_size)*btpp)[FI_RGBA_GREEN] = (BYTE)bg.g;
		(lineb+(ul_x/min_block_size)*btpp)[FI_RGBA_BLUE] = (BYTE)bg.b;
	} else {
		for (r = 0; r <= (height - 1) / block_size; r++)
		{
			for (c = 0; c <= (width - 1) / block_size; c++)
			{
				left = c * block_size + ul_x;
				upper = r * block_size + ul_y;
				right = MIN((c + 1) * block_size + left, lr_x);
				lower = MIN((r + 1) * block_size + upper, lr_y);				
				
				ImthresholdFilterDjvuThresholdRecurse(src_dib, upper, left, lower, right, smoothness, min_block_size, fg_image, bg_image, fg, bg, block_size / 2);				
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* ImthresholdFilterDjvuThreshold(FIBITMAP* src_dib, const double smoothness, 
						 const unsigned max_block_size, const unsigned min_block_size,
						 const unsigned block_factor,
						 RGBD init_fg, 
						 RGBD init_bg)
{
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
	unsigned src_pitch = FreeImage_GetPitch(src_dib);
	unsigned bpp = FreeImage_GetBPP(src_dib);
	unsigned btpp = bpp/8;
	
	FIBITMAP* dst_dib = FreeImage_Allocate(width, height, 1);
	
	RGBQUAD *pal = FreeImage_GetPalette(dst_dib);
	pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
	pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
	
	unsigned dst_pitch = FreeImage_GetPitch(dst_dib);
	BYTE* src_bits = (BYTE*)FreeImage_GetBits(src_dib);
	BYTE* dst_bits = (BYTE*)FreeImage_GetBits(dst_dib);
	BYTE* lines, *lined, *linef, *lineb;
	
	unsigned x, y;
	BYTE val;
	
	// Create some temporary images to store the foreground and 
	// background colors for each block
	
	FIBITMAP* fg_image = FreeImage_Allocate(width / min_block_size, height / min_block_size, bpp);
	FIBITMAP* bg_image = FreeImage_Allocate(width / min_block_size, height / min_block_size, bpp);
	
	unsigned g_pitch = FreeImage_GetPitch(fg_image);	
	
	BYTE* fg_bits = (BYTE*)FreeImage_GetBits(fg_image);
	BYTE* bg_bits = (BYTE*)FreeImage_GetBits(bg_image);
	
	//****************************	
	
	ImthresholdFilterDjvuThresholdRecurse(src_dib, 0, 0, height, width, smoothness, min_block_size, fg_image, bg_image, init_fg, init_bg, max_block_size);
	
	//****************************	
	
	double fg_dist, bg_dist;
	unsigned x_frac, y_frac;
	
	RGBQUAD fg, bg;
	RGBD rgbgfg, rgbgbg, rgbdlines;
	
	for (y = 0; y < height; y++)
	{
		lines = src_bits + y * src_pitch;
		lined = dst_bits + y * dst_pitch;
		for (x = 0; x < width; x++)
		{
			x_frac = x / min_block_size;
			y_frac = y / min_block_size;
			
			linef = fg_bits + y_frac * g_pitch;		
			lineb = bg_bits + y_frac * g_pitch;
			
			fg.rgbRed = (linef+x_frac*btpp)[FI_RGBA_RED];
			fg.rgbGreen = (linef+x_frac*btpp)[FI_RGBA_GREEN];
			fg.rgbBlue = (linef+x_frac*btpp)[FI_RGBA_BLUE];
			
			bg.rgbRed = (lineb+x_frac*btpp)[FI_RGBA_RED];
			bg.rgbGreen = (lineb+x_frac*btpp)[FI_RGBA_GREEN];
			bg.rgbBlue = (lineb+x_frac*btpp)[FI_RGBA_BLUE];
			
			RGBD rgbdlines = RGBD(lines);
			RGBD rgbdfg = RGBD(&fg);
			RGBD rgbdbg = RGBD(&bg);
			fg_dist = ImthresholdFilterDjvuDistance(rgbdlines, rgbdfg);
			bg_dist = ImthresholdFilterDjvuDistance(rgbdlines, rgbdbg);
			
			if (fg_dist <= bg_dist) {val = 0;} else {val = 255;}
			SetPixel(lined, x, &val);
			lines += btpp;
		}		
	}
//	const char *output_filename_fg = "fg.tif";
//	FREE_IMAGE_FORMAT out_fif_fg = FreeImage_GetFIFFromFilename(output_filename_fg);
//	if(out_fif_fg != FIF_UNKNOWN)
//	{
//		FreeImage_Save(out_fif_fg, fg_image, output_filename_fg, 0);
//	}
//	const char *output_filename_bg = "bg.tif";
//	FREE_IMAGE_FORMAT out_fif_bg = FreeImage_GetFIFFromFilename(output_filename_bg);
//	if(out_fif_bg != FIF_UNKNOWN)
//	{
//		FreeImage_Save(out_fif_bg, bg_image, output_filename_bg, 0);
//	}
	FreeImage_Unload(fg_image);
	FreeImage_Unload(bg_image);
	
	return dst_dib;
}

////////////////////////////////////////////////////////////////////////////////

FIBITMAP* ImthresholdFilterDjvu(FIBITMAP* src_dib, double smoothness, int max_block_size, int min_block_size, int block_factor)
{
	unsigned width = FreeImage_GetWidth(src_dib);
	unsigned height = FreeImage_GetHeight(src_dib);
	unsigned src_pitch = FreeImage_GetPitch(src_dib);
	unsigned bpp = FreeImage_GetBPP(src_dib);
	unsigned btpp = bpp/8;
	
	printf("Smoothness= %f\n", smoothness);
	printf("MaxBlock= %d\n", max_block_size);
	printf("MinBlock= %d\n", min_block_size);
	printf("BlockFactor= %d\n", block_factor);

	BYTE* src_bits = (BYTE*)FreeImage_GetBits(src_dib);
	BYTE* lines;
	
	// We do an approximate histrogram here, using 6 bits per pixel
	// plane.  That greatly reduces the amount of memory required.
	
	RGBQUAD max_color;
	
	unsigned x, y;	
	unsigned max_count = 0;
	unsigned approx_color;
	unsigned x_val;
	
	int hist_size = 64 * 64 * 64 * sizeof(unsigned);
	unsigned* histogram = (unsigned*)malloc(hist_size);
	
	memset(histogram,0,hist_size);
	
	for (y = 0; y < height; y++)
	{
		lines = src_bits + y * src_pitch;
		for (x = 0; x < width; x++)
		{			
			approx_color = ((((unsigned)lines[FI_RGBA_RED]) << 10) | (((unsigned)lines[FI_RGBA_GREEN]) << 4) | (((unsigned)lines[FI_RGBA_BLUE]) >> 2));
			x_val = histogram[approx_color]++;
			if (x_val > max_count)
			{
				max_count = x_val;
				max_color.rgbRed = lines[FI_RGBA_RED] >> 2;
				max_color.rgbGreen = lines[FI_RGBA_GREEN] >> 2;
				max_color.rgbBlue = lines[FI_RGBA_BLUE] >> 2;
			}
			lines += btpp;
		}
	}
	
	free(histogram);
	
	if (max_color.rgbRed < 128 || max_color.rgbGreen < 128 || max_color.rgbBlue < 128)
	{
		max_color.rgbRed = 255;
		max_color.rgbGreen = 255;
		max_color.rgbBlue = 255;
	}
	
	RGBQUAD black_color;
	black_color.rgbRed = 0;
	black_color.rgbGreen = 0;
	black_color.rgbBlue = 0;
	
	return ImthresholdFilterDjvuThreshold(src_dib, smoothness, max_block_size, min_block_size, block_factor, RGBD(&black_color), RGBD(&max_color));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

void ImthresholdFilterDjvuTitle()
{
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("Color-based thresholding using the algorithm from DjVu image compression.\n");
	printf("This algorithm was taken from the gamera.sf.net sourcecodes and adopted for the FreeImage library.\n\n");
}

void ImthresholdFilterDjvuUsage()
{
	printf("Usage : imthreshold-tdjvu [options] <input_file>(RGB) <output_file>(BW)\n\n");
	printf("options:\n");
	printf("          -s N.N  smoothness (double, optional, default = 0.2)\n");
	printf("          -b N    max block size (int, optional, default = 512)\n");
	printf("          -m N    min block size (int, optional, default = 16)\n");
	printf("          -d N    block factor (int, optional, default = 2)\n");
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
	double smoothness = 0.2;
	int max_block_size = 512;
	int min_block_size = 16;
	int block_factor = 2;
	int fhelp = 0;
	while ((opt = getopt(argc, argv, ":s:b:m:d:h")) != -1)
	{
		switch(opt)
		{
			case 's':
				smoothness = atof(optarg);
				break;
			case 'b':
				max_block_size = atof(optarg);
				break;
			case 'm':
				min_block_size = atof(optarg);
				break;
			case 'd':
				block_factor = atof(optarg);
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
	
	ImthresholdFilterDjvuTitle();
	
	if(optind + 2 > argc || fhelp > 0 || max_block_size <= 0 || min_block_size <= 0 || block_factor <= 0)
	{
		ImthresholdFilterDjvuUsage();
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
				if (FreeImage_GetBPP(dib) == 1 || FreeImage_GetBPP(dib) == 8)
				{
					dst_dib = ImthresholdFilterNone(dib);
				}
				else
				{
					dst_dib = ImthresholdFilterDjvu(dib, smoothness, max_block_size, min_block_size, block_factor);
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
