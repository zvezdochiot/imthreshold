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

// This algorithm was taken from the C++ Augmented Reality Toolkit sourcecodes
// http://www.dandiggins.co.uk/arlib-1.html
// and adopted for the FreeImage library
//
// Copyright (C) 2007-2008:
// monday2000  monday2000@yandex.ru

#include <inttypes.h>
#include <math.h>
#include <stdlib.h> 
#include <memory.h>

#ifndef IMTHRESHOLD_H
#define IMTHRESHOLD_H
#define MAXVAL 256
#define PI  ((double)3.14159265358979323846264338327950288419716939937510)

typedef uint8_t  BYTE;
typedef uint16_t  WORD;

typedef struct mpixel
{
	BYTE c[3];
	WORD s;
}
IMTpixel;

typedef struct mcluster
{
	int c[3];
	unsigned n;
}
IMTcluster;

typedef struct minfo
{
	unsigned width;
	unsigned height;
	unsigned bpp;
	unsigned min;
	unsigned max;
	double mid;
	double mean;
	double std;
	double wb;
}
IMTinfo;

////////////////////////////////////////////////////////////////////////////////

/// Max function
template <class T> T MAX(T a, T b) {
	return (a > b) ? a: b;
}

/// Min function
template <class T> T MIN(T a, T b) {
	return (a < b) ? a: b;
}

/** This procedure computes minimum min and maximum max
 of n numbers using only (3n/2) - 2 comparisons.
 min = L[i1] and max = L[i2].
 ref: Aho A.V., Hopcroft J.E., Ullman J.D., 
 The design and analysis of computer algorithms, 
 Addison-Wesley, Reading, 1974.
*/
template <class T> void 
MAXMIN(const T* L, long n, T& max, T& min) {
	long i1, i2, i, j;
	T x1, x2;
	long k1, k2;

	i1 = 0; i2 = 0; min = L[0]; max = L[0]; j = 0;
	if((n % 2) != 0)  j = 1;
	for(i = j; i < n; i+= 2) {
		k1 = i; k2 = i+1;
		x1 = L[k1]; x2 = L[k2];
		if(x1 > x2)	{
			k1 = k2;  k2 = i;
			x1 = x2;  x2 = L[k2];
		}
		if(x1 < min) {
			min = x1;  i1 = k1;
		}
		if(x2 > max) {
			max = x2;  i2 = k2;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

int compare_colors(const void *elem1, const void *elem2);
void MFilterMean(unsigned width, unsigned height, int radius, double** p_im, double** p_mean);
void MFilterVariance(unsigned width, unsigned height, int radius, double** p_im, double** p_mean, double** p_var);
double MFilterMedian(unsigned width, unsigned height, double** p_im, double** p_var);
IMTpixel IMTset(BYTE c0, BYTE c1, BYTE c2);
IMTpixel IMTrefilter1p(IMTpixel IMTim, IMTpixel IMTimf);
IMTpixel IMTinterpolation(IMTpixel** p_im, unsigned height, unsigned width, double y, double x);
BYTE IMTmax(IMTpixel** IMTim, unsigned height, unsigned width);
BYTE IMTmin(IMTpixel** IMTim, unsigned height, unsigned width);
double IMTmean(IMTpixel** IMTim, unsigned height, unsigned width);
double IMTdev(IMTpixel** IMTim, double immean, unsigned height, unsigned width);
double IMTwb(IMTpixel** IMTim, double immean, unsigned height, unsigned width);
int IMTFilterSNorm(IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterInvertBW(BYTE** p_im, unsigned height, unsigned width);
double IMTdist(IMTpixel IMTim0, IMTpixel IMTim1);
double IMTdist3c2p(IMTpixel IMTim, double* IMTimc);
IMTpixel IMTmeanIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTmaxIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTminIc(IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTaverageIc(IMTpixel** IMTim, IMTpixel IMTima, unsigned y0, unsigned x0, unsigned y1, unsigned x1, double part);
void IMTFilterDespeck2(BYTE** p_im, unsigned height, unsigned width, unsigned Ksize);
void IMTFilterDNeuro2(BYTE** p_im, unsigned height, unsigned width, unsigned Ksize, double lambda, unsigned lnum);
void IMTBlurMask(IMTpixel** p_im, BYTE** m_im, unsigned height, unsigned width, int radius);
void IMTReduceBW(BYTE** m_im, BYTE** g_im, unsigned height, unsigned width, unsigned heightg, unsigned widthg, unsigned kred, BYTE preval, BYTE result);
IMTpixel IMTFilterIllumCorr(IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width);
double IMTFilterFindSkew(IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterRotate(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double angle);
IMTinfo IMTFilterInfo(IMTpixel** p_im, unsigned height, unsigned width, unsigned bpp);
void IMTFilterInvert(IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterCopy(IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width);
void IMTFilterAdSmooth(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius);
void IMTFilterBGFGLsep(IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, double doverlay);
IMTpixel IMTFilterGreyWorld(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width);
double IMTFilterLevelMean(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double contour, double thres, int lower_bound, int upper_bound);
double IMTFilterLevelSigma(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double thres, double fpart);
IMTpixel IMTmeanIcM(IMTpixel** IMTim, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanPtIcM(IMTpixel** IMTim, IMTpixel IMTimm, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
double IMTwbIcM(IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanThIcM(IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx, int tflag);
IMTpixel IMTmeanRadIcM(IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanBlIcM(IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanMinIcM(IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanMaxIcM(IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
int IMTFilterKMeans(IMTpixel** IMTim, unsigned height, unsigned width, unsigned ncluster, unsigned iters);
void IMTFilterPMean(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int fmode, bool fneared);
int IMTFilterRetinex(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double sigma);
void IMTSelGaussInitMatrix (double radius, double *mat, int num);
void IMTFilterSelGauss (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int maxdelta);
double IMTFilterShrink(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, int thres);
void IMTFilterSobel(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width);
void IMTFilterUnRipple(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double thres);
void IMTGaussLineMatrix (double *cmatrix, double radius);
void IMTFilterGaussBlur (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius);
double IMTFilterBgDiv (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned ndiv);
void IMTFilterUnsharpMask (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width, double amount, int threshold);
void IMTFilterWiener(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double noise_variance);
double BiCubicKernel(double x);
void IMTFilterSBicub(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
void IMTFilterSBilin(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
int IMTFilterSBWMag2(BYTE** d_im, BYTE** r_im, unsigned height, unsigned width, unsigned height2, unsigned width2);
int IMTFilterSBWReduce2(BYTE** d_im, BYTE** r_im, unsigned height, unsigned width, unsigned height2, unsigned width2);
void IMTFilterSHRIS(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int smode);
void IMTFilterSReduce(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int smode);
void IMTFilterSNearest(IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
int IMTFilterTAbutaleb(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius);
int IMTFilterTBernsen(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, unsigned contrast_limit, bool set_doubt_to_low);
int IMTFilterTBHT(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTBiMod(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta);
int IMTFilterTDalg(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int region_size);
int IMTFilterTDither(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTDjVuL(IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, int wbmode, double anisotropic, double doverlay);
int IMTFilterTEnt(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTEqBright(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int fmode);
int IMTFilterGatosBG(IMTpixel** p_im, BYTE** d_im, IMTpixel** bg_im, unsigned height, unsigned width, int radius);
int IMTFilterTGatos(IMTpixel** p_im, BYTE** d_im, IMTpixel** bg_im, BYTE** g_im, unsigned height, unsigned width, double q, double p1, double p2);
int IMTFilterTGrad (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, bool contour);
int IMTFilterTHalftone2(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTJanni(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTKMeans(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned knum, unsigned iters);
int IMTFilterTNiblack(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound);
int IMTFilterTOtsu(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTRot(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, bool weight);
int IMTFilterTSauvola(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int dynamic_range, int lower_bound, int upper_bound);
int IMTFilterTText(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned contour, unsigned radius);
int IMTFilterTTsai(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int shift);
int IMTFilterTWhiteRohrer(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int x_lookahead, int y_lookahead, int bias_mode, int bias_factor, int f_factor, int g_factor);

#endif // IMTHRESHOLD_H
