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

BYTE ImthresholdGet1BitPixel(BYTE *bits, unsigned x)
{
	return (bits[x >> 3] & (0x80 >> (x & 0x07))) != 0 ? 255 : 0;
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdSetPixel(BYTE *bits, unsigned x, BYTE* value)
{
	*value ? bits[x >> 3] |= (0x80 >> (x & 0x7)) : bits[x >> 3] &= (0xFF7F >> (x & 0x7));
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdGetData(FIBITMAP* dib, IMTpixel** p_im)
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
				p_im[y][x].s += p_im[y][x].c[d];
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTset(BYTE c0, BYTE c1, BYTE c2)
{
	IMTpixel im;
	WORD s;
	
	im.c[0] = (BYTE)c0;
	im.c[1] = (BYTE)c1;
	im.c[2] = (BYTE)c2;
	im.s = (WORD)c0 + (WORD)c1 + (WORD)c2;
		
	return im;
}

////////////////////////////////////////////////////////////////////////////////

double IMTmean(IMTpixel** IMTim, unsigned height, unsigned width)
{
	unsigned y, x;
	double imx, immean = 0;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			imx = (double)IMTim[y][x].s;
			immean += imx;
		}
	}
	immean /= width;
	immean /= height;
	immean /= 3.0;
	
	return immean;
}

////////////////////////////////////////////////////////////////////////////////

double IMTwb(IMTpixel** IMTim, double immean, unsigned height, unsigned width)
{
	unsigned y, x;
	double imx;
	long imwn = 0;
	double imwb;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			imx = (double)IMTim[y][x].s / 3.0;
			if (imx > immean) {imwn++;}
		}
	}
	imwb = (double)imwn;
	imwb /= width;
	imwb /= height;
	
	return imwb;
}

////////////////////////////////////////////////////////////////////////////////

double IMTdist(IMTpixel IMTim0, IMTpixel IMTim1)
{
	unsigned d;
	double imd, imds = 0;
	
	for (d = 0; d < 3; d++)
	{
		imd = (double)IMTim0.c[d];
		imd -= (double)IMTim1.c[d];
		imd /= 255;
		imd *= imd;
		imds += imd;
	}
	imds /= 3;
	
	return sqrt(imds);
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
	unsigned y, x, d;
	unsigned imm, ims, n = 0;
	IMTpixel immean;
	
	ims = 0;
	for (y = y0; y < y1; y++)
	{
		for (x = x0; x < x1; x++)
		{
			imm = IMTim[y][x].s;
			ims += imm;
			n++;
		}
	}
	if (n > 0) {ims /= n;}
	immean.s = (WORD)ims;
	for (d = 0; d < 3; d++)
	{
		ims = 0;
		for (y = y0; y < y1; y++)
		{
			for (x = x0; x < x1; x++)
			{
				imm = IMTim[y][x].c[d];
				ims += imm;
			}
		}
		if (n > 0) {ims /= n;}
		immean.c[d] = (BYTE)ims;
	}
	
	return immean;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmaxIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
	unsigned y, x, d;
	unsigned imm, immax, ny, nx;
	IMTpixel imtmax;
	
	immax = 0;
	ny = y0;
	nx = x0;
	for (y = y0; y < y1; y++)
	{
		for (x = x0; x < x1; x++)
		{
			imm = IMTim[y][x].s;
			if (imm > immax)
			{
				immax = imm;
				ny = y;
				nx = x;
			}
		}
	}
	imtmax = IMTim[ny][nx];
	return imtmax;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTminIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
	unsigned y, x, d;
	unsigned imm, immin, ny, nx;
	IMTpixel imtmin;
	
	immin = 768;
	ny = y0;
	nx = x0;
	for (y = y0; y < y1; y++)
	{
		for (x = x0; x < x1; x++)
		{
			imm = IMTim[y][x].s;
			if (imm < immin)
			{
				immin = imm;
				ny = y;
				nx = x;
			}
		}
	}
	imtmin = IMTim[ny][nx];
	return imtmin;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTaverageIc(IMTpixel** IMTim, IMTpixel IMTima, unsigned y0, unsigned x0, unsigned y1, unsigned x1, double part)
{
	unsigned y, x, d;
	unsigned imm, ims, n = 0;
	IMTpixel immean;
	double imx, parts = part + 1.0;
	
	ims = 0;
	for (y = y0; y < y1; y++)
	{
		for (x = x0; x < x1; x++)
		{
			imx = (double)IMTim[y][x].s;
			imx *= part;
			imx += (double)IMTima.s;
			imx /= parts;
			imm = (unsigned)imx;
			IMTim[y][x].s = (WORD)imm;
			ims += imm;
			n++;
		}
	}
	if (n > 0) {ims /= n;}
	immean.s = (WORD)ims;
	for (d = 0; d < 3; d++)
	{
		ims = 0;
		for (y = y0; y < y1; y++)
		{
			for (x = x0; x < x1; x++)
			{
				imx = (double)IMTim[y][x].c[d];
				imx *= part;
				imx += (double)IMTima.c[d];
				imx /= parts;
				imm = (unsigned)imx;
				IMTim[y][x].c[d] = (BYTE)imm;
				ims += imm;
			}
		}
		if (n > 0) {ims /= n;}
		immean.c[d] = (BYTE)ims;
	}
	
	return immean;
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

void IMTFilterDjVuL(IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, int wbmode, double anisotropic)
{
	unsigned widthbg = (width + bgs - 1) / bgs;
	unsigned heightbg = (height + bgs - 1) / bgs;
	unsigned widthfg = (widthbg + fgs - 1) / fgs;
	unsigned heightfg = (heightbg + fgs - 1) / fgs;
	unsigned y, x, d, l, i, j, y0, x0, y1, x1, y0b, x0b, y1b, x1b, yb, xb, yf, xf;
	unsigned blsz = 1, mblsz;
	double immean, imwb;
	BYTE fgbase, bgbase;
	unsigned cnth, cntw;
	IMTpixel pim, fgim, bgim;
	double fgdist, bgdist, fgpart, bgpart, mpart, gold2, gold2h, fgk = 1.0;
	unsigned fgsum[3], bgsum[3], fgnum, bgnum;
	
	IMTpixel** fgt_im;
	fgt_im = (IMTpixel**)malloc(heightbg * sizeof(IMTpixel*));
	for (y = 0; y < heightbg; y++) {fgt_im[y] = (IMTpixel*)malloc(widthbg * sizeof(IMTpixel));}
	
	gold2 = (sqrt(5) + 1) / 2; // 1.61803398875
	gold2h = gold2 / 2; // 0.809016994375
	mpart = 1.0;
//	if (mpart < 1.0){ mpart = 1.0;}
	
	for (l = 0; l < level; l++)
	{
		blsz *= 2;
	}
	printf("Level= %d\n", level);
	mblsz = bgs * blsz;
	printf("MaxBlockSize= %d\n", mblsz);
	printf("Anisotropic= %f\n", anisotropic);
	immean = IMTmean(p_im, height, width);
	printf("Mean= %f\n", immean);
	imwb = IMTwb(p_im, immean, height, width);
	printf("W/B= %f\n", imwb);
	if (anisotropic == 0)
	{
		fgk = sqrt(1.5 - imwb);
	}
	if (wbmode == 0)
	{
		if (imwb < 0.5) {wbmode = -1;} else {wbmode = 1;}
	}
	if (wbmode > 0)
	{
		printf("Mode= white\n");
	} else {
		printf("Mode= black\n");
	}
	
	if (wbmode < 0)
	{
		fgbase = 255;
		bgbase = 0;
	} else {
		fgbase = 0;
		bgbase = 255;
	}
	for (y = 0; y < heightbg; y++)
	{
		for (x = 0; x < widthbg; x++)
		{
			fgt_im[y][x] = IMTset(fgbase, fgbase, fgbase);
			bg_im[y][x] = IMTset(bgbase, bgbase, bgbase);
		}
	}
//	fgk = exp(anisotropic/exp(1.0));
	for (l = 0; l <= level; l++)
	{
		cnth = (heightbg + blsz - 1) / blsz;
		cntw = (widthbg + blsz - 1) / blsz;
		for (i = 0; i < cnth; i++)
		{
			y0 = i * blsz * bgs;
			y1 = y0 + blsz * bgs;
			if (y1 > height) {y1 = height;}
			y0b = i * blsz;
			y1b = y0b + blsz;
			if (y1b > heightbg) {y1b = heightbg;}
			for (j = 0; j < cntw; j++)
			{
				x0 = j * blsz * bgs;
				x1 = x0 + blsz * bgs;
				if (x1 > width) {x1 = width;}
				x0b = j * blsz;
				x1b = x0b + blsz;
				if (x1b > widthbg) {x1b = widthbg;}

				pim = IMTmeanIc(p_im, y0, x0, y1, x1);

				fgim = IMTmeanIc(fgt_im, y0b, x0b, y1b, x1b);
				bgim = IMTmeanIc(bg_im, y0b, x0b, y1b, x1b);

				fgdist = IMTdist(pim, fgim);
				bgdist = IMTdist(pim, bgim);

				fgk = (fgdist + bgdist);
				if (fgk > 0)
				{
					fgk = (bgdist - fgdist) / fgk;
					fgk *= anisotropic;
/*
					fgk *= gold2h;
					fgk += 1.0;
*/
					fgk = exp(fgk);
				} else {
					fgk = 1.0;
				}
				for (d = 0; d < 3; d++)
				{
					fgsum[d] = 0;
					bgsum[d] = 0;
				}
				fgnum = 0;
				bgnum = 0;
				for (y = y0; y < y1; y++)
				{
					for (x = x0; x < x1; x++)
					{
						pim = p_im[y][x];
						fgdist = IMTdist(pim, fgim);
						bgdist = IMTdist(pim, bgim);
						if (fgdist * fgk < bgdist)
						{
							for (d = 0; d < 3; d++)
							{
								fgsum[d] += (int)pim.c[d];
							}
							fgnum++;
						} else {
							for (d = 0; d < 3; d++)
							{
								bgsum[d] += (int)pim.c[d];
							}
							bgnum++;
						}
					}
				}
				fgpart = mpart;
				bgpart = mpart;
				if (fgnum > 0)
				{
					for (d = 0; d < 3; d++)
					{
						fgsum[d] /= fgnum;
						fgim.c[d] = fgsum[d];
					}
					fgim = IMTaverageIc(fgt_im, fgim, y0b, x0b, y1b, x1b, fgpart);
				}
				if (bgnum > 0)
				{
					for (d = 0; d < 3; d++)
					{
						bgsum[d] /= bgnum;
						bgim.c[d] = bgsum[d];
					}
					bgim = IMTaverageIc(bg_im, bgim, y0b, x0b, y1b, x1b, bgpart);
				}

			}
		}
		blsz /= 2;
	}
	for (y = 0; y < heightfg; y++)
	{
		y0 = y * fgs;
		y1 = y0 + fgs;
		if (y1 > heightbg) {y1 = heightbg;}
		for (x = 0; x < widthfg; x++)
		{
			x0 = x * fgs;
			x1 = x0 + fgs;
			if (x1 > widthbg) {x1 = widthbg;}
			fg_im[y][x] = IMTmeanIc(fgt_im, y0, x0, y1, x1);

		}
	}
	for (y = 0; y < height; y++)
	{
		yb = y / bgs;
		yf = yb / fgs;
		for (x = 0; x < width; x++)
		{
			xb = x /bgs;
			xf = xb / fgs;
			pim = p_im[y][x];
			fgim = fg_im[yf][xf];
			bgim = bg_im[yb][xb];
			fgdist = IMTdist(pim, fgim);
			bgdist = IMTdist(pim, bgim);
			if (fgdist < bgdist)
			{
				m_im[y][x] = 0;
			} else {
				m_im[y][x] = 255;
			}
		}
	}
	
	for (y = 0; y < heightbg; y++){free(fgt_im[y]);}
	free(fgt_im);
}

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
