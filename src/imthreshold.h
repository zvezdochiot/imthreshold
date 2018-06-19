//  Zlib license
//
// ImThreshold header library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

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
        if(x1 > x2) {
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

IMTpixel IMTset (BYTE c0, BYTE c1, BYTE c2);
IMTpixel IMTcalcS (IMTpixel im);
IMTpixel IMTdiffS (IMTpixel im);
IMTpixel IMTrefilter1p (IMTpixel IMTim, IMTpixel IMTimf);
IMTpixel IMTinterpolation (IMTpixel** p_im, unsigned height, unsigned width, double y, double x);
BYTE IMTmax (IMTpixel** IMTim, unsigned height, unsigned width);
BYTE IMTmin (IMTpixel** IMTim, unsigned height, unsigned width);
double IMTmean (IMTpixel** IMTim, unsigned height, unsigned width);
double IMTdev (IMTpixel** IMTim, double immean, unsigned height, unsigned width);
double IMTwb (IMTpixel** IMTim, double immean, unsigned height, unsigned width);
void IMTFilterSMirror (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterSNorm (IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterSCompare (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width);
void IMTFilterSEdge (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width);
void IMTFilterInvertBW (BYTE** p_im, unsigned height, unsigned width);
/*
double IMTdist IMTpixel IMTim0, IMTpixel IMTim1);
*/
unsigned IMTdist (IMTpixel IMTim0, IMTpixel IMTim1);
double IMTdist3c2p (IMTpixel IMTim, double* IMTimc);
IMTpixel IMTmeanIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTmaxIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTminIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
IMTpixel IMTaverageIc (IMTpixel** IMTim, IMTpixel IMTima, unsigned y0, unsigned x0, unsigned y1, unsigned x1, double part);
void IMTFilterDespeck2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize);
unsigned IMTFilterDMag2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize);
void IMTFilterDNeuro2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize, double lambda, unsigned lnum);
double IMTFilterDphist (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize);
double IMTFilterDeNoiseDiff1p (IMTpixel** p_im, unsigned height, unsigned width, double kdenoise);
double IMTFilterDeNoiseDiff (IMTpixel** p_im, unsigned height, unsigned width, unsigned radius, double kdenoise);
void IMTBlurMask (IMTpixel** p_im, BYTE** m_im, unsigned height, unsigned width, int radius);
void IMTReduceBW (BYTE** m_im, BYTE** g_im, unsigned height, unsigned width, unsigned heightg, unsigned widthg, unsigned kred, BYTE preval, BYTE result);
IMTpixel IMTFilterIllumCorr (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width);
double IMTFilterFindSkew (IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterRotate (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double angle);
IMTinfo IMTFilterInfo (IMTpixel** p_im, unsigned height, unsigned width, unsigned bpp);
void IMTFilterInvert (IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterCopy (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width);
void IMTFilterAdSmooth (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius);
void IMTFilterInpaint (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, unsigned value);
void IMTFilterSeparate (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, unsigned value);
void IMTFilterSeparateBGFGL (IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, double doverlay);
void IMTFilterSeparateDelta (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, int value, double kdelta);
IMTpixel IMTFilterGreyWorld (IMTpixel** p_im, unsigned height, unsigned width);
IMTpixel IMTFilterGreyNorm (IMTpixel** p_im, unsigned height, unsigned width);
double IMTFilterLevelMean (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double contour, double thres, int lower_bound, int upper_bound);
double IMTFilterLevelSigma (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double thres, double fpart);
void IMTFilterMathAverage (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathDistance (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathDivide (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathGeometric (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathHarmonic (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathMax (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathMin (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathMinus (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathMirror (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathMultiply (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathNorm (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
void IMTFilterMathPlus (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta);
double IMTFilterMathSharpenBadMetric (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width);
void IMTFilterMathThreshold (IMTpixel** p_im, IMTpixel** m_im, BYTE** d_im, unsigned height, unsigned width, int delta);
double IMTFilterMirror (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width);
double IMTFilterMirrorHalf (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width);
void IMTFilterMirrorMean (IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterMonoColor (IMTpixel** p_im, unsigned height, unsigned width);
void IMTFilterMorph (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, bool fdilate);
IMTpixel IMTmeanIcM (IMTpixel** IMTim, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanPtIcM (IMTpixel** IMTim, IMTpixel IMTimm, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
double IMTwbIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanThIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx, int tflag);
IMTpixel IMTmeanRadIcM (IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanBlIcM (IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanMinIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
IMTpixel IMTmeanMaxIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx);
int IMTFilterKMeans (IMTpixel** IMTim, unsigned height, unsigned width, unsigned ncluster, unsigned iters);
void IMTFilterPeron (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, double noise);
double IMTFilterPosterize (IMTpixel** p_im, unsigned height, unsigned width, unsigned thres);
void IMTFilterPMean (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int fmode, bool fneared);
int IMTFilterRetinex (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double sigma);
double IMTFilterRS (IMTpixel** p_im, unsigned height, unsigned width);
void IMTSelGaussInitMatrix (double radius, double *mat, int num);
void IMTFilterSelGauss (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int maxdelta);
double IMTFilterShrink (IMTpixel** p_im, unsigned height, unsigned width, int thres);
void IMTFilterSobel (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width);
void IMTFilterUnRipple (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double thres);
void IMTGaussLineMatrix (double *cmatrix, double radius);
void IMTFilterGaussBlur (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius);
int IMTFilterClusterBiModC (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned ncolor);
double IMTFilterClusterBWC (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned radius);
void IMTFilterUnsharpMask (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width, double amount, int threshold);
double IMTFilterNoiseVariance (IMTpixel** p_im, unsigned height, unsigned width, int radius);
void IMTFilterWiener (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double noise);
int IMTFilterWhiteFill (IMTpixel** p_im, unsigned height, unsigned width);
double BiCubicKernel (double x);
void IMTFilterSBicub (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
void IMTFilterSBilin (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
int IMTFilterSBWMag2 (BYTE** d_im, BYTE** r_im, unsigned height, unsigned width, unsigned height2, unsigned width2);
int IMTFilterSBWReduce2 (BYTE** d_im, BYTE** r_im, unsigned height, unsigned width, unsigned height2, unsigned width2);
void IMTFilterSGsample (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
void IMTFilterSHRIS (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int smode);
void IMTFilterSReduce (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int smode);
void IMTFilterSNearest (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width);
int IMTFilterThreshold (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int threshold);
int IMTFilterThresholdLayer (IMTpixel** p_im, WORD** t_im, BYTE** d_im, unsigned height, unsigned width);
void IMTFilterTLayerToImg (WORD** t_im, IMTpixel** d_im, unsigned height, unsigned width);
int IMTFilterTAbutaleb (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius);
int IMTFilterTBernsenLayer(IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, unsigned contrast_limit);
int IMTFilterTBernsen (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, unsigned contrast_limit);
int IMTFilterTBHTValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTBHT (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTBiModValueBound (IMTpixel** p_im, unsigned height, unsigned width, int lower_bound, int upper_bound);
int IMTFilterTBiModValueIc (IMTpixel** p_im, unsigned y0, unsigned x0, unsigned y1, unsigned x1);
int IMTFilterTBiModValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTBiMod (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta);
int IMTFilterTBiModC (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta);
int IMTFilterTBiModLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int delta);
int IMTFilterTBiModRegion (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int delta);
int IMTFilterTColor (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta);
int IMTFilterTChistianLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta);
int IMTFilterTChistian (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta);
int IMTFilterTDalg (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int region_size, int delta);
int IMTFilterTDither (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTDjVuL (IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, int wbmode, double anisotropic, double doverlay, unsigned fposter);
int IMTFilterTEntValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTEnt (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTEqBrightValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTEqBright (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterGatosBG (IMTpixel** p_im, BYTE** d_im, IMTpixel** bg_im, unsigned height, unsigned width, int radius);
int IMTFilterTGatos (IMTpixel** p_im, BYTE** d_im, IMTpixel** bg_im, BYTE** g_im, unsigned height, unsigned width, double q, double p1, double p2);
int IMTFilterTGradValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTGrad (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTHalftone2 (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTJanniValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTJanni (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTKMeans (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned knum, unsigned iters);
int IMTFilterTMscaleLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, unsigned radius, double sensitivity, double doverlay, double delta);
int IMTFilterTMscale (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned radius, double sensitivity, double doverlay, double delta);
int IMTFilterTNiblackLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta);
int IMTFilterTNiblack (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta);
int IMTFilterTOtsuValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTOtsu (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width);
int IMTFilterTQuadModValue (IMTpixel** p_im, unsigned height, unsigned width);
int IMTFilterTQuadMod (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta);
int IMTFilterTRotValue (IMTpixel** p_im, unsigned height, unsigned width, bool weight);
int IMTFilterTRot (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, bool weight);
int IMTFilterTSauvolaLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int dynamic_range, int lower_bound, int upper_bound, double delta);
int IMTFilterTSauvola (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int dynamic_range, int lower_bound, int upper_bound, double delta);
int IMTFilterTText (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned contour, unsigned radius);
int IMTFilterTTsaiValue (IMTpixel** p_im, unsigned height, unsigned width, int shift);
int IMTFilterTTsai (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int shift);
int IMTFilterTWhiteRohrer (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int x_lookahead, int y_lookahead, int bias_mode, int bias_factor, int f_factor, int g_factor);

#endif // IMTHRESHOLD_H
