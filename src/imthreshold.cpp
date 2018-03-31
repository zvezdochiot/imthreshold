//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "imthreshold.h"

double RADGRD = 57.295779513082320876798154814105;

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTset (BYTE c0, BYTE c1, BYTE c2)
{
    IMTpixel im;

    im.c[0] = (BYTE)c0;
    im.c[1] = (BYTE)c1;
    im.c[2] = (BYTE)c2;
    im.s = (WORD)c0 + (WORD)c1 + (WORD)c2;

    return im;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTcalcS (IMTpixel im)
{
    unsigned ims, d;

    ims = 0;
    for (d = 0; d < 3; d++)
    {
        ims += (int)im.c[d];
    }
    im.s = ims;

    return im;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTdiffS (IMTpixel im)
{
    unsigned immax, immin, d;

    immax = im.c[0];
    immin = im.c[0];
    for (d = 1; d < 3; d++)
    {
        if (im.c[d] > immax) {immax = im.c[d];}
        if (im.c[d] < immin) {immin = im.c[d];}
    }
    im.s = (immax - immin) * 3;

    return im;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTrefilter1p (IMTpixel IMTim, IMTpixel IMTimf)
{
    unsigned d;
    int im, imf, imd;
    IMTpixel immt;

    for (d = 0; d < 3; d++)
    {
        im = IMTim.c[d];
        imf = IMTimf.c[d];
        imd = 2 * im - imf;
        immt.c[d] = (BYTE)MIN(MAX((int)0, (int)(imd)), (int)255);
    }
    immt = IMTcalcS (immt);

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTinterpolation (IMTpixel** p_im, unsigned height, unsigned width, double y, double x)
{
    unsigned d, y1, x1, y2, x2;
    double p11, p21, p12, p22, ky, kx, k11, k21, k12, k22, t;
    IMTpixel res;

    y1 = int(y);
    x1 = int(x);
    y2 = y1 + 1;
    x2 = x1 + 1;
    if (y1 < 0) {y1 = 0;}
    if (x1 < 0) {x1 = 0;}
    if (y1 >= height - 1) {y1 = height - 1;}
    if (x1 >= width - 1) {x1 = width - 1;}
    if (y2 < 0) {y2 = 0;}
    if (x2 < 0) {x2 = 0;}
    if (y2 >= height - 1) {y2 = height - 1;}
    if (x2 >= width - 1) {x2 = width - 1;}
    ky = y - y1;
    if (ky < 0) {ky = 0.0;}
    if (ky > 1) {ky = 1.0;}
    kx = x - x1;
    if (kx < 0) {kx = 0.0;}
    if (kx > 1) {kx = 1.0;}
    k11 = (1.0 - ky) * (1.0 - kx);
    k21 = ky * (1.0 - kx);
    k12 = (1.0 - ky) * kx;
    k22 = ky * kx;
    for (d = 0; d < 3; d++)
    {
        p11 = p_im[y1][x1].c[d];
        p21 = p_im[y2][x1].c[d];
        p12 = p_im[y1][x2].c[d];
        p22 = p_im[y2][x2].c[d];
        t = p11 * k11 + p21 * k21 + p12 * k12 + p22 * k22;
        res.c[d] = (BYTE)t;
    }
    res = IMTcalcS (res);

    return res;
}

////////////////////////////////////////////////////////////////////////////////

BYTE IMTmax (IMTpixel** IMTim, unsigned height, unsigned width)
{
    unsigned x, y, d;
    BYTE im, immax = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                im = IMTim[y][x].c[d];
                if (im > immax) {immax = im;}
            }
        }
    }

    return immax;
}

////////////////////////////////////////////////////////////////////////////////

BYTE IMTmin (IMTpixel** IMTim, unsigned height, unsigned width)
{
    unsigned x, y, d;
    BYTE im, immin = 255;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                im = IMTim[y][x].c[d];
                if (im < immin) {immin = im;}
            }
        }
    }

    return immin;
}

////////////////////////////////////////////////////////////////////////////////

double IMTmean (IMTpixel** IMTim, unsigned height, unsigned width)
{
    unsigned x, y;
    double imx, immean = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (double)IMTim[y][x].s;
            immean += imx;
        }
    }
    immean /= height;
    immean /= width;
    immean /= 3.0;

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

double IMTdev (IMTpixel** IMTim, double immean, unsigned height, unsigned width)
{
    unsigned x, y;
    double imx, imd, imdev = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (double)IMTim[y][x].s / 3.0;
            imd = imx - immean;
            imdev += (imd * imd);
        }
    }
    imdev /= height;
    imdev /= width;
    if (imdev < 0) {imdev = -imdev;}
    imdev = sqrt(imdev);

    return imdev;
}

////////////////////////////////////////////////////////////////////////////////

double IMTwb (IMTpixel** IMTim, double immean, unsigned height, unsigned width)
{
    unsigned x, y;
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
    imwb /= height;
    imwb /= width;

    return imwb;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSMirror (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, ims;
    IMTpixel pim, mim;

    mim = IMTmeanIc(p_im, 0, 0, height, width);
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            pim = p_im[y][x];
            ims = IMTdist(pim, mim);
            p_im[y][x].s = (WORD)ims;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterSNorm (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, im, immin, immax, imd, threshold;

    immin = 765;
    immax = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            if (im < immin) {immin = im;}
            if (im > immax) {immax = im;}
        }
    }
    imd = immax - immin;
    if (imd > 0)
    {
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                im = p_im[y][x].s;
                im -= immin;
                im *= 765;
                im /= imd;
                p_im[y][x].s = (WORD)im;
            }
        }
    }
    threshold = immax + immin;
    threshold /= 6;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSCompare (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width)
{
    unsigned y, x;
    int ims, imsd;
    IMTpixel pim, bim;

    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            pim = p_im[y][x];
            bim = b_im[y][x];
            ims = pim.s;
            ims -= 384;
            imsd = 765 + IMTdist(pim, bim);
            ims *= imsd;
            ims /= 765;
            ims *= imsd;
            ims /= 765;
            ims += 384;
            if (ims < 0) {ims = 0;}
            if (ims > 765) {ims = 765;}
            p_im[y][x].s = (WORD)ims;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSEdge (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width)
{
    unsigned y, x;
    int ims, imsd;
    IMTpixel pim, bim;

    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            pim = p_im[y][x];
            bim = b_im[y][x];
            ims = pim.s;
            ims -= bim.s;
            imsd = IMTdist(pim, bim);
            if (ims < 0) {ims = 384 - imsd;} else {ims = 384 + imsd;}
            if (ims < 0) {ims = 0;}
            if (ims > 765) {ims = 765;}
            p_im[y][x].s = (WORD)ims;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterInvertBW (BYTE** p_im, unsigned height, unsigned width)
{
    unsigned y, x;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            p_im[y][x] = 255 - p_im[y][x];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/*
double IMTdist (IMTpixel IMTim0, IMTpixel IMTim1)
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
*/
////////////////////////////////////////////////////////////////////////////////

unsigned IMTdist (IMTpixel IMTim0, IMTpixel IMTim1)
{
    unsigned d, imds = 0;
    int imd;

    for (d = 0; d < 3; d++)
    {
        imd = (int)IMTim0.c[d];
        imd -= (int)IMTim1.c[d];
        if (imd < 0) {imd = -imd;}
        imds += imd;
    }

    return imds;
}

////////////////////////////////////////////////////////////////////////////////

double IMTdist3c2p(IMTpixel IMTim, double* IMTimc)
{
    unsigned d;
    double imd, imds = 0;

    for (d = 0; d < 3; d++)
    {
        imd = (double)IMTim.c[d];
        imd -= (double)IMTimc[d];
        imd /= 255;
        imd *= imd;
        imds += imd;
    }
    imds /= 3;

    return sqrt(imds);
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
    unsigned y, x, d;
    unsigned imm, n = 0;
    double ims;
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
    if (n > 0)
    {
        ims /= n;
        if (ims > 765) {ims = 765;}
    }
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
        if (n > 0)
        {
            ims /= n;
            if (ims > 255) {ims = 255;}
        }
        immean.c[d] = (BYTE)ims;
    }
    immean = IMTcalcS (immean);

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmaxIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
    unsigned y, x;
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

IMTpixel IMTminIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
    unsigned y, x;
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

IMTpixel IMTaverageIc (IMTpixel** IMTim, IMTpixel IMTima, unsigned y0, unsigned x0, unsigned y1, unsigned x1, double part)
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
    immean = IMTcalcS (immean);

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterDespeck2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    int k2 = Ksize/2;
    int kmax = Ksize - k2;
    int i, j, y, x, y1, x1;
    unsigned n, val;
    int h = height;
    int w = width;
    BYTE** d_im;
    d_im = (BYTE**)malloc(height * sizeof(BYTE*));
    for (y = 0; y < h; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            val = 0;
            n = 0;
            for(i = -k2; i < kmax; i++)
            {
                for(j = -k2; j < kmax; j++)
                {
                    y1 = y + i;
                    x1 = x + j;
                    if ((y1 >= 0) && (x1 >= 0) && (y1 < h) && (x1 < w))
                    {
                        n++;
                        if (p_im[y1][x1] > 0) {val++;}
                    }
                }
            }
            val *= 2;
            if (val == n)
            {
                val = p_im[y][x];
            } else if (val > n) {
                val = 255;
            } else {
                val = 0;
            }
            d_im[y][x] = (BYTE)val;
        }
    }
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            p_im[y][x] = d_im[y][x];
        }
    }

    for (y = 0; y < h; y++){free(d_im[y]);}
    free(d_im);
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterDNeuro2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize, double lambda, unsigned lnum)
{
    int i, j, y, x, y1, x1;
    unsigned n, l;
    int val;
    int h = height;
    int w = width;
    int k = Ksize;
    int k2 = k / 2;
    double z, s, er, sigma, dw;
    double** weight;
    weight = (double**)malloc(Ksize * sizeof(double*));
    for (y = 0; y < k; y++) {weight[y] = (double*)malloc(Ksize * sizeof(double));}
    BYTE** d_im;
    d_im = (BYTE**)malloc(height * sizeof(BYTE*));
    for (y = 0; y < h; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

    if (lnum < 1) {lnum = 1;}
    n = 0;
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < k; j++)
        {
            n++;
            weight[i][j] = n;
        }
    }
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < k; j++)
        {
            weight[i][j] /= n;
            weight[i][j] += 1;
        }
    }
    for (l = 0; l < lnum; l++)
    {
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                s = 0;
                for(i = 0; i < k; i++)
                {
                    for(j = 0; j < k; j++)
                    {
                        y1 = y + i - k2;
                        x1 = x + j - k2;
                        if ((y1 >= 0) && (x1 >= 0) && (y1 < h) && (x1 < w) && y1 != y && x1 != x)
                        {
                            if (p_im[y1][x1] > 0)
                            {
                                s += weight[i][j];
                            } else {
                                s -= weight[i][j];
                            }
                        }
                    }
                }
                z = 2.0 / (1.0 + exp(-s)) - 1;
                if (p_im[y][x] > 0) {er = 1.0;} else {er = -1.0;}
                er -= z;
                sigma = er * z * (1 - z);
                dw = lambda * sigma;
                for(i = 0; i < k; i++)
                {
                    for(j = 0; j < k; j++)
                    {
                        y1 = y + i - k2;
                        x1 = x + j - k2;
                        if ((y1 >= 0) && (x1 >= 0) && (y1 < h) && (x1 < w) && y1 != y && x1 != x)
                        {
                            weight[i][j] += dw;
                        }
                    }
                }
            }
        }
    }
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            s = 0;
            for(i = 0; i < k; i++)
            {
                for(j = 0; j < k; j++)
                {
                    y1 = y + i - k2;
                    x1 = x + j - k2;
                    if ((y1 >= 0) && (x1 >= 0) && (y1 < h) && (x1 < w) && y1 != y && x1 != x)
                    {
                        if (p_im[y1][x1] > 0)
                        {
                            s += weight[i][j];
                        } else {
                            s -= weight[i][j];
                        }
                    }
                }
            }
            z = 2.0 / (1.0 + exp(-s)) - 1;
            if (z > 0)
            {
                val = 0;
            } else {
                val = 255;
            }
            d_im[y][x] = (BYTE)val;
        }
    }
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            p_im[y][x] = d_im[y][x];
        }
    }

    for (y = 0; y < k; y++){free(weight[y]);}
    free(weight);
    for (y = 0; y < h; y++){free(d_im[y]);}
    free(d_im);
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterDphist (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    unsigned y, x, f, g, i, j, k, l, m, n, val, hmin, imin, hmax, imax, nrep, nreps;
    unsigned hist[512] = {0};
    double kdp;

    int** d_im;
    d_im = (int**)malloc(height * sizeof(BYTE*));
    for (y = 0; y < height; y++) {d_im[y] = (int*)malloc(width * sizeof(int));}

    n = width * height;
    for (k = 0; k < 512; k++)
    {
        hist[k] = 0;
    }
    for (y = 0; y < height - 2; y++)
    {
        for (x = 0; x < width - 2; x++)
        {
            val = 0;
            k = 0;
            for(i = 0; i < 3; i++)
            {
                for(j = 0; j < 3; j++)
                {
                    if (p_im[y + i][x + j] > 0)
                    {
                        l = (1 << k);
                        val += l;
                    }
                    k++;
                }
            }
            d_im[y][x] = val;
            hist[val]++;
        }
    }
    imin = 0;
    imax = 1;
    nreps = 0;
    while (imin != imax)
    {
        hmin = n;
        imin = 0;
        for (k = 0; k < 512; k++)
        {
            if (hist[k] > 0 && hist[k] < hmin)
            {
                hmin = hist[k];
                imin = k;
            }
        }
        hmax = hmin;
        imax = imin;
        for (k = 0; k < 9; k++)
        {
            i = (1 << k);
            m = imin | i;
            if (hist[m] > hmax)
            {
                hmax = hist[m];
                imax = m;
            }
        }
        if (imin == imax)
        {
            if (Ksize > 1)
            {
                for (k = 0; k < 9; k++)
                {
                    i = (1 << k);
                    for (l = 0; l < 9; l++)
                    {
                        if (k != l)
                        {
                            j = (1 << l);
                            m = imin | (i + j);
                            if (hist[m] > hmax)
                            {
                                hmax = hist[m];
                                imax = m;
                            }
                        }
                    }
                }
                if (imin == imax)
                {
                    if (Ksize > 2)
                    {
                        for (k = 0; k < 9; k++)
                        {
                            i = (1 << k);
                            for (l = 0; l < 9; l++)
                            {
                                if (k != l)
                                {
                                    j = (1 << l);
                                    for (f = 0; f < 9; f++)
                                    {
                                        if (f != l && f != k)
                                        {
                                            g = (1 << f);
                                            m = imin | (i + j + g);
                                            if (hist[m] > hmax)
                                            {
                                                hmax = hist[m];
                                                imax = m;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        nrep = 0;
        if (imin != imax)
        {
            for (y = 0; y < height - 2; y++)
            {
                for (x = 0; x < width - 2; x++)
                {
                    val = d_im[y][x];
                    if (val == imin)
                    {
                        k = 0;
                        for(i = 0; i < 3; i++)
                        {
                            for(j = 0; j < 3; j++)
                            {
                                p_im[y + i][x + j] = (BYTE) (((imax >> k) & 1) ? 255 : 0 );
                                k++;
                            }
                        }
                        nrep++;
                        d_im[y][x] = imax;
                        hist[imax]++;
                    }
                }
            }
            hist[imin] = 0;
            nreps += nrep;
        }
    }
    kdp = nreps;
    kdp /= n;

    for (y = 0; y < height; y++){free(d_im[y]);}
    free(d_im);

    return kdp;
}

////////////////////////////////////////////////////////////////////////////////

void IMTBlurMask (IMTpixel** p_im, BYTE** m_im, unsigned height, unsigned width, int radius)
{
    int y, x, y0, x0;
    unsigned y1, x1, i, j, c0, c1, c2, n, k, l;
    int h = height;
    int w = width;
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            if (m_im[y][x] == 0)
            {
                y0 = y - radius;
                if (y0 < 0) {y0 = 0;}
                y1 = y + radius;
                if (y1 > height) {y1 = height;}
                x0 = x - radius;
                if (x0 < 0) {x0 = 0;}
                x1 = x + radius;
                if (x1 > width) {x1 = width;}
                c0 = 0;
                c1 = 0;
                c2 = 0;
                n = 0;
                for (j = y0; j < y1; j++)
                {
                    for (i = x0; i < x1; i++)
                    {
                        k = 1;
                        if (m_im[j][i] > 0)
                        {
                            k = radius + 1;
                        }
                        for (l = 0; l < k; l++)
                        {
                            c0 += p_im[j][i].c[0];
                            c1 += p_im[j][i].c[1];
                            c2 += p_im[j][i].c[2];
                            n++;
                        }
                    }
                }
                if (n > 0)
                {
                    p_im[y][x].c[0] = (BYTE)(c0 / n);
                    p_im[y][x].c[1] = (BYTE)(c1 / n);
                    p_im[y][x].c[2] = (BYTE)(c2 / n);
                }
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTReduceBW (BYTE** m_im, BYTE** g_im, unsigned height, unsigned width, unsigned heightg, unsigned widthg, unsigned kred, BYTE preval, BYTE result)
{
    unsigned y, x, y0, x0, y1, x1, i, j;
    BYTE val;
    for (y = 0; y < heightg; y++)
    {
        y0 = y * kred;
        y1 = y0 + kred;
        if (y1 > height) {y1 = height;}
        for (x = 0; x < widthg; x++)
        {
            x0 = x * kred;
            x1 = x0 + kred;
            if (x1 > width) {x1 = width;}
            val = preval;
            for (i = y0; i < y1; i++)
            {
                for (j = x0; j < x1; j++)
                {
                    if (m_im[i][j] == 0) {val = result;}
                }
            }
            g_im[y][x] = val;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTFilterIllumCorr (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned y, x, d, im, imb;
    double res;
    int i, k;
    int max;
    int max_pos[3];
    IMTpixel mean; // dominant color
    BYTE val;
    long palette[768] = {0};

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                im = b_im[y][x].c[d];
                im += (d * 256);
                palette[im]++;
            }
        }
    }

    // finding the dominant color
    k = 0;
    for (d = 0; d < 3; d++)
    {
        max = 0;
        for (i = 0; i < 256; i++)
        {
            if (palette[k] > max)
            {
                max = palette[k];
                max_pos[d] = k;
            }
            k++;
        }
    }
    mean.s = 0;
    for (d = 0; d < 3; d++)
    {
        val = max_pos[d]; // dominant color
        mean.c[d] = val;
        mean.s += val;
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            d_im[y][x].s = 0;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                im++;
                imb = b_im[y][x].c[d];
                imb++;
                res = (double)im;
                res /= imb;
                res *= mean.c[d];
                res += im;
                res /= 2;
                val = (BYTE)MIN(MAX((int)0, (int)(res + 0.5)), (int)255);
                d_im[y][x].c[d] = val;
                d_im[y][x].s += val;
            }
        }
    }
    return mean;
}

/////////////////////////////////////////////////////////////////////////////////////////////

double IMTFilterFindSkew (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x;
    int t, u;
    int h = height;
    double kmin, kmax, kt, dkp, dkt, yt, smin, smax, st, s;
    double fskew;

    dkp = 1.0 / sqrt(height * height + width * width);
    kmin = -0.5;
    kt = kmin;
    st = 0;
    for (y = 0; y < height; y++)
    {
        s = 0;
        yt = y;
        for (x = 0; x < width; x++)
        {
            t = yt;
            if (t >= 0 && t < h)
            {
                u = p_im[t][x].s;
                u = 765 - u;
                s += u;
            }
            yt += kt;
        }
        if (s > st) {st = s;}
    }
    smin = st;
    kmax = 0.5;
    kt = kmax;
    st = 0;
    for (y = 0; y < height; y++)
    {
        s = 0;
        yt = y;
        for (x = 0; x < width; x++)
        {
            t = yt;
            if (t >= 0 && t < h)
            {
                u = p_im[t][x].s;
                u = 765 - u;
                s += u;
            }
            yt += kt;
        }
        if (s > st) {st = s;}
    }
    smax = st;
    dkt = kmax - kmin;
    while (dkt > dkp)
    {
        kt = (kmax + kmin) / 2.0;
        st = 0;
        for (y = 0; y < height; y++)
        {
            s = 0;
            yt = y;
            for (x = 0; x < width; x++)
            {
                t = yt;
                if (t >= 0 && t < h)
                {
                    u = p_im[t][x].s;
                    u = 765 - u;
                    s += u;
                }
                yt += kt;
            }
            if (s > st) {st = s;}
        }
        if (smax > smin)
        {
            smin = st;
            kmin = kt;
        } else {
            smax = st;
            kmax = kt;
        }
        dkt = kmax - kmin;
    }
    if (smin < smax) {fskew = kmax;} else {fskew = kmin;}
    fskew = -atan(fskew) * RADGRD;
    return fskew;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterRotate (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double angle)
{
    unsigned int y, x;
    double yt, xt, yr, xr, ktc, kts;

    angle /= RADGRD;
    kts = sin(angle);
    ktc = cos(angle);
    for (y = 0; y < height; y++)
    {
        yt = y;
        yt -= height / 2;
        for (x = 0; x < width; x++)
        {
            xt = x;
            xt -= width / 2;
            yr = ktc * yt - kts * xt;
            yr += height / 2;
            xr = ktc * xt + kts * yt;
            xr += width / 2;
            if (yr >= 0 && yr < height && xr >= 0 && xr < width)
            {
                d_im[y][x] = IMTinterpolation(p_im, height, width, yr, xr);
            } else {
                d_im[y][x] = IMTset(255,255,255);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

IMTinfo IMTFilterInfo (IMTpixel** p_im, unsigned height, unsigned width, unsigned bpp)
{
    IMTinfo p_info;

    p_info.height = height;
    p_info.width = width;
    p_info.bpp = bpp;
    p_info.min = 255;
    p_info.max = 0;
    p_info.mean = 0.0;
    p_info.std = 0.0;
    p_info.wb = 0.0;

    p_info.min = IMTmin(p_im, height, width);
    p_info.max = IMTmax(p_im, height, width);
    p_info.mid = (double)(p_info.max + p_info.min) / 510;
    p_info.mean = IMTmean(p_im, height, width);
    p_info.std = IMTdev(p_im, p_info.mean, height, width);
    p_info.wb = IMTwb(p_im, p_info.mean, height, width);
    p_info.mean /= 255;
    p_info.std /= 255;

    return p_info;
 }

////////////////////////////////////////////////////////////////////////////////

void IMTFilterInvert (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned x, y, d, t;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            p_im[y][x].s = 0;
            for (d = 0; d < 3; d++)
            {
                t = 255 - (int)p_im[y][x].c[d];
                p_im[y][x].c[d] = (BYTE)(t);
                p_im[y][x].s += (WORD)(t);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterCopy (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width)
{
    unsigned y, x;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            b_im[y][x] = p_im[y][x];
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void IMTFilterAdSmooth (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius)
{
    int y, x, yf, xf, yfp, xfp, yfn, xfn, i, j, d;
    int h = height;
    int w = width;

    double f = -1.0 / 8.0 / radius / radius;
    double p_gx, p_gy, p_weight;
    double p_weightTotal[3] = {0};
    double p_total[3] = {0};

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            // original formulas for weight calculation:
            // w(x, y) = exp( -1 * (Gx^2 + Gy^2) / (2 * factor^2) )
            // Gx(x, y) = (I(x + 1, y) - I(x - 1, y)) / 2
            // Gy(x, y) = (I(x, y + 1) - I(x, y - 1)) / 2
            //
            // here is a little bit optimized version

            for (i = 0; i < 3; i++)
            {
                p_weightTotal[i] = 0;
                p_total[i] = 0;
            }
            for (i = -1; i <= 1; i++)
            {
                yf = y + i;
                if (yf < 0) {yf = 0;}
                if (yf >= h) {yf = h - 1;}
                yfp = yf - 1;
                if (yfp < 0) {yfp = 0; yfn = yfp + 2;}
                yfn = yf + 1;
                if (yfn >= h) {yfn = h - 1;}
                for (j = -1; j <= -1; j++)
                {
                    xf = x + j;
                    if (xf < 0) {xf = 0;}
                    if (xf >= w) {xf = w - 1;}
                    xfp = xf - 1;
                    if (xfp < 0) {xfp = 0; xfn = xfp + 2;}
                    xfn = xf + 1;
                    if (xfn >= w) {xfn = w - 1;}
                    for (d = 0; d < 3; d++)
                    {
                        p_gx = (double)p_im[yf][xfn].c[d];
                        p_gx -= (double)p_im[yf][xfp].c[d];
                        p_gy = (double)p_im[yfn][xf].c[d];
                        p_gy -= (double)p_im[yfp][xf].c[d];
                        p_weight = exp((p_gx * p_gx + p_gy * p_gy) * f);
                        p_total[d] += (p_weight * (double)p_im[yf][xf].c[d]);
                        p_weightTotal[d] += p_weight;
                    }
                }
            }

            for (d = 0; d < 3; d++)
            {
                if (p_weightTotal[d] == 0.0)
                {
                    d_im[y][x].c[d] = p_im[y][x].c[d];
                } else {
                    d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(p_total[d] / p_weightTotal[d] + 0.5)), (int)255);
                }
                d_im[y][x] = IMTcalcS (d_im[y][x]);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterInpaint (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, unsigned value)
{
    unsigned y, x, d, nb, nn, ns, yp, xp, yn, xn, yf, xf;
    double cs[3];
    BYTE csb[3];

    BYTE** mg_im;
    mg_im = (BYTE**)malloc(height * sizeof(BYTE*));
    for (y = 0; y < height; y++) {mg_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}
    BYTE** md_im;
    md_im = (BYTE**)malloc(height * sizeof(BYTE*));
    for (y = 0; y < height; y++) {md_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

    nb = 0;
    nn = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            mg_im[y][x] = m_im[y][x];
            g_im[y][x] = p_im[y][x];
            if (mg_im[y][x] == value)
            {
                nb++;
            } else {
                nn++;
            }
        }
    }

    if (nb > 0)
    {
        if (nn > 0)
        {
            while (nn > 0)
            {
                nn = 0;
                for (y = 0; y < height; y++)
                {
                    yp = y;
                    if (yp > 0) {yp--;}
                    yn = y + 1;
                    if (yn >= height) {yn = height - 1;}
                    for (x = 0; x < width; x++)
                    {
                        xp = x;
                        if (xp > 0) {xp--;}
                        xn = x + 1;
                        if (xn >= width) {xn = width - 1;}
                        md_im[y][x] = 0;
                        if (mg_im[y][x] != value)
                        {
                            ns = 0;
                            if (mg_im[yp][x] == value || mg_im[yn][x] == value || mg_im[y][xp] == value || mg_im[y][xn] == value)
                            {
                                ns++;
                            }
                            if (ns > 0)
                            {
                                md_im[y][x] = 255;
                            } else {
                                nn++;
                            }
                        }
                    }
                }
                for (y = 0; y < height; y++)
                {
                    yp = y;
                    if (yp > 0) {yp--;}
                    yn = y + 1;
                    if (yn >= height) {yn = height - 1;}
                    for (x = 0; x < width; x++)
                    {
                        xp = x;
                        if (xp > 0) {xp--;}
                        xn = x + 1;
                        if (xn >= width) {xn = width - 1;}
                        if (md_im[y][x] > 0)
                        {
                            ns = 0;
                            for (d = 0; d < 3; d++)
                            {
                                cs[d] = 0;
                            }
                            for (yf = yp; yf <= yn; yf++)
                            {
                                for (xf = xp; xf <= xn; xf++)
                                {
                                    if (mg_im[yf][xf] == value)
                                    {
                                        for (d = 0; d < 3; d++)
                                        {
                                            cs[d] += (int)g_im[yf][xf].c[d];
                                        }
                                        ns++;
                                    }
                                }
                            }
                            if (ns > 0)
                            {
                                for (d = 0; d < 3; d++)
                                {
                                    cs[d] /= ns;
                                    csb[d] = (BYTE)MIN(MAX((int)0, (int)(cs[d] + 0.5)), (int)255);
                                }
                                g_im[y][x] = IMTset(csb[0], csb[1], csb[2]);
                            } else {
                                g_im[y][x] = IMTset(255, 255, 255);
                            }
                            mg_im[y][x] = value;
                        }
                    }
                }
            }
            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {
                    g_im[y][x] = IMTcalcS (g_im[y][x]);
                }
            }
        }
    } else {
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                g_im[y][x] = IMTset(255, 255, 255);
            }
        }
    }

    for (y = 0; y < height; y++){free(md_im[y]);}
    free(md_im);
    for (y = 0; y < height; y++){free(mg_im[y]);}
    free(mg_im);
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterSeparate (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, unsigned value)
{
    unsigned y, x;
    IMTpixel gim;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            gim = p_im[y][x];
            if (m_im[y][x] != value)
            {
                gim = IMTset(255, 255, 255);
            }
            g_im[y][x] = gim;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterSeparateBGFGL (IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, double doverlay)
{
    unsigned widthbg = (width + bgs - 1) / bgs;
    unsigned heightbg = (height + bgs - 1) / bgs;
    unsigned widthfg = (widthbg + fgs - 1) / fgs;
    unsigned heightfg = (heightbg + fgs - 1) / fgs;
    unsigned whcp, y, x, d, l, i, j, y0, x0, y1, x1, y0b, x0b, y1b, x1b, yb, xb, yf, xf, blsz;
    BYTE fgbase, bgbase, mim;
    unsigned cnth, cntw;
    IMTpixel pim, fgim, bgim;
    double fgdist, bgdist, kover, fgpart, bgpart;
    unsigned maskbl, maskover, bgsover, fgsum[3], bgsum[3], fgnum, bgnum;

    IMTpixel** fgt_im;
    fgt_im = (IMTpixel**)malloc(heightbg * sizeof(IMTpixel*));
    for (y = 0; y < heightbg; y++) {fgt_im[y] = (IMTpixel*)malloc(widthbg * sizeof(IMTpixel));}

    whcp = height;
    whcp += width;
    whcp /= 2;
    blsz = 1;
    if (level == 0)
    {
        while (bgs * blsz < whcp)
        {
            level++;
            blsz *= 2;
        }
    } else {
        for (l = 0; l < level; l++)
        {
            blsz *= 2;
        }
    }
    fgbase = 127;
    bgbase = 127;
    for (y = 0; y < heightbg; y++)
    {
        for (x = 0; x < widthbg; x++)
        {
            fgt_im[y][x] = IMTset(fgbase, fgbase, fgbase);
            bg_im[y][x] = IMTset(bgbase, bgbase, bgbase);
        }
    }
    if (doverlay < 0) {doverlay = 0;}
    kover = doverlay + 1.0;
    for (l = 0; l < level; l++)
    {
        cnth = (heightbg + blsz - 1) / blsz;
        cntw = (widthbg + blsz - 1) / blsz;
        maskbl = bgs * blsz;
        maskover = (kover * maskbl);
        bgsover = (kover * blsz);
        for (i = 0; i < cnth; i++)
        {
            y0 = i * maskbl;
            y1 = y0 + maskover;
            if (y1 > height) {y1 = height;}
            y0b = i * blsz;
            y1b = y0b + bgsover;
            if (y1b > heightbg) {y1b = heightbg;}
            for (j = 0; j < cntw; j++)
            {
                x0 = j * maskbl;
                x1 = x0 + maskover;
                if (x1 > width) {x1 = width;}
                x0b = j * blsz;
                x1b = x0b + bgsover;
                if (x1b > widthbg) {x1b = widthbg;}

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
                        mim = m_im[y][x];
                        if (mim == 0)
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
                if (fgnum > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        fgsum[d] /= fgnum;
                        fgim.c[d] = fgsum[d];
                    }
                }
                if (bgnum > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        bgsum[d] /= bgnum;
                        bgim.c[d] = bgsum[d];
                    }
                }
                pim = IMTmeanIc(p_im, y0, x0, y1, x1);
                fgdist = IMTdist(pim, fgim);
                bgdist = IMTdist(pim, bgim);
                fgpart = 1.0;
                bgpart = 1.0;
                if ((fgdist + bgdist)> 0)
                {
                    fgpart += (2.0 * fgdist / (fgdist + bgdist));
                    bgpart += (2.0 * bgdist / (fgdist + bgdist));
                }
                fgim = IMTaverageIc(fgt_im, fgim, y0b, x0b, y1b, x1b, fgpart);
                bgim = IMTaverageIc(bg_im, bgim, y0b, x0b, y1b, x1b, bgpart);
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

///////////////////////////////////////////////////////////////////////////////

void IMTFilterSeparateDelta (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, int value, double kdelta)
{
    unsigned y, x, d;
    int im, img, imd;
    double simd, simp;
    IMTpixel gim;

    simd = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = m_im[y][x];
            for (d = 0; d < 3; d++)
            {
                img = p_im[y][x].c[d];
                imd = im - img;
                if (imd < 0) {imd = -imd;}
                simd += imd;
            }
        }
    }
    simd /= width;
    simd /= height;
    simd /= 3;
    simd *= value;
    simd *= kdelta;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = m_im[y][x];
            simp = 0;
            for (d = 0; d < 3; d++)
            {
                img = p_im[y][x].c[d];
                imd = im - img;
                if (imd < 0) {imd = -imd;}
                simp += imd;
            }
            simp /= 3;
            simp *= value;
            gim = p_im[y][x];
            if (simp > simd)
            {
                gim = IMTset(im, im, im);
            }
            g_im[y][x] = gim;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

IMTpixel IMTFilterGreyNorm (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, d, n;
    int im;
    double di, dist, sd, mins, maxs, means, ds, ming, maxg, meang, dg;
    IMTpixel imd;
    double pc[3];
    double mc[3];
    double sc[3];
    double ac[3];
    BYTE grey;

    n = height * width;
    pc[0]=0.227; pc[1]=0.453; pc[2]=0.320;
    mins = 768.0;
    maxs = 0.0;
    ming = 768.0;
    maxg = 0.0;
    for (d = 0; d < 3; d++)
    {
        mc[d] = 0;
        sc[d] = 0;
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                mc[d] += double(im);
            }
            im = p_im[y][x].s;
            di = im;
            if (di < mins) {mins = di;}
            if (di > maxs) {maxs = di;}
        }
    }
    mins /= 3;
    maxs /= 3;
    means = (maxs+mins)/2;
    ds = maxs - mins;
    if (ds == 0.0)
    {
        ds = means;
        if (ds > 127.5) {ds = 255.0 - ds;}
    }
    for (d = 0; d < 3; d++)
    {
        mc[d] /= n;
    }
    sd = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dist = 0;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                di = im;
                di -= mc[d];
                di *= pc[d];
                if (di < 0) {di = -di;}
                sc[d] += di;
                di *= di;
                dist += di;
            }
            dist = sqrt(dist);
            sd += dist;
        }
    }
    sd /= n;
    for (d = 0; d < 3; d++)
    {
        sc[d] /= n;
    }
    if (sd != 0)
    {
        for (d = 0; d < 3; d++)
        {
            ac[d] = sc[d] / sd;
        }
    } else {
        for (d = 0; d < 3; d++)
        {
            ac[d] = 1.0;
        }
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dist = 0;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                di = im;
                di -= mc[d];
                di *= pc[d];
                di *= ac[d];
                dist += di;
            }
            if (dist < ming) {ming = dist;}
            if (dist > maxg) {maxg = dist;}
        }
    }
    meang = (maxg + ming) / 2;
    dg = maxg - ming;
    if (dg == 0) {dg = 1;}
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dist = 0;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                di = im;
                di -= mc[d];
                di *= pc[d];
                di *= ac[d];
                dist += di;
            }
            dist -= meang;
            dist /= dg;
            dist *= ds;
            dist += means;
            grey = (BYTE)MIN(MAX((int)0, (int)(dist)), (int)255);
            for (d = 0; d < 3; d++)
            {
                p_im[y][x].c[d] = grey;
            }
        }
    }
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }

    for (d = 0; d < 3; d++)
    {
        imd.c[d] = (BYTE)MIN(MAX((int)0, (int)(ac[d] * 255.0)), (int)255);
    }
    imd.s = means;

    return imd;
 }

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTFilterGreyWorld (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, d;
    int im;
    double si, st, kt, vt;
    IMTpixel imd;

    si = 0;
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            im = p_im[y][x].s;
            si += double(im);
        }
    }
    si /= 3;
    for (d = 0; d < 3; d++)
    {
        st=0;
        for ( y = 0; y < height; y++ )
        {
            for ( x = 0; x < width; x++ )
            {
                im = p_im[y][x].c[d];
                st += double(im);
            }
        }
        if (st == 0.0)
        {
            kt = 1;
        } else {
            kt = si/st;
        }
        imd.c[d] = (BYTE)(255 * kt / 2);
        for ( y = 0; y < height; y++ )
        {
            for ( x = 0; x < width; x++ )
            {
                im = p_im[y][x].c[d];
                vt = (double)(im);
                vt *= kt;
                p_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(vt + 0.5)), (int)255);
            }
        }
    }
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }

    return imd;
 }

////////////////////////////////////////////////////////////////////////////////

double IMTFilterLevelL (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned level, unsigned num)
{
    unsigned y, x, d, l, i, j, y0, x0, y1, x1;
    unsigned blsz = 1, cnth, cntw;
    double immean, val;
    IMTpixel pim, dim;

    for (l = 0; l < level; l++)
    {
        blsz *= 2;
    }
    pim = IMTmeanIc(p_im, 0, 0, height, width);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            d_im[y][x] = pim;
        }
    }
    if (num > level) {num = 0;}
    level -= num;
    for (l = 0; l <= level; l++)
    {
        cnth = (height + blsz - 1) / blsz;
        cntw = (width + blsz - 1) / blsz;
        for (i = 0; i < cnth; i++)
        {
            y0 = i * blsz;
            y1 = y0 + blsz;
            if (y1 > height) {y1 = height;}
            for (j = 0; j < cntw; j++)
            {
                x0 = j * blsz;
                x1 = x0 + blsz;
                if (x1 > width) {x1 = width;}

                pim = IMTmeanIc(p_im, y0, x0, y1, x1);
                dim = IMTaverageIc(d_im, pim, y0, x0, y1, x1, level);
            }
        }
        blsz /= 2;
    }
    immean = IMTmean(d_im, height, width);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                val = p_im[y][x].c[d];
                val += immean;
                val -= d_im[y][x].c[d];
                d_im[y][x].c[d] = MIN(MAX((int)0, (int)val), (int)255);;
            }
        }
    }

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterLevelMean (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double contour, double thres, int lower_bound, int upper_bound)
{
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }
    if (contour < 0)
    {
        if ((upper_bound - lower_bound + 1) > 0) {contour = 256.0 / (upper_bound - lower_bound + 1);} else {contour = 1.0;}
    }
    if (radius < 0) {radius = -radius;}
    if (thres < 0) {thres = -thres;}

    int x, y, i, j, xf, yf, t;
    unsigned d, n;
    int im, mean;
    double imx, val, km;
    int h = height;
    int w = width;
    int d_bound = upper_bound - lower_bound;

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            mean = 0;
            n = 0;
            for (i = -radius; i<= radius; i++)
            {
                yf = y + i;
                if (yf >= 0 && yf < h)
                {
                    for (j = -radius; j <= radius; j++)
                    {
                        xf = x + j;
                        if (xf >= 0 && xf < w)
                        {
                            mean += p_im[yf][xf].s;
                            n++;
                        }
                    }
                }
            }
            mean /= n;
            im = p_im[y][x].s;
            km = 1;
            if (im > 0)
            {
                t = im - mean;
                if (t > -thres && t < thres) {t = 0;}
                t *= contour;
                imx = (double)mean / 3;
                imx -= lower_bound;
                imx *= 255;
                imx /= d_bound;
                if (imx < 0) {imx = 0;}
                if (imx > 255) {imx = 255;}
                imx *= 3;
                imx += t;
                km = (double)imx / im;
            }
            for (d = 0; d < 3; d++)
            {
                val = p_im[y][x].c[d];
                val *= km;
                if (val < 0) {val = 0;}
                if (val > 255) {val = 255;}
                d_im[y][x].c[d] = (BYTE)(val + 0.5);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }

    return contour;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterLevelSigma (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double thres, double fpart)
{
    unsigned y, x, d;
    int im;
    double imx, tx, apart, k, ks;

    if (thres < 0) {thres = -thres;}
    if (thres > 1) {thres = 1;}
    if (fpart < -1) {fpart = -1;}
    if (fpart > 1) {fpart = 1;}
    apart = fpart;
    if (apart < 0) {apart = -apart;}

    ks = 0;
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            im = p_im[y][x].s;
            imx = im;
            imx /= 765;
            if (fpart > 0)
            {
                if (imx < thres)
                {
                    tx = thres;
                    imx = tx - sqrt(tx * tx - imx * imx);
                } else {
                    tx = 1 - thres;
                    imx = 1 - imx;
                    imx = 1 - tx + sqrt(tx * tx - imx * imx);
                }
            } else {
                if (imx < thres)
                {
                    tx = thres;
                    imx -= thres;
                    imx = sqrt(tx * tx - imx * imx);
                } else {
                    tx = 1 - thres;
                    imx -= thres;
                    imx = 1 - sqrt(tx * tx - imx * imx);
                }
            }
            imx *= 765;
            k = imx + 1;
            k /= (im + 1);
            k -= 1.0;
            k *= apart;
            k += 1.0;
            ks += k;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                imx = im;
                imx *= k;
                d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(imx + 0.5)), (int)255);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    ks /= height;
    ks /= width;

    return ks;
}

///////////////////////////////////////////////////////////////////////////////

double IMTFilterMirror (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned y, x, d;
    int im, imm, imd;
    double ims = 0.0;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                imm = d_im[y][x].c[d];
                imd = im - imm;
                im += imd;
                ims += imd;
                d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)im), (int)255);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    ims /= 3;
    ims /= width;
    ims /= height;

    return ims;
 }

///////////////////////////////////////////////////////////////////////////////

double IMTFilterMirrorHalf (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned y, x, d;
    int im, imm, imd, ime = 0, imf;
    double ims = 0.0;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                imm = d_im[y][x].c[d];
                imd = im - imm;
                imf = (imd + ime) / 2;
                ime += (imd - imf * 2);
                im += imf;
                ims += imf;
                d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)im), (int)255);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    ims /= 3;
    ims /= width;
    ims /= height;

    return ims;
 }

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMirrorMean (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, d;
    int im, imm;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                imm = im - 128;
                if (imm < 0) {imm += 255;}
                imm = MIN(MAX((int)0, imm), (int)255);
                p_im[y][x].c[d] = (BYTE)imm;
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
 }

///////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanIcM (IMTpixel** IMTim, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    unsigned imm, ims, n = 0;
    IMTpixel immean;

    ims = 0;
    i = dy;
    for (y = y0; y < y1; y++)
    {
        j = dx;
        for (x = x0; x < x1; x++)
        {
            if (fmask[i][j])
            {
                imm = IMTim[y][x].s;
                ims += imm;
                n++;
            }
            j++;
        }
        i++;
    }
    if (n > 0) {ims /= n;}
    immean.s = (WORD)ims;
    for (d = 0; d < 3; d++)
    {
        ims = 0;
        i = dy;
        for (y = y0; y < y1; y++)
        {
            j = dx;
            for (x = x0; x < x1; x++)
            {
                if (fmask[i][j])
                {
                    imm = IMTim[y][x].c[d];
                    ims += imm;
                }
                j++;
            }
            i++;
        }
        if (n > 0) {ims /= n;}
        immean.c[d] = (BYTE)ims;
    }

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanPtIcM (IMTpixel** IMTim, IMTpixel IMTimm, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int im, imf, imd, imm, ims = 0;
    IMTpixel immt;
    double imx, spi, sp, p;

    for (d = 0; d < 3; d++)
    {
        im = (int)IMTimm.c[d];
        spi = 0;
        sp = 0;
        i = dy;
        for (y = y0; y < y1; y++)
        {
            j = dx;
            for (x = x0; x < x1; x++)
            {
                if (fmask[i][j])
                {
                    imf = IMTim[y][x].c[d];
                    imd = im - imf;
                    if (imd < 0) {imd = -imd;}
                    p = linfilt[imd];
                    sp += p;
                    imx = (double)imf;
                    spi += (p * imx);
                }
                j++;
            }
            i++;
        }
        if (sp == 0.0)
        {
            imm = im;
        } else {
            spi /= sp;
            imm = (BYTE)MIN(MAX((int)0, (int)(spi + 0.5)), (int)255);
        }
        immt.c[d] = imm;
        ims += imm;
    }
    immt.s = ims;

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

double IMTwbIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, im, immean, i, j;
    long imn = 0, imwn = 0;
    double imwb;

    immean = IMTimm.s;
    i = dy;
    for (y = y0; y < y1; y++)
    {
        j = dx;
        for (x = x0; x < x1; x++)
        {
            if (fmask[i][j])
            {
                im = IMTim[y][x].s;
                if (im > immean) {imwn++;}
                imn++;
            }
            j++;
        }
        i++;
    }
    imwb = (double)imwn;
    imwb /= imn;

    return imwb;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanThIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx, int tflag)
{
    unsigned y, x, d, i, j;
    int im, imm, ims;
    long n = 0;
    int imsc[3] = {0};
    IMTpixel immt;

    imm = (int)IMTimm.s;
    imm *= tflag;
    ims = 0;
    i = dy;
    for (y = y0; y < y1; y++)
    {
        j = dx;
        for (x = x0; x < x1; x++)
        {
            if (fmask[i][j])
            {
                im = (int)IMTim[y][x].s;
                im *= tflag;
                if (im > imm)
                {
                    ims += im;
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = IMTim[y][x].c[d];
                        imsc[d] += im;
                    }
                }
            }
            j++;
        }
        i++;
    }
    if (n > 0)
    {
        ims *= tflag;
        ims /= n;
        immt.s = (WORD)ims;
        for (d = 0; d < 3; d++)
        {
            imsc[d] /= n;
            immt.c[d] = (BYTE)imsc[d];
        }
    }

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanRadIcM (IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int im, imf, imm, ims = 0;
    IMTpixel immt;
    double spi, sp, p;

    for (d = 0; d < 3; d++)
    {
        im = IMTimm.c[d];
        spi = 0;
        sp = 0;
        i = dy;
        for (y = y0; y < y1; y++)
        {
            j = dx;
            for (x = x0; x < x1; x++)
            {
                if (fmask[i][j])
                {
                    imf = IMTim[y][x].c[d];
                    p = sqfilt[i][j];
                    sp += p;
                    spi += (p * (double)(imf));
                }
                j++;
            }
            i++;
        }
        if (sp == 0.0)
        {
            imm = im;
        } else {
            spi /= sp;
            imm = (BYTE)MIN(MAX((int)0, (int)(spi + 0.5)), (int)255);
        }
        immt.c[d] = imm;
        ims += imm;
    }
    immt.s = ims;

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanBlIcM (IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int im, imf, imd, imm, ims = 0;
    IMTpixel immt;
    double imx, spi, sp, p;

    for (d = 0; d < 3; d++)
    {
        im = (int)IMTimm.c[d];
        spi = 0;
        sp = 0;
        i = dy;
        for (y = y0; y < y1; y++)
        {
            j = dx;
            for (x = x0; x < x1; x++)
            {
                if (fmask[i][j])
                {
                    imf = IMTim[y][x].c[d];
                    imd = im - imf;
                    if (imd < 0) {imd = -imd;}
                    p = linfilt[imd];
                    p *= sqfilt[i][j];
                    sp += p;
                    imx = (double)imf;
                    spi += (p * imx);
                }
                j++;
            }
            i++;
        }
        if (sp == 0.0)
        {
            imm = im;
        } else {
            spi /= sp;
            imm = (BYTE)MIN(MAX((int)0, (int)(spi + 0.5)), (int)255);
        }
        immt.c[d] = imm;
        ims += imm;
    }
    immt.s = ims;

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanMinIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int imf, imm, ims = 0;
    IMTpixel immt;

    for (d = 0; d < 3; d++)
    {
        imm = 256;
        i = dy;
        for (y = y0; y < y1; y++)
        {
            j = dx;
            for (x = x0; x < x1; x++)
            {
                if (fmask[i][j])
                {
                    imf = IMTim[y][x].c[d];
                    if (imf < imm) {imm = imf;}
                }
                j++;
            }
            i++;
        }
        immt.c[d] = imm;
        ims += imm;
    }
    immt.s = ims;

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanMaxIcM (IMTpixel** IMTim, IMTpixel IMTimm, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int imf, imm, ims = 0;
    IMTpixel immt;

    for (d = 0; d < 3; d++)
    {
        imm = 0;
        i = dy;
        for (y = y0; y < y1; y++)
        {
            j = dx;
            for (x = x0; x < x1; x++)
            {
                if (fmask[i][j])
                {
                    imf = IMTim[y][x].c[d];
                    if (imf > imm) {imm = imf;}
                }
                j++;
            }
            i++;
        }
        immt.c[d] = imm;
        ims += imm;
    }
    immt.s = ims;

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterKMeans (IMTpixel** IMTim, unsigned height, unsigned width, unsigned ncluster, unsigned iters)
{
    unsigned y, x, i, j, d, y0, x0, y1, x1;
    unsigned nr, nr2, nc, k, d0, dn, dd, dy, dx;
    IMTpixel fgim, bgim;
    double fgdist, bgdist, cl;
    IMTpixel* fg_c;
    fg_c = (IMTpixel*)malloc(ncluster * sizeof(IMTpixel));
    IMTcluster* fg_cs;
    fg_cs = (IMTcluster*)malloc(ncluster * sizeof(IMTcluster));
    cl = (double)ncluster;
    cl = sqrt(cl);
    nr = (int)cl;
    if (nr < cl) {nr++;}
    nr2 = nr / 2;

    if (iters == 0) {iters = ncluster;}

    dy = height / nr;
    for (k = 0; k < nr; k++)
    {
        d0 = (k * ncluster + nr2) / nr;
        dn = ((k + 1) * ncluster + nr2) / nr;
        nc = dn - d0;
        dx = width / nc;
        y0 = k * dy;
        y1 = y0 + dy;
        if (y1 > height) {y1 = height;}
        for (d = d0; d < dn; d++)
        {
            dd = d - d0;
            x0 = dd * dx;
            x1 = x0 + dx;
            if (x1 > width) {x1 = width;}
            fg_c[d] = IMTmeanIc(IMTim, y0, x0, y1, x1);
            fg_cs[d].c[0] = 0;
            fg_cs[d].c[1] = 0;
            fg_cs[d].c[2] = 0;
            fg_cs[d].n = 0;
        }
    }
    for (j = 0; j < iters; j++)
    {
        for (d = 0; d < 3; d++)
        {
            fg_cs[j].c[d] = 0;
        }
        fg_cs[j].n = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                fgim = IMTim[y][x];
                bgdist = 2;
                i = 0;
                for (k = 0; k < ncluster; k++)
                {
                    bgim = fg_c[k];
                    fgdist = IMTdist(fgim, bgim);
                    if (fgdist < bgdist)
                    {
                        bgdist = fgdist;
                        i = k;
                    }
                }
                for (d = 0; d < 3; d++)
                {
                    fg_cs[i].c[d] += fgim.c[d];
                }
                fg_cs[i].n++;
            }
        }
        for (k = 0; k < ncluster; k++)
        {
            i = fg_cs[k].n;
            if (i > 0)
            {
                for (d = 0; d < 3; d++)
                {
                    fg_c[k].c[d] = (BYTE)((fg_cs[k].c[d] + i - 1)/ i);
                }
            }
        }
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            fgim = IMTim[y][x];
            bgdist = 2;
            i = 0;
            for (k = 0; k < ncluster; k++)
            {
                bgim = fg_c[k];
                fgdist = IMTdist(fgim, bgim);
                if (fgdist < bgdist)
                {
                    bgdist = fgdist;
                    i = k;
                }
            }
            bgim.s = 0;
            for (d = 0; d < 3; d++)
            {
                bgim.c[d] = fg_c[i].c[d];
                bgim.s += bgim.c[d];
            }
            IMTim[y][x] = bgim;
        }
    }
    free(fg_c);
    free(fg_cs);

    return iters;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterPeron (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, double noise)
{
    unsigned y, x, d, r, yp, yn, xp, xn, i, j, level;
    int im, imf;
    double dr = 0.2;
    double fim, fimf;

    if (radius < 0) {radius = -radius;}
    if (noise < 0) {noise = -noise;}
    if (noise == 0) {noise = 1;}
    level = (radius / dr);
    for ( r = 0; r < level; r++ )
    {
        for ( y = 0; y < height; y++ )
        {
            if (y == 0) {yp = y;} else {yp = y - 1;}
            yn = y + 2;
            if (yn > height) {yn = height;}
            for ( x = 0; x < width; x++ )
            {
                if (x == 0) {xp = x;} else {xp = x - 1;}
                xn = x + 2;
                if (xn > width) {xn = width;}
                for (d = 0; d < 3; d++)
                {
                    im = p_im[y][x].c[d];
                    fim = 0.0;
                    for (i = yp; i < yn; i++)
                    {
                        for (j = xp; j < xn; j++)
                        {
                            imf = p_im[i][j].c[d];
                            imf -= im;
                            fimf = imf;
                            fimf /= noise;
                            fimf *= fimf;
                            fimf++;
                            fimf = double(imf) / (fimf);
                            fim += fimf;
                        }
                    }
                    fim /= 2;
                    fim *= dr;
                    imf = im + int(fim + 0.5);
                    d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)imf), (int)255);
                }
                d_im[y][x] = IMTcalcS (d_im[y][x]);
            }
        }
        IMTFilterCopy(d_im, p_im, height, width);
    }
 }

////////////////////////////////////////////////////////////////////////////////

double IMTFilterPosterize (IMTpixel** p_im, unsigned height, unsigned width, unsigned thres)
{
    unsigned y, x, d, n;
    unsigned imin, imax, imst;
    double xmin, xmax, xd, ims = 0.0, ime, sumc = 0.0;
    IMTpixel im, immin, immax;

    if (thres < 1) {thres = 1;}
    if (thres > 256) {thres = 256;}

    if (thres > 1)
    {
        n=0;
        for (d = 0; d < 3; d++)
        {
            immin.c[d] = 255;
            immax.c[d] = 0;
        }
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                im = p_im[y][x];
                for (d = 0; d < 3; d++)
                {
                    if (im.c[d] < immin.c[d]) {immin.c[d] = im.c[d];}
                    if (im.c[d] > immax.c[d]) {immax.c[d] = im.c[d];}
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            imin = immin.c[d];
            xmin = imin;
            xd = (immax.c[d] - immin.c[d] + 1);
            xd /= 256.0;
            xd *= thres;
            imst = immax.c[d];
            imst++;
            while (imin < imst)
            {
                xmax = xmin + xd;
                imax = uint(xmax + 0.5);
                n = 0;
                sumc = 0.0;
                for (y = 0; y < height; y++)
                {
                    for (x = 0; x < width; x++)
                    {
                        im = p_im[y][x];
                        if ((im.c[d] >= imin) && (im.c[d] < imax))
                        {
                            sumc += im.c[d];
                            n++;
                        }
                    }
                }
                if (n > 0)
                {
                    sumc /= n;
                    sumc += 0.5;
                    for (y = 0; y < height; y++)
                    {
                        for (x = 0; x < width; x++)
                        {
                            im = p_im[y][x];
                            if ((im.c[d] >= imin) && (im.c[d] < imax))
                            {
                                ime = im.c[d];
                                ime -= sumc;
                                if (ime < 0) {ime = -ime;}
                                ims += ime;
                                p_im[y][x].c[d] = int(sumc);
                            }
                        }
                    }
                }
                xmin += xd;
                imin = uint(xmin + 0.5);
            }
        }
        ims /= 3;
        ims /= width;
        ims /= height;
    }

    return ims;
 }

////////////////////////////////////////////////////////////////////////////////

void IMTFilterPMean (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int fmode, bool fneared)
{
    int y, x, i, j, rn, dy, dx;
    int y0, x0, y1, x1, iradius;
    double l, radius2, p, sp, wb;
    IMTpixel IMTmeanS;
    double deltac[256] = {0};
    int h = height;
    int w = width;

    if (radius < 0) {iradius = (int)(-radius);} else {iradius = (int)radius;}
    rn = 2 * iradius + 1;
    radius2 = radius * radius;

    bool** fmask;
    fmask = (bool**)malloc(rn * sizeof(bool*));
    for (y = 0; y < rn; y++) {fmask[y] = (bool*)malloc(rn * sizeof(bool));}
    double** fradial;
    fradial = (double**)malloc(rn * sizeof(double*));
    for (y = 0; y < rn; y++) {fradial[y] = (double*)malloc(rn * sizeof(double));}

    for (i = -iradius; i <= iradius; i++)
    {
        for (j = -iradius; j <= iradius; j++)
        {
            fmask[i + iradius][j + iradius] = false;
            l = (double)(i * i + j * j);
            if (l < radius2)
            {
                fmask[i + iradius][j + iradius] = true;
                fradial[i + iradius][j + iradius] = 1.0 / (l + 1.0);
            }
        }
    }
    if (fneared)
    {
        fmask[iradius][iradius] = false;
    }

    switch(fmode)
    {
        case 0:
            for (i = 0; i < 256; i++)
            {
                deltac[i] = 1.0 / ((double)(i + 1));
            }
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanPtIcM(p_im, p_im[y][x], deltac, fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 1:
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanIcM(p_im, fmask, y0, x0, y1, x1, dy, dx);
                    wb = IMTwbIcM(p_im, IMTmeanS, fmask, y0, x0, y1, x1, dy, dx);
                    if (wb > 0.5) {
                        IMTmeanS = IMTmeanThIcM(p_im, IMTmeanS, fmask, y0, x0, y1, x1, dy, dx, 1);
                    } else if (wb < 0.5) {
                        IMTmeanS = IMTmeanThIcM(p_im, IMTmeanS, fmask, y0, x0, y1, x1, dy, dx, -1);
                    }
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 2:
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanRadIcM(p_im, p_im[y][x], fradial, fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 3:
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanIcM(p_im, fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 4:
            for (i = 0; i < 256; i++)
            {
                p = (double)i;
                p *= p;
                sp = 2.0 * radius + 1.0;
                sp *= sp;
                deltac[i] = exp2(-p / sp);
            }
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanPtIcM(p_im, p_im[y][x], deltac, fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 5:
            for (i = 0; i < 256; i++)
            {
                p = (double)i;
                p *= p;
                sp = 2.0 * radius + 1.0;
                sp *= sp;
                deltac[i] = exp2(-p / sp);
            }
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanBlIcM(p_im, p_im[y][x], fradial, deltac, fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 6:
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanMinIcM(p_im, p_im[y][x], fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
        case 7:
            for (y = 0; y < h; y++)
            {
                dy = 0;
                y0 = y - iradius;
                if (y0 < 0) {dy = -y0; y0 = 0;}
                y1 = y + iradius + 1;
                if (y1 > h) {y1 = h;}
                for (x = 0; x < w; x++)
                {
                    dx = 0;
                    x0 = x - iradius;
                    if (x0 < 0) {dx = -x0; x0 = 0;}
                    x1 = x + iradius + 1;
                    if (x1 > w) {x1 = w;}
                    IMTmeanS = IMTmeanMaxIcM(p_im, p_im[y][x], fmask, y0, x0, y1, x1, dy, dx);
                    if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                    d_im[y][x] = IMTmeanS;
                }
            }
            break;
    }

    for (y = 0; y < rn; y++){free(fradial[y]);}
    free(fradial);
    for (y = 0; y < rn; y++){free(fmask[y]);}
    free(fmask);
 }

////////////////////////////////////////////////////////////////////////////////

int IMTFilterRetinex (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double sigma)
{
    unsigned d, k;
    int y, x, yf, xf, i, j;
    int im, imf, kimi;
    double dbi, dbj, gauss, imx, imxf, imxg, imxn, kim, kims;
    double imxs, imxns;
    int h = height;
    int w = width;

    if (radius < 0) {radius = -radius;}
    unsigned  ksz = 2 * radius + 1;
    int  kszm = ksz * ksz;
    double* gaussKernel = new double[kszm];
    double div2 = (sqrt(5.0)+1.0)/2.0-1.0;
    double sumgk = 0.0;

    k = 0;
    for (i = -radius; i <= radius; i++)
    {
        dbi=(double)(i);
        for (j = -radius; j <= radius; j++)
        {
            dbj=(double)(j);
            gauss = exp(-(dbi * dbi + dbj * dbj + 1.0) / (sigma * sigma + 1.0));
            gaussKernel[k] = gauss;
            sumgk += gauss;
            k++;
        }
    }
    for (i = 0; i < kszm; i++)
    {
        gaussKernel[i] /= sumgk;
    }
    kims=0.0;
    imxs=0.0;
    imxns=0.0;
    for ( y = 0; y < h; y++ )
    {
        for ( x = 0; x < w; x++ )
        {
            im = p_im[y][x].s;
            imx = ((double)im / 3.0 + 1.0) / 256.0;
            imxs += imx;
            imxg = 0;
            k = 0;
            for (i = -radius; i <= radius; i++)
            {
                yf = y + i;
                if (yf < 0) {yf = -yf;}
                if (yf >= h) {yf = 2 * (height - 1) - yf;}
                for (j = -radius; j <= radius; j++)
                {
                    xf = x + j;
                    if (xf < 0) {xf = -xf;}
                    if (xf >= w) {xf = 2 * (width -1) - xf;}
                    imf = p_im[yf][xf].s;
                    imxf = ((double)imf / 3.0 + 1.0) / 256.0;
                    imxg += imxf * gaussKernel[k];
                    k++;
                }
            }
            imxn = log(imx / imxg + div2);
            imxns += imxn;
            if (imxn < 0.0) {imxn = 0.0;}
            if (imxn > 1.0) {imxn = 1.0;}
            kim = imxn / imx;
            kims += kim;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                imx = (double)im;
                im = MIN(MAX((int)0, (int)(imx * kim + 0.5)), (int)255);
                d_im[y][x].c[d] = (BYTE)im;
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    imxs /= (width*height);
    kims /= (width*height);
    imxns /= (width*height);
    kimi = (int)(255 * kims / 2);

    delete [] gaussKernel;

    return kimi;
 }

////////////////////////////////////////////////////////////////////////////////

double IMTFilterRS (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned d;
    int y, x, yp2, xp2, yp1, xp1, yn1, xn1, yn2, xn2;
    double imx = 0, ims = 0, imd = 0;
    int h = height;
    int w = width;
    IMTpixel** t_im;
    t_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
    for (d = 0; d < height; d++) {t_im[d] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

    imd = 0;
    for ( y = 0; y < h; y++ )
    {
        yp2 = y - 2;
        if (yp2 < 0) {yp2 = 0;}
        yp1 = y - 1;
        if (yp1 < 0) {yp1 = 0;}
        yn1 = y + 1;
        if (yn1 >= h) {yn1 = h - 1;}
        yn2 = y + 2;
        if (yn2 >= h) {yn2 = h - 1;}
        for ( x = 0; x < w; x++ )
        {
            xp2 = x - 2;
            if (xp2 < 0) {xp2 = 0;}
            xp1 = x - 1;
            if (xp1 < 0) {xp1 = 0;}
            xn1 = x + 1;
            if (xn1 >= w) {xn1 = w - 1;}
            xn2 = x + 2;
            if (xn2 >= h) {xn2 = w - 1;}
            for (d = 0; d < 3; d++)
            {
                ims = 0;
                imx = p_im[yp2][xp2].c[d];
                imx *= imx;
                ims -= imx;
                imx = p_im[yp2][xp1].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yp2][x].c[d];
                imx *= imx;
                ims -= (3 * imx);
                imx = p_im[yp2][xn1].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yp2][xn2].c[d];
                imx *= imx;
                ims -= imx;
                imx = p_im[yp1][xp2].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yp1][xp1].c[d];
                imx *= imx;
                ims += (5 * imx);
                imx = p_im[yp1][x].c[d];
                imx *= imx;
                ims += (3 * imx);
                imx = p_im[yp1][xn1].c[d];
                imx *= imx;
                ims += (5 * imx);
                imx = p_im[yp1][xn2].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[y][xp2].c[d];
                imx *= imx;
                ims -= (3 * imx);
                imx = p_im[y][xp1].c[d];
                imx *= imx;
                ims += (3 * imx);
                imx = p_im[y][xn1].c[d];
                imx *= imx;
                ims += (3 * imx);
                imx = p_im[y][xn2].c[d];
                imx *= imx;
                ims -= (3 * imx);
                imx = p_im[yn1][xp2].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yn1][xp1].c[d];
                imx *= imx;
                ims += (5 * imx);
                imx = p_im[yn1][x].c[d];
                imx *= imx;
                ims += (3 * imx);
                imx = p_im[yn1][xn1].c[d];
                imx *= imx;
                ims += (5 * imx);
                imx = p_im[yn1][xn2].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yn2][xp2].c[d];
                imx *= imx;
                ims -= imx;
                imx = p_im[yn2][xp1].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yn2][x].c[d];
                imx *= imx;
                ims -= (3 * imx);
                imx = p_im[yn2][xn1].c[d];
                imx *= imx;
                ims -= (2 * imx);
                imx = p_im[yn2][xn2].c[d];
                imx *= imx;
                ims -= imx;
                ims /= 81.0;
                if (ims < 0) {imd -= ims;} else {imd += ims;}
                imx = p_im[y][x].c[d];
                imx *= imx;
                ims += imx;
                if (ims < 0) {ims = -ims;}
                ims = sqrt(ims);
                t_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(ims + 0.5)), (int)255);
            }
        }
    }
    for ( y = 0; y < h; y++ )
    {
        for ( x = 0; x < w; x++ )
        {
            p_im[y][x] = IMTcalcS (t_im[y][x]);
        }
    }
    imd /= h;
    imd /= w;
    imd /= 1.5;
    imd = sqrt(imd);
    imd /= 255.0;

    for (d = 0; d < height; d++){free(t_im[d]);}
    free(t_im);

    return imd;
 }

////////////////////////////////////////////////////////////////////////////////

void IMTSelGaussInitMatrix (double radius, double *mat, int num)
{
    int    dx;
    double sd, c1, c2;

    // This formula isn't really correct, but it'll do
    sd = radius / 3.329042969;
    c1 = 1.0 / sqrt (2.0 * PI * sd);
    c2 = -2.0 * (sd * sd);

    for (dx = 0; dx <= num; dx++)
        mat[dx] = c1 * exp ((dx * dx)/ c2);
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSelGauss (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int maxdelta)
{
    double     *mat;
    int         numrad;

    unsigned d;

    numrad = (int)radius;
    mat = (double*) calloc (numrad + 1, sizeof(double));

    IMTSelGaussInitMatrix(radius, mat, numrad);

    unsigned short    *imat;
    double     fsum, fscale;
    int        i, j, x, y, xk, yk;
    int h = height;
    int w = width;

    imat = (unsigned short*)calloc (2 * numrad + 1, sizeof(unsigned short));

    fsum = 0.0;
    for (y = -numrad; y <= numrad; y++)
    {
        fsum += mat[abs(y)];
    }

    // Ensure that the sum fits in 32bits.
    fscale = 0x1000 / fsum;
    for (y = 0; y <= numrad; y++)
    {
        imat[numrad - y] = imat[numrad + y] = mat[y] * fscale;
    }

    /////////////////////////////////////////

    unsigned p_sum[3] = {0};
    unsigned p_fact[3] = {0};
    unsigned p_rowsum[3] = {0};
    unsigned p_rowfact[3] = {0};

    int di;
    int tmp;

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            for (d = 0; d < 3; d++)
            {
                p_sum[d] = 0;
                p_fact[d] = 0;
            }
            for (j = -numrad; j <= numrad; j++)
            {
                for (d = 0; d < 3; d++)
                {
                    p_rowsum[d] = 0;
                    p_rowfact[d] = 0;
                }
                yk = y + j;
                if (yk >= 0 && yk < h)
                {
                    for (i = -numrad; i < numrad; i++)
                    {
                        xk = x + i;
                        if (xk >= 0 && xk < w)
                        {
                            for (d = 0; d < 3; d++)
                            {
                                tmp = p_im[y][x].c[d];
                                tmp -= p_im[yk][xk].c[d];
                                if ( tmp >= -maxdelta && tmp <= maxdelta)
                                {
                                    di = imat[numrad + i];
                                    p_rowsum[d] += di * (double)p_im[yk][xk].c[d];
                                    p_rowfact[d] += di;
                                }
                            }
                        }
                    }
                    di = imat[numrad + j];
                    for (d = 0; d < 3; d++)
                    {
                        p_sum[d] += di * p_rowsum[d];
                        p_fact[d] += di * p_rowfact[d];
                    }
                }
            }
            for (d = 0; d < 3; d++)
            {
                if (p_fact[d] == 0)
                    d_im[y][x].c[d] = p_im[y][x].c[d];
                else
                    d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(p_sum[d] / p_fact[d])), (int)255);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }

    // free up buffers
    free (imat);
    free (mat);
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterShrink (IMTpixel** p_im, unsigned height, unsigned width, int thres)
{
    unsigned d;
    int y, x, n;
    int imdy , imdx, imd;
    double ims;
    IMTpixel im, imy, imx, imf;
    int h = height;
    int w = width;

    if (thres < 0)
    {
        ims = IMTmean (p_im, height, width);
        ims = IMTdev (p_im, ims, height, width);
        ims /= -thres;
        thres = (int)ims;
    }
    if (thres > 255) {thres = 255;}

    n=0;
    for ( y = 0; y < h; y++ )
    {
        for ( x = 0; x < w; x++ )
        {
            imd = 0;
            imdy = 256;
            im = p_im[y][x];
            if (y > 0)
            {
                imdy = 0;
                imy = p_im[y - 1][x];
                for (d = 0; d < 3; d++)
                {
                    if (im.c[d] > imy.c[d]) {imd = im.c[d] - imy.c[d];} else {imd = imy.c[d] - im.c[d];}
                    if (imdy < imd) {imdy = imd;}
                }
            }
            imdx = 256;
            if (x > 0)
            {
                imdx = 0;
                imx = p_im[y][x - 1];
                for (d = 0; d < 3; d++)
                {
                    if (im.c[d] > imx.c[d]) {imd = im.c[d] - imx.c[d];} else {imd = imx.c[d] - im.c[d];}
                    if (imdx < imd) {imdx = imd;}
                }
            }
            if (imdy < imdx)
            {
                imf = imy;
                imd = imdy;
            } else {
                imf = imx;
                imd = imdx;
            }
            if (imd < thres)
            {
                im = imf;
            }
            if (im.s != p_im[y][x].s) {n++;}
            p_im[y][x] = im;
        }
    }
    for ( y = h - 1; y >= 0; y-- )
    {
        for ( x = w - 1; x >= 0; x-- )
        {
            imd = 0;
            imdy = 256;
            im = p_im[y][x];
            if (y < h - 1)
            {
                imdy = 0;
                imy = p_im[y + 1][x];
                for (d=0; d < 3; d++)
                {
                    if (im.c[d] > imy.c[d]) {imd = im.c[d] - imy.c[d];} else {imd = imy.c[d] - im.c[d];}
                    if (imdy < imd) {imdy = imd;}
                }
            }
            imdx = 256;
            if (x < w - 1)
            {
                imdx = 0;
                imx = p_im[y][x + 1];
                for (d=0; d < 3; d++)
                {
                    if (im.c[d] > imx.c[d]) {imd = im.c[d] - imx.c[d];} else {imd = imx.c[d] - im.c[d];}
                    if (imdx < imd) {imdx = imd;}
                }
            }
            if (imdy < imdx)
            {
                imf = imy;
                imd = imdy;
            } else {
                imf = imx;
                imd = imdx;
            }
            if (imd < thres)
            {
                im = imf;
            }
            if (im.s != p_im[y][x].s) {n++;}
            p_im[y][x] = im;
        }
    }
    ims = (double)(n) / height / width * 2 / 3;

    return ims;
 }

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSobel (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned int y, x, d;
    int i, j, im, imf, ims;
    BYTE val;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                d_im[y][x].c[d] = 255;
            }
            d_im[y][x].s = 765;
        }
    }
    for (y = 1; y < height - 1; y++)
    {
        for (x = 1; x < width - 1; x++)
        {
            ims = 0;
            for (d = 0; d < 3; d++)
            {
                imf = p_im[y][x].c[d];
                im = imf * 9;
                for (i = -1; i <= 1; i++)
                {
                    for (j = -1; j <= 1; j++)
                    {
                        imf = p_im[y + i][x + j].c[d];
                        im -= imf;
                    }
                }
                im = 255 - im;
                val = (BYTE)MIN(MAX((int)0, im), (int)255);
                ims += int(val);
                d_im[y][x].c[d] = val;
            }
            d_im[y][x].s = (WORD)ims;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterUnRipple (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double thres)
{

    unsigned d;
    int y, x, yf, xf, i, j;
    int im, imf, imd;
    double imx, p, sp, spi, si, di;
    int h = height;
    int w = width;

    if (radius < 0) {radius = -radius;}
    if (thres < 0) {thres = 0;}

    for ( y = 0; y < h; y++ )
    {
        for ( x = 0; x < w; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                sp = 0;
                spi = 0;
                for (i = -radius; i <= radius; i++)
                {
                    yf = y + i;
                    if (yf < 0) {yf = -yf;}
                    if (yf >= h) {yf = 2 * (h - 1)- yf;}
                    for (j = -radius; j <= radius; j++)
                    {
                        xf = x + j;
                        if (xf < 0) {xf = -xf;}
                        if (xf >= w) {xf = 2 * (w - 1) - xf;}
                        imf = p_im[yf][xf].c[d];
                        imd = im - imf;
                        if (imd < 0) {imd = -imd;}
                        imx = (double)(imd);
                        p = 1.0 / (imx + 1.0);
                        sp += p;
                        imx = (double)imf * p;
                        spi += imx;
                    }
                }
                imx = (double)(im);
                if (sp == 0.0)
                {
                    si = imx;
                } else {
                    si = spi/sp;
                }
                di = imx-si;
                if (di < 0.0) {di = -di;}
                if (di > thres)
                {
                    d_im[y][x].c[d] = p_im[y][x].c[d];
                } else {
                    im = MIN(MAX((int)0, (int)(si + 0.5)), (int)255);
                    d_im[y][x].c[d] = (BYTE)(im);
                }
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTGaussLineMatrix (double *cmatrix, double radius)
{
    double std_dev;
    double sum, t, tt;
    int i, j, iradius;

    if (radius < 0) {radius = -radius;}
    std_dev = radius;
    iradius = (int)(2.0 * radius + 0.5) + 1;
    if (iradius > 1)
    {
        for (i = 0; i < iradius; i++)
        {
            sum = 0;
            for (j = 0; j <= 50; j++)
            {
                t = i;
                t -= 0.5;
                t += 0.02 * j;
                tt = -(t * t) / (2 * std_dev * std_dev);
                sum += exp (tt);
            }
            cmatrix[i] = sum / 50;
        }
        sum = cmatrix[0];
        for (i = 1; i < iradius; i++)
        {
            sum += 2 * cmatrix[i];
        }
        for (i = 0; i < iradius; i++)
        {
            cmatrix[i] = cmatrix[i] / sum;
        }
    } else {
        cmatrix[0] = 1;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////

void IMTFilterGaussBlur (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius)
{
    int iradius;
    int y, x, yf, xf, i;
    unsigned d;
    double imc;
    double sc[3];
    double* gaussmat;
    BYTE val;
    IMTpixel** t_im;
    int h = height;
    int w = width;

    if (radius < 0) {radius = -radius;}
    iradius = (int)(2.0 * radius + 0.5) + 1;

    gaussmat = (double*)malloc((iradius) * sizeof(double));
    t_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
    for (y = 0; y < h; y++) {t_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

    IMTGaussLineMatrix (gaussmat, radius);

    if (iradius > 1)
    {
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                for (d = 0; d < 3; d++)
                {
                    imc = p_im[y][x].c[d];
                    sc[d] = imc * gaussmat[0];
                }
                for (i = 1; i < iradius; i++)
                {
                    yf = y - i;
                    if (yf < 0) {yf = 0;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = p_im[yf][x].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                    yf = y + i;
                    if (yf >= h) {yf = h - 1;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = p_im[yf][x].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                }
                for (d = 0; d < 3; d++)
                {
                    t_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(sc[d] + 0.5)), (int)255);
                }
            }
        }
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                for (d = 0; d < 3; d++)
                {
                    imc = t_im[y][x].c[d];
                    sc[d] = imc * gaussmat[0];
                }
                for (i = 1; i < iradius; i++)
                {
                    xf = x - i;
                    if (xf < 0) {xf = 0;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = t_im[y][xf].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                    xf = x + i;
                    if (xf >= w) {xf = w - 1;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = t_im[y][xf].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                }
                for (d = 0; d < 3; d++)
                {
                    val = (BYTE)MIN(MAX((int)0, (int)(sc[d] + 0.5)), (int)255);
                    d_im[y][x].c[d] = val;
                }
                d_im[y][x] = IMTcalcS (d_im[y][x]);
            }
        }
    } else {
        for (y = 0; y < h; y++)
        {
            for (x = 0; x< w; x++)
            {
                d_im[y][x] = p_im[y][x];
            }
        }
    }

    for (y = 0; y < h; y++){free(t_im[y]);}
    free(t_im);
    free(gaussmat);
}

//////////////////////////////////////////////////////////////////////////////////////////////

double IMTFilterBgDiv (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned ndiv)
{
    unsigned y, x, d, k, im, n;
    int t, dd;
    double xdiv, xx, value, ims;

    ims = 0;
    n = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            ims += im;
            n++;
        }
    }
    ims /= n;
    ims /= 3.0;
    ims /= 255.0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            t = p_im[y][x].s;
            t++;
            dd = b_im[y][x].s;
            dd++;
            xdiv = t;
            xx = dd;
            xdiv /= xx;
            xx = xdiv;
            for (k = 0; k < ndiv; k++)
            {
                xdiv *= xx;
            }
            xx = 1.0 / (ims + 0.5);
            for (d = 0; d < 3; d++)
            {
                value = p_im[y][x].c[d];
                value -= 127.5;
                value *= xx;
                value += 127.5;
                value *= xdiv;
                d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(value + 0.5)), (int)255);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    return ims;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterClusterBiModC (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned ncolor)
{
    unsigned y, x, d, k, l, m, n, im, imt, ims, imds, imdm;
    int imd;
    double T, Tw, Tb, Tn, iw, ib, cn[3];
    unsigned thres[3];
    unsigned cd[256];

    imdm = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            d_im[y][x].s = 0;
        }
    }
    m = 0;
    for (l = 1; l < ncolor; l++)
    {
        for (d = 0; d < 3; d++)
        {
            cn[d] = 0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = d_im[y][x].s;
                if (imt == m)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = p_im[y][x].c[d];
                        cn[d] += im;
                    }
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            if (n > 0)
            {
                cn[d] /= n;
            } else {
                cn[d] = 127.5;
            }
        }
        for (d = 0; d < 3; d++)
        {
            thres[d] = 0;
            T = cn[d];
            Tn = 0;
            while ( T != Tn )
            {
                Tn = T;
                Tb = 0;
                Tw = 0;
                ib = 0;
                iw = 0;
                for (y = 0; y < height; y++ )
                {
                    for (x = 0; x < width; x++)
                    {
                        im = p_im[y][x].c[d];
                        imt = d_im[y][x].s;
                        if (imt == m)
                        {
                            if ( im > T)
                            {
                                Tw += im;
                                iw++;
                            } else {
                                Tb += im;
                                ib++;
                            }
                        }
                    }
                }
                if (iw == 0 && ib == 0)
                {
                    T = Tn;
                } else if (iw == 0) {
                    T = Tb/ib;
                } else if (ib == 0) {
                    T = Tw/iw;
                } else {
                    T = ((Tw/iw) + (Tb/ib)) / 2.0;
                }
            }
            if (T < 0) {T = 0;}
            if (T > 255) {T = 255;}
            thres[d] = (int)T;
        }
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = d_im[y][x].s;
                if (imt == m)
                {
                    ims = 0;
                    for (d = 0; d < 3; d++)
                    {
                        im = p_im[y][x].c[d];
                        ims += ((im >= thres[d]) ? 255 : 0);
                    }
                    if (ims >= 384) {d_im[y][x].s = l;}
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            cn[d] = 0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = d_im[y][x].s;
                if (imt == m)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = p_im[y][x].c[d];
                        cn[d] += im;
                    }
                }
            }
        }
        cd[m] = 0;
        if (n > 0)
        {
            for (d = 0; d < 3; d++)
            {
                cn[d] /= n;
            }
            for (y = 0; y < height; y++ )
            {
                for (x = 0; x < width; x++)
                {
                    imt = d_im[y][x].s;
                    if (imt == m)
                    {
                        imds = 0;
                        for (d = 0; d < 3; d++)
                        {
                            imd = p_im[y][x].c[d];
                            imd -= cn[d];
                            if (imd < 0) {imd = -imd;}
                            imds += imd;
                        }
                        if (imds > cd[m]) {cd[m] = imds;}
                    }
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            cn[d] = 0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = d_im[y][x].s;
                if (imt == l)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = p_im[y][x].c[d];
                        cn[d] += im;
                    }
                }
            }
        }
        cd[l] = 0;
        if (n > 0)
        {
            for (d = 0; d < 3; d++)
            {
                cn[d] /= n;
            }
            for (y = 0; y < height; y++ )
            {
                for (x = 0; x < width; x++)
                {
                    imt = d_im[y][x].s;
                    if (imt == l)
                    {
                        imds = 0;
                        for (d = 0; d < 3; d++)
                        {
                            imd = p_im[y][x].c[d];
                            imd -= cn[d];
                            if (imd < 0) {imd = -imd;}
                            imds += imd;
                        }
                        if (imds > cd[l]) {cd[l] = imds;}
                    }
                }
            }
        }
        imdm = 0;
        for (k = 0; k <= l; k++)
        {
            if (cd[k] > imdm)
            {
                imdm = cd[k];
                m = k;
            }
        }
    }
    for (l = 0; l < ncolor; l++)
    {
        for (d = 0; d < 3; d++)
        {
            cn[d] = 0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = d_im[y][x].s;
                if (imt == l)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = p_im[y][x].c[d];
                        cn[d] += im;
                    }
                }
            }
        }
        if (n > 0)
        {
            for (d = 0; d < 3; d++)
            {
                cn[d] /= n;
            }
        }
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = d_im[y][x].s;
                if (imt == l)
                {
                    for (d = 0; d < 3; d++)
                    {
                        d_im[y][x].c[d] = BYTE(cn[d]);
                    }
                }
            }
        }
    }
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    return imdm;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterClusterBWC (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned radius)
{
    unsigned y, x, d, y1, x1, y2, x2, i, j, distw, distb, gpart, bc, bs;
    IMTpixel pim, gim, bim, fim, cimw, cimb, cim;
    unsigned bwcsw[3], bwcsb[3], bwcnw, bwcnb, cw, cb;
    double kwb = 0.5;

    if (radius == 0)
    {
        for ( y = 0; y < height; y++ )
        {
            for ( x = 0; x < width; x++ )
            {
                d_im[y][x] = p_im[y][x];
            }
        }
    } else {
        cw = 0;
        cb = 0;
        gim = IMTmeanIc(p_im, 0, 0, height, width);
        gpart = height;
        gpart += width;
        gpart /= radius;
        gpart /= 2;
        gpart++;
        gpart /= 2;
        if (gpart == 0) {gpart = 1;}
        for (y = 0; y < height; y++)
        {
            
            if (y <= radius) {y1 = 0;} else {y1 = y - radius;}
            y2 = y;
            y2 += radius;
            y2++;
            if (y2 > height) {y2 = height;}
            for (x = 0; x < width; x++)
            {
                if (x <= radius) {x1 = 0;} else {x1 = x - radius;}
                x2 = x;
                x2 += radius;
                x2++;
                if (x2 > width) {x2 = width;}
                pim = p_im[y][x];
                cim = pim;
                bim = IMTmeanIc(p_im, y1, x1, y2, x2);
                bs = 0;
                for (d = 0; d < 3; d++)
                {
                    bc = bim.c[d];
                    bc *= gpart;
                    bc += gim.c[d];
                    bc /= (gpart + 1);
                    bim.c[d] = bc;
                    bs += bc;
                }
                bim.s = bs;
                bwcnw = 0;
                bwcnb = 0;
                for (d = 0; d < 3; d++)
                {
                    bwcsw[d] = 0;
                    bwcsb[d] = 0;
                }
                for (i = y1; i < y2; i++)
                {
                    for (j = x1; j < x2; j++)
                    {
                        fim = p_im[i][j];
                        if (fim.s >= bim.s)
                        {
                            for (d = 0; d < 3; d++)
                            {
                                bwcsw[d] += fim.c[d];
                            }
                            bwcnw++;
                        }
                        if (fim.s <= bim.s)
                        {
                            for (d = 0; d < 3; d++)
                            {
                                bwcsb[d] += fim.c[d];
                            }
                            bwcnb++;
                        }
                    }
                }
                if (bwcnw > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        bwcsw[d] /= bwcnw;
                        bwcsw[d] = MIN(MAX((int)0, (int)(bwcsw[d] + 0.5)), (int)255);
                    }
                    cimw = IMTset(BYTE(bwcsw[0]),BYTE(bwcsw[1]),BYTE(bwcsw[2]));
                } else {
                    cimw = IMTset(255,255,255);
                }
                if (bwcnb > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        bwcsb[d] /= bwcnb;
                        bwcsb[d] = MIN(MAX((int)0, (int)(bwcsb[d] + 0.5)), (int)255);
                    }
                    cimb = IMTset(BYTE(bwcsb[0]),BYTE(bwcsb[1]),BYTE(bwcsb[2]));
                } else {
                    cimb = IMTset(0,0,0);
                }
                distw = IMTdist(pim, cimw);
                distb = IMTdist(pim, cimb);
                d_im[y][x] = p_im[y][x];
                if (distb < distw)
                {
                    cim = cimb;
                    cb++;
                } else {
                    cim = cimw;
                    cw++;
                }
                d_im[y][x] = cim;
            }
        }
        kwb = cw;
        kwb /= (cw + cb);
    }

    return kwb;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterUnsharpMask (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width, double amount, int threshold)
{
    unsigned y, x, d;
    int t, diff, adiff;
    double value;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                t = p_im[y][x].c[d];
                diff = t - b_im[y][x].c[d];
                adiff = diff;
                if (adiff < 0){ adiff = -adiff;}
                if (2 * adiff < threshold) {adiff = 0;}

                value = p_im[y][x].c[d];
                value += (amount * diff);
                d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(value + 0.5)), (int)255);;
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterMonoColor (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned x, y, d, maxc, maxcv;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            maxc = 0;
            maxcv = p_im[y][x].c[0];
            for (d = 1; d < 3; d++)
            {
                if (p_im[y][x].c[d] > maxcv)
                {
                    maxc = d;
                    maxcv = p_im[y][x].c[d];
                }
            }
            for (d = 0; d < 3; d++)
            {
                p_im[y][x].c[d] = 0;
            }
            p_im[y][x].c[maxc] = (BYTE)maxcv;
            p_im[y][x].s = (WORD)maxcv;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterNoiseVariance (IMTpixel** p_im, unsigned height, unsigned width, int radius)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, imn;
    int h = height;
    int w = width;
    double noise = 0.0;

    if (radius < 0) {radius = -radius;}

    imn = 0;
    for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    n++;
                }
            }
            if (n > 0) {imm /= n;}
            imm /= 3;
            imv = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imv += (imx * imx);
                    n++;
                }
            }
            if (n > 0) {imv /= n;}
            imv /= 9;
            imv -= (imm * imm);
            imn += imv;
        }
    }
    imn /= width;
    imn /= height;
    imn *= 0.25;
    if (imn < 0) {imn = -imn;}
    noise = sqrt(imn);

    return noise;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterWiener (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double noise)
{
    unsigned x, y, d, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, kvar, var, multiplier;
    int h = height;
    int w = width;
    int ivar;

    noise *= noise;
    if (radius < 0) {radius = -radius;}

    for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    n++;
                }
            }
            if (n > 0) {imm /= n;}
            imm /= 3;
            imv = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imv += (imx * imx);
                    n++;
                }
            }
            if (n > 0) {imv /= n;}
            imv /= 9;
            imv -= (imm * imm);
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imv < noise)
            {
                kvar = imm - imx;
            } else {
                multiplier = (imv - noise) / imv;
                kvar = imm + multiplier * (imx - imm) - imx;
            }
            for (d = 0; d < 3; d++)
            {
                var = p_im[y][x].c[d] + kvar;
                ivar = MIN(MAX((int)0, (int)(var+0.5)), (int)255);
                d_im[y][x].c[d] = ivar;
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int IMTFilterWhiteFill (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, yy, xx;
    int im, sw, imf, niter = 0;

    sw = 1;
    while (sw > 0)
    {
        niter++;
        sw = 0;
        for ( x = 0; x < width; x++ )
        {
            for ( y = 0; y < height; y++ )
            {
                im = p_im[y][x].s;
                if (im != 765)
                {
                    if (x > 0)
                    {
                        imf = p_im[y][x - 1].s;
                        if (imf == 765) {p_im[y][x - 1] = p_im[y][x];}
                    }
                    if (x < width - 1)
                    {
                        imf = p_im[y][x + 1].s;
                        if (imf == 765) {p_im[y][x + 1] = p_im[y][x];}
                    }
                    if (y > 0)
                    {
                        imf = p_im[y - 1][x].s;
                        if (imf == 765) {p_im[y - 1][x] = p_im[y][x];}
                    }
                    if (y < height - 1)
                    {
                        imf = p_im[y + 1][x].s;
                        if (imf == 765) {p_im[y + 1][x] = p_im[y][x];}
                    }
                } else {
                    sw++;
                }
            }
        }
        sw = 0;
        for ( xx = width; xx > 0; xx-- )
        {
            x = xx - 1;
            for ( yy = height; yy > 0; yy-- )
            {
                y = yy - 1;
                im = p_im[y][x].s;
                if (im != 765)
                {
                    if (x > 0)
                    {
                        imf = p_im[y][x - 1].s;
                        if (imf == 765) {p_im[y][x - 1] = p_im[y][x];}
                    }
                    if (x < width - 1)
                    {
                        imf = p_im[y][x + 1].s;
                        if (imf == 765) {p_im[y][x + 1] = p_im[y][x];}
                    }
                    if (y > 0)
                    {
                        imf = p_im[y - 1][x].s;
                        if (imf == 765) {p_im[y - 1][x] = p_im[y][x];}
                    }
                    if (y < height - 1)
                    {
                        imf = p_im[y + 1][x].s;
                        if (imf == 765) {p_im[y + 1][x] = p_im[y][x];}
                    }
                } else {
                    sw++;
                }
            }
        }
    }
    return niter;
 }

////////////////////////////////////////////////////////////////////////////////

double BiCubicKernel (double x)
{
    if ( x > 2.0 )
        return 0.0;

    double a, b, c, d;
    double xm1 = x - 1.0;
    double xp1 = x + 1.0;
    double xp2 = x + 2.0;

    a = ( xp2 <= 0.0 ) ? 0.0 : xp2 * xp2 * xp2;
    b = ( xp1 <= 0.0 ) ? 0.0 : xp1 * xp1 * xp1;
    c = ( x   <= 0.0 ) ? 0.0 : x * x * x;
    d = ( xm1 <= 0.0 ) ? 0.0 : xm1 * xm1 * xm1;

    return ( 0.16666666666666666667 * ( a - ( 4.0 * b ) + ( 6.0 * c ) - ( 4.0 * d ) ) );
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSBicub (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width)
{

    unsigned d;
    double xFactor = (double)width / new_width;
    double yFactor = (double)height / new_height;
    double  ox, oy, dx, dy, k1, k2;
    int y, x, ox1, oy1, ox2, oy2, m, n;
    double p_g[3] = {0};
    int h = new_height;
    int w = new_width;
    int ymax = height - 1;
    int xmax = width - 1;

    for (y = 0; y < h; y++ )
    {
        oy  = (double)y * yFactor - 0.5;
        oy1 = (int)oy;
        dy  = oy - (double)oy1;
        for (x = 0; x < w; x++ )
        {
            ox  = (double)x * xFactor - 0.5f;
            ox1 = (int)ox;
            dx  = ox - (double)ox1;
            for (d = 0; d < 3; d++)
            {
                p_g[d] = 0;
            }
            for (n = -1; n < 3; n++)
            {
                k1 = BiCubicKernel(dy - (double)n);
                oy2 = oy1 + n;
                if ( oy2 < 0 ) {oy2 = 0;}
                if ( oy2 > ymax ) {oy2 = ymax;}
                for (m = -1; m < 3; m++)
                {
                    k2 = k1 * BiCubicKernel((double)m - dx);
                    ox2 = ox1 + m;
                    if ( ox2 < 0 ) {ox2 = 0;}
                    if ( ox2 > xmax ) {ox2 = xmax;}
                    for (d = 0; d < 3; d++)
                    {
                        p_g[d] += (k2 * (double)p_im[oy2][ox2].c[d]);
                    }
                }
            }
            for (d = 0; d < 3; d++)
            {
                d_im[y][x].c[d] = (BYTE)p_g[d];
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSBilin (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width)
{
    unsigned d;
    double xFactor = (double)width / new_width;
    double yFactor = (double)height / new_height;
    double ox, oy, dx1, dy1, dx2, dy2;
    int y, x, ox1, oy1, ox2, oy2;
    int h = new_height;
    int w = new_width;
    int ymax = height - 1;
    int xmax = width - 1;

    for (y = 0; y < h; y++)
    {
        oy  = (double)y * yFactor;
        oy1 = (int)oy;
        oy2 = (oy1 == ymax) ? oy1 : oy1 + 1;
        dy1 = oy - (double)oy1;
        dy2 = 1.0 - dy1;
        for (x = 0; x < w; x++)
        {
            ox  = (double)x * xFactor;
            ox1 = (int)ox;
            ox2 = (ox1 == xmax) ? ox1 : ox1 + 1;
            dx1 = ox - (double)ox1;
            dx2 = 1.0 - dx1;
            for (d = 0; d < 3; d++)
            {
                d_im[y][x].c[d] = (BYTE) (dy2 * (dx2 * p_im[oy1][ox1].c[d] + dx1 * p_im[oy1][ox2].c[d]) + dy1 * (dx2 * p_im[oy2][ox1].c[d] + dx1 * p_im[oy2][ox2].c[d]));
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterSBWMag2 (BYTE** d_im, BYTE** r_im, unsigned height, unsigned width, unsigned height2, unsigned width2)
{
    unsigned y, x, y1, x1, y2, x2, n, s, threshold = 0;
    int im, er, ym, xm, yp, xp;
    int h = height;
    int w = width;

    n = 0;
    s = 0;
    er = 0;
    for (y = 0; y < height; y++ )
    {
        ym = y;
        ym--;
        if (ym < 0) {ym = 0;}
        yp = y;
        yp++;
        if (yp >= h) {yp = h - 1;}
        y1 = y * 2;
        if (y1 >= height2) {y1 = height2 -1;}
        y2 = y1 + 1;
        if (y2 >= height2) {y2 = height2 -1;}
        for (x = 0; x < width; x++)
        {
            xm = x;
            xm--;
            if (xm < 0) {xm = 0;}
            xp = x;
            xp++;
            if (xp >= w) {xp = w - 1;}
            x1 = 2 * x;
            if (x1 >= width2) {x1 = width2 -1;}
            x2 = x1 + 1;
            if (x2 >= width2) {x2 = width2 -1;}
            im = d_im[y][x];
            er = (16 * im);
            im = d_im[ym][x];
            er -= (2 * im);
            im = d_im[yp][x];
            er -= (2 * im);
            im = d_im[y][xm];
            er -= (2 * im);
            im = d_im[y][xp];
            er -= (2 * im);
            im = d_im[ym][xm];
            er -= im;
            im = d_im[yp][xm];
            er -= im;
            im = d_im[ym][xp];
            er -= im;
            im = d_im[yp][xp];
            er -= im;
            er /= 12;

            im = d_im[y][x];
            im += d_im[ym][x];
            im += d_im[y][xm];
            im += d_im[ym][xm];
            im += er;
            im /= 4;
            if (im == 127)
            {
                if (d_im[y][x] > 127)
                {
                    r_im[y1][x1] = 255;
                    s++;
                } else {
                    r_im[y1][x1] = 0;
                }
            } else if (im < 127) {
                r_im[y1][x1] = 0;
            } else {
                r_im[y1][x1] = 255;
                s++;
            }
            n++;
            im = d_im[y][x];
            im += d_im[yp][x];
            im += d_im[y][xm];
            im += d_im[yp][xm];
            im += er;
            im /= 4;
            if (im == 127)
            {
                if (d_im[y][x] > 127)
                {
                    r_im[y2][x1] = 255;
                    s++;
                } else {
                    r_im[y2][x1] = 0;
                }
            } else if (im < 127) {
                r_im[y2][x1] = 0;
            } else {
                r_im[y2][x1] = 255;
                s++;
            }
            n++;
            im = d_im[y][x];
            im += d_im[ym][x];
            im += d_im[y][xp];
            im += d_im[ym][xp];
            im += er;
            im /= 4;
            if (im == 127)
            {
                if (d_im[y][x] > 127)
                {
                    r_im[y1][x2] = 255;
                    s++;
                } else {
                    r_im[y1][x2] = 0;
                }
            } else if (im < 127) {
                r_im[y1][x2] = 0;
            } else {
                r_im[y1][x2] = 255;
                s++;
            }
            n++;
            im = d_im[y][x];
            im += d_im[yp][x];
            im += d_im[y][xp];
            im += d_im[yp][xp];
            im += er;
            im /= 4;
            if (im == 127)
            {
                if (d_im[y][x] > 127)
                {
                    r_im[y2][x2] = 255;
                    s++;
                } else {
                    r_im[y2][x2] = 0;
                }
            } else if (im < 127) {
                r_im[y2][x2] = 0;
            } else {
                r_im[y2][x2] = 255;
                s++;
            }
            n++;
        }
    }
    threshold = s * 256 / n;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterSBWReduce2 (BYTE** d_im, BYTE** r_im, unsigned height, unsigned width, unsigned height2, unsigned width2)
{
    unsigned y, x, y1, x1, y2, x2, im, n, s, threshold = 0;
    int er;

    n = 0;
    s = 0;
    er = 0;
    for (y = 0; y < height2; y++ )
    {
        y1 = 2 * y;
        if (y1 >= height) {y1 = height -1;}
        y2 = y1 + 1;
        if (y2 >= height) {y2 = height -1;}
        for (x = 0; x < width2; x++)
        {
            x1 = 2 * x;
            if (x1 >= width) {x1 = width -1;}
            x2 = x1 + 1;
            if (x2 >= width) {x2 = width -1;}
            im = d_im[y1][x1];
            im += d_im[y2][x1];
            im += d_im[y1][x2];
            im += d_im[y2][x2];
            im /= 255;
            n++;
            if (im == 2)
            {
                if ((im + er) < 3) {
                    r_im[y][x] = 0;
                    s++;
                    er = im;
                } else {
                    r_im[y][x] = 255;
                    s++;
                    er = -im;
                }
            } else if (im < 2) {
                r_im[y][x] = 0;
                er = im;
            } else {
                r_im[y][x] = 255;
                s++;
                er = -im;
            }
        }
    }
    threshold = s * 256 / n;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSGsample (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width)
{
    unsigned d;
    double xFactor = (double)width / new_width;
    double yFactor = (double)height / new_height;
    double y0, x0, yn, xn, y0t, x0t, ynt, xnt, yt, xt, dyt, dxt, fyt, fxt, s, ss, sim, gy, gx, gc;
    int y, x, y0i, x0i, yni, xni, i, j, yti, xti, ypr, xpr, ynx, xnx;
    int h = height;
    int w = width;
    int h2 = new_height;
    int w2 = new_width;

    for (y = 0; y < h2; y++)
    {
        y0 = y;
        y0 *= yFactor;
        y0i = (int)y0;
        yn = y0 + yFactor;
        yni = (int)yn;
        if (double(yni) < yn) {yni++;}
        for (x = 0; x < w2; x++)
        {
            x0 = x;
            x0 *= xFactor;
            x0i = (int)x0;
            xn = x0 + xFactor;
            xni = (int)xn;
            if (double(xni) < xn) {xni++;}
            for (d = 0; d < 3; d++)
            {
                sim = 0;
                ss = 0;
                for (i = y0i; i < yni; i++)
                {
                    y0t = i;
                    if (y0t < y0) {y0t = y0;}
                    ynt = i + 1;
                    if (ynt > yn) {ynt = yn;}
                    yt = (y0t + ynt) / 2;
                    yti = (int)yt;
                    if (yti >= h) {yti = h - 1;}
                    dyt = ynt - y0t;
                    fyt = yt - yti - 0.5;
                    ypr = yti - 1;
                    if (ypr < 0) {ypr = 0;}
                    ynx = yti + 1;
                    if (ynx >= h) {ynx = h - 1;}
                    for (j = x0i; j < xni; j++)
                    {
                        x0t = j;
                        if (x0t < x0) {x0t = x0;}
                        xnt = j + 1;
                        if (xnt > xn) {xnt = xn;}
                        xt = (x0t + xnt) / 2;
                        xti = (int)xt;
                        if (xti >= w) {xti = w - 1;}
                        dxt = xnt - x0t;
                        fxt = xt - xti - 0.5;
                        xpr = xti - 1;
                        if (xpr < 0) {xpr = 0;}
                        xnx = xti + 1;
                        if (xnx >= w) {xnx = w - 1;}
                        gy = p_im[ynx][xti].c[d];
                        gy += p_im[ynx][xti].c[d];
                        gy += p_im[ynx][xnx].c[d];
                        gy += p_im[ynx][xpr].c[d];
                        gy -= p_im[ypr][xti].c[d];
                        gy -= p_im[ypr][xti].c[d];
                        gy -= p_im[ypr][xnx].c[d];
                        gy -= p_im[ypr][xpr].c[d];
                        gy /= 8.0;
                        gx = p_im[yti][xnx].c[d];
                        gx += p_im[yti][xnx].c[d];
                        gx += p_im[ynx][xnx].c[d];
                        gx += p_im[ypr][xnx].c[d];
                        gx -= p_im[yti][xpr].c[d];
                        gx -= p_im[yti][xpr].c[d];
                        gx -= p_im[ynx][xpr].c[d];
                        gx -= p_im[ypr][xpr].c[d];
                        gx /= 8.0;
                        gc = p_im[yti][xti].c[d];
                        gc += fyt * gy;
                        gc += fxt * gx;
                        s = dxt * dyt;
                        ss += s;
                        sim += (gc * s);
                    }
                }
                if (ss != 0.0) {sim /= ss;}
                d_im[y][x].c[d] = (BYTE)MIN(MAX((int)0, (int)(sim + 0.5)), (int)255);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSHRIS (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int smode)
{
    unsigned d;
    int y, x, y2, x2, yp1, xp1, yp2, xp2, yn1, xn1, yn2, xn2;
    int h = height;
    int w = width;
    int h2 = h * smode;
    int w2 = w * smode;

    double imx;
    double b11, b12, b13, b21, b22, b23, b31, b32, b33;
    double r11, r12, r13, r21, r22, r23, r31, r32, r33;

    double k0[2];
    double k1[3];
    double k2[3];

    if (smode < 2) {smode = 2;}
    if (smode > 3) {smode = 3;}

    if (smode == 2)
    {
        k0[0] = 0.750;
        k0[1] = 0.250;
        k1[0] = k0[0] * k0[0];
        k1[1] = k0[0] * k0[1];
        k1[2] = k0[1] * k0[1];
        k2[0] = k1[0];
        k2[1] = k1[1] / 2;
        k2[2] = k1[2] / 4;
    } else {
        k0[0] = 2.0 / 3.0;
        k0[1] = 1.0 / 3.0;
        k1[0] = k0[0] * k0[0];
        k1[1] = k0[0] * k0[1];
        k1[2] = k0[1] * k0[1];
        k2[0] = (1 + 4 * k0[0] + 4 * k1[0]) / 9;
        k2[1] = (k0[1] + 2 * k1[1]) / 9;
        k2[2] = k1[2] / 9;
    }

    for (y = 0; y < h; y++)
    {
        yp1 = y - 1; if (yp1 < 0) {yp1 = 0;}
        yp2 = y - 2; if (yp2 < 0) {yp2 = 0;}
        yn1 = y + 1; if (yn1 > h - 1) {yn1 = h - 1;}
        yn2 = y + 2; if (yn2 > h - 1) {yn2 = h - 1;}
        for (x = 0; x < w; x++)
        {
            xp1 = x - 1; if (xp1 < 0) {xp1 = 0;}
            xp2 = x - 2; if (xp2 < 0) {xp2 = 0;}
            xn1 = x + 1; if (xn1 > w - 1) {xn1 = w - 1;}
            xn2 = x + 2; if (xn2 > w - 1) {xn2 = w - 1;}
            for (d = 0; d < 3; d++)
            {
                imx = (double)p_im[yp2][xp2].c[d];
                  b11 = -(k2[2] * imx);
                imx = (double)p_im[yp2][xp1].c[d];
                  b11 -= (k2[1] * imx);
                  b12 = -(k2[2] * imx);
                imx = (double)p_im[yp2][x].c[d];
                  b11 -= (k2[2] * imx);
                  b12 -= (k2[1] * imx);
                  b13 = -(k2[2] * imx);
                imx = (double)p_im[yp2][xn1].c[d];
                  b12 -= (k2[2] * imx);
                  b13 -= (k2[1] * imx);
                imx = (double)p_im[yp2][xn2].c[d];
                  b13 -= (k2[2] * imx);
                imx = (double)p_im[yp1][xp2].c[d];
                  b11 -= (k2[1] * imx);
                  b21 = -(k2[2] * imx);
                imx = (double)p_im[yp1][xp1].c[d];
                  b11 += ((2.0 - k2[0]) * imx);
                  b12 -= (k2[1] * imx);
                  b21 -= (k2[1] * imx);
                  b22 = -(k2[2] * imx);
                imx = (double)p_im[yp1][x].c[d];
                  b11 -= (k2[1] * imx);
                  b12 += ((2.0 - k2[0]) * imx);
                  b13 -= (k2[1] * imx);
                  b21 -= (k2[2] * imx);
                  b22 -= (k2[1] * imx);
                  b23 = -(k2[2] * imx);
                imx = (double)p_im[yp1][xn1].c[d];
                  b12 -= (k2[1] * imx);
                  b13 += ((2.0 - k2[0]) * imx);
                  b22 -= (k2[2] * imx);
                  b23 -= (k2[1] * imx);
                imx = (double)p_im[yp1][xn2].c[d];
                  b13 -= (k2[1] * imx);
                  b23 -= (k2[2] * imx);
                imx = (double)p_im[y][xp2].c[d];
                  b11 -= (k2[2] * imx);
                  b21 -= (k2[1] * imx);
                  b31 = -(k2[2] * imx);
                imx = (double)p_im[y][xp1].c[d];
                  b11 -= (k2[1] * imx);
                  b12 -= (k2[2] * imx);
                  b21 += ((2.0 - k2[0]) * imx);
                  b22 -= (k2[1] * imx);
                  b31 -= (k2[1] * imx);
                  b32 = -(k2[2] * imx);
                imx = (double)p_im[y][x].c[d];
                  b11 -= (k2[2] * imx);
                  b12 -= (k2[1] * imx);
                  b13 -= (k2[2] * imx);
                  b21 -= (k2[1] * imx);
                  b22 += ((2.0 - k2[0]) * imx);
                  b23 -= (k2[1] * imx);
                  b31 -= (k2[2] * imx);
                  b32 -= (k2[1] * imx);
                  b33 = -(k2[2] * imx);
                imx = (double)p_im[y][xn1].c[d];
                  b12 -= (k2[2] * imx);
                  b13 -= (k2[1] * imx);
                  b22 -= (k2[1] * imx);
                  b23 += ((2.0 - k2[0]) * imx);
                  b32 -= (k2[2] * imx);
                  b33 -= (k2[1] * imx);
                imx = (double)p_im[y][xn2].c[d];
                  b13 -= (k2[2] * imx);
                  b23 -= (k2[1] * imx);
                  b33 -= (k2[2] * imx);
                imx = (double)p_im[yn1][xp2].c[d];
                  b21 -= (k2[2] * imx);
                  b31 -= (k2[1] * imx);
                imx = (double)p_im[yn1][xp1].c[d];
                  b21 -= (k2[1] * imx);
                  b22 -= (k2[2] * imx);
                  b31 += ((2.0 - k2[0]) * imx);
                  b32 -= (k2[1] * imx);
                imx = (double)p_im[yn1][x].c[d];
                  b21 -= (k2[2] * imx);
                  b22 -= (k2[1] * imx);
                  b23 -= (k2[2] * imx);
                  b31 -= (k2[1] * imx);
                  b32 += ((2.0 - k2[0]) * imx);
                  b33 -= (k2[1] * imx);
                imx = (double)p_im[yn1][xn1].c[d];
                  b22 -= (k2[2] * imx);
                  b23 -= (k2[1] * imx);
                  b32 -= (k2[1] * imx);
                  b33 += ((2.0 - k2[0]) * imx);
                imx = (double)p_im[yn1][xn2].c[d];
                  b23 -= (k2[2] * imx);
                  b33 -= (k2[1] * imx);
                imx = (double)p_im[yn2][xp2].c[d];
                  b31 -= (k2[2] * imx);
                imx = (double)p_im[yn2][xp1].c[d];
                  b31 -= (k2[1] * imx);
                  b32 -= (k2[2] * imx);
                imx = (double)p_im[yn2][x].c[d];
                  b31 -= (k2[2] * imx);
                  b32 -= (k2[1] * imx);
                  b33 -= (k2[2] * imx);
                imx = (double)p_im[yn2][xn1].c[d];
                  b32 -= (k2[2] * imx);
                  b33 -= (k2[1] * imx);
                imx = (double)p_im[yn2][xn2].c[d];
                  b33 -= (k2[2] * imx);

                if (smode == 2)
                {
                    r11 = (k1[0] * b22 + k1[1] * (b12 + b21) + k1[2] * b11);
                    r12 = (k1[0] * b22 + k1[1] * (b12 + b23) + k1[2] * b13);
                    r21 = (k1[0] * b22 + k1[1] * (b32 + b21) + k1[2] * b31);
                    r22 = (k1[0] * b22 + k1[1] * (b32 + b23) + k1[2] * b33);

                    y2 = y * 2;
                    x2 = x * 2;

                    d_im[y2][x2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r11 + 0.5)), (int)255);
                    d_im[y2][x2 + 1].c[d] = (BYTE)MIN(MAX((int)0, (int)(r12 + 0.5)), (int)255);
                    d_im[y2 + 1][x2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r21 + 0.5)), (int)255);
                    d_im[y2 + 1][x2 + 1].c[d] = (BYTE)MIN(MAX((int)0, (int)(r22 + 0.5)), (int)255);
                } else {
                    r11 = (k1[0] * b22 + k1[1] * (b12 + b21) + k1[2] * b11);
                    r12 = (k0[0] * b22 + k0[1] * b12);
                    r13 = (k1[0] * b22 + k1[1] * (b12 + b23) + k1[2] * b13);
                    r21 = (k0[0] * b22 + k0[1] * b21);
                    r22 = p_im[y][x].c[d];
                    r23 = (k0[0] * b22 + k0[1] * b23);
                    r31 = (k1[0] * b22 + k1[1] * (b32 + b21) + k1[2] * b31);
                    r32 = (k0[0] * b22 + k0[1] * b32);
                    r33 = (k1[0] * b22 + k1[1] * (b32 + b23) + k1[2] * b33);

                    y2 = y * 3;
                    x2 = x * 3;

                    d_im[y2][x2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r11 + 0.5)), (int)255);
                    d_im[y2][x2 + 1].c[d] = (BYTE)MIN(MAX((int)0, (int)(r12 + 0.5)), (int)255);
                    d_im[y2][x2 + 2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r13 + 0.5)), (int)255);
                    d_im[y2 + 1][x2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r21 + 0.5)), (int)255);
                    d_im[y2 + 1][x2 + 1].c[d] = (BYTE)MIN(MAX((int)0, (int)(r22 + 0.5)), (int)255);
                    d_im[y2 + 1][x2 + 2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r23 + 0.5)), (int)255);
                    d_im[y2 + 2][x2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r31 + 0.5)), (int)255);
                    d_im[y2 + 2][x2 + 1].c[d] = (BYTE)MIN(MAX((int)0, (int)(r32 + 0.5)), (int)255);
                    d_im[y2 + 2][x2 + 2].c[d] = (BYTE)MIN(MAX((int)0, (int)(r33 + 0.5)), (int)255);
                }
            }
        }
    }
    for (y = 0; y < h2; y++)
    {
        for (x = 0; x < w2; x++)
        {
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSReduce (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int smode)
{
    unsigned d;
    int yr, xr, y1, x1, y2, x2, y3, x3;
    int h = height;
    int w = width;
    int h2, w2;

    double imx, imr;

    if (smode < 2) {smode = 2;}
    if (smode > 3) {smode = 3;}

    h2 = (height + smode - 1) / smode;
    w2 = (width + smode - 1) / smode;

    if (smode == 2)
    {
        for (yr = 0; yr < h2; yr++)
        {
            y1 = 2 * yr;
            y2 = 2 * yr + 1;
            if (y2 >= h) {y2 = h - 1;}
            for (xr = 0; xr < w2; xr++)
            {
                x1 = 2 * xr;
                x2 = 2 * xr + 1;
                if (x2 >= w) {x2 = w - 1;}
                for (d = 0; d < 3; d++)
                {
                    imr = 0;
                    imx = (double)p_im[y1][x1].c[d];
                    imr += imx;
                    imx = (double)p_im[y1][x2].c[d];
                    imr += imx;
                    imx = (double)p_im[y2][x1].c[d];
                    imr += imx;
                    imx = (double)p_im[y2][x2].c[d];
                    imr += imx;
                    imr /= 4.0;
                    d_im[yr][xr].c[d] = (BYTE)MIN(MAX((int)0, (int)(imr + 0.5)), (int)255);
                }
                d_im[yr][xr] = IMTcalcS (d_im[yr][xr]);
            }
        }
    } else {
        for (yr = 0; yr < h2; yr++)
        {
            y1 = 3 * yr;
            y2 = 3 * yr + 1;
            y3 = 3 * yr + 2;
            if (y2 >= h) {y2 = h - 1;}
            if (y3 >= h) {y3 = h - 2;}
            for (xr = 0; xr < w2; xr++)
            {
                x1 = 3 * xr;
                x2 = 3 * xr + 1;
                x3 = 3 * xr + 2;
                if (x2 >= w) {x2 = w - 1;}
                if (x3 >= w) {x3 = w - 2;}
                for (d = 0; d < 3; d++)
                {
                    imr = 0;
                    imx = (double)p_im[y1][x1].c[d];
                    imr += imx;
                    imx = (double)p_im[y1][x2].c[d];
                    imr += imx;
                    imx = (double)p_im[y1][x3].c[d];
                    imr += imx;
                    imx = (double)p_im[y2][x1].c[d];
                    imr += imx;
                    imx = (double)p_im[y2][x2].c[d];
                    imr += imx;
                    imx = (double)p_im[y2][x3].c[d];
                    imr += imx;
                    imx = (double)p_im[y3][x1].c[d];
                    imr += imx;
                    imx = (double)p_im[y3][x2].c[d];
                    imr += imx;
                    imx = (double)p_im[y3][x3].c[d];
                    imr += imx;
                    imr /= 9.0;
                    d_im[yr][xr].c[d] = (BYTE)MIN(MAX((int)0, (int)(imr + 0.5)), (int)255);
                }
                d_im[yr][xr] = IMTcalcS (d_im[yr][xr]);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSNearest (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned new_height, unsigned new_width)
{
    double xFactor = (double) width / new_width;
    double yFactor = (double) height / new_height;
    int y, x, ox, oy;
    int h = new_height;
    int w = new_width;

    for (y = 0; y < h; y++)
    {
        oy = (int)(y * yFactor);
        for (x = 0; x < w; x++)
        {
            ox = (int)(x * xFactor);
            d_im[y][x] = p_im[oy][ox];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBernsen(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, unsigned contrast_limit)
{
    int x, y, i, j, xt, yt;
    unsigned im, t;
    unsigned confused, c, minimum, maximum;
    BYTE val;
    int h = height;
    int w = width;
    int threshold = 0, st = 0, sn = 0;

    if (radius < 0) {radius = -radius;}

    confused = 255; // white

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            minimum = 768;
            maximum = 0;
            for (i = -radius; i <= radius; i++)
            {
                yt = y + i;
                if (yt >= 0 && yt < h)
                {
                    for (j = -radius; j <= radius; j++)
                    {
                        xt = x + j;
                        if (xt >= 0 && xt < w)
                        {
                            im = p_im[yt][xt].s;
                            minimum = MIN(minimum, im);
                            maximum = MAX(maximum, im);
                        }
                    }
                }
            }
            c = maximum - minimum;
            if (c < contrast_limit * 3)
            {
                val = (BYTE)confused;
            } else {
                t = (maximum + minimum) / 2;
                st += t;
                sn++;
                im = p_im[y][x].s;
                if (im >= t)
                {
                    val = 255; // white
                } else {
                    val = 0; // black
                }
            }
            d_im[y][x] = val;
        }
    }
    if (sn > 0) {threshold =  st / sn;}

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTAbutaleb (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius)
{
    unsigned x, y, i, n, st = 0, sn = 0;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm;
    int h = height;
    int w = width;
    unsigned a, b, s, t;
    BYTE val;
    int hw = 256;    // square histogram width
    int hist_size = hw*hw*sizeof(double);
    double one_over_area = 1.0 / (width * height);
    double P_sum, p, H_sum, H_end, Phi, P, H;
    double Phi_max = 1;
    double Phi_max_sub = 1;
    double tiny = 1e-6;
    unsigned threshold = 0, avg_threshold = 0;

    double* histogram = (double*)malloc(hist_size);
    double* P_histogram = (double*)malloc(hist_size);
    double* H_histogram = (double*)malloc(hist_size);

    memset(histogram,0,hist_size);
    memset(P_histogram,0,hist_size);
    memset(H_histogram,0,hist_size);

    if (radius < 0) {radius = -radius;}

    for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    n++;
                }
            }
            if (n > 0) {imm /= n;}
            imm /= 3;
            imx = (double)p_im[y][x].s;
            imx /= 3;
            a = (unsigned)imx;
            b = (unsigned)imm;
            i = (a * hw + b);
            histogram[i]++;
        }
    }

    for (b = 0; b < 256; b++)
    {
        for (a = 0; a < 256; a++)
        {
            i = (a * hw + b);
            histogram[i] = histogram[i] * one_over_area;
        }
    }
    P_sum = 0.0;
    for (s = 0; s < 256; s++)
    {
        i = s * hw;
        P_sum += histogram[i];
        P_histogram[i] = P_sum;
    }
    for (t = 1; t < 256; t++)
    {
        P_sum = 0.0;
        for (s = 0; s < 256; s++)
        {
            i = s * hw + t;
            P_sum += histogram[i];
            P_histogram[i] = P_histogram[i - 1] + P_sum;
        }
    }
    H_sum = 0.0;
    for (s = 0; s < 256; s++)
    {
        i = s * hw;
        p = histogram[i];
        if (p != 0) {H_sum -= (p * log(p));}
        H_histogram[i] = H_sum;
    }
    for (t = 1; t < 256; t++)
    {
        H_sum = 0.0;
        for (s = 0; s < 256; ++s)
        {
            i = s * hw + t;
            p = histogram[i];
            if (p != 0) {H_sum -= (p * log(p));}
            H_histogram[i] = H_histogram[i - 1] + H_sum;
        }
    }

    while (1 + Phi_max_sub > 1)
    {
        Phi_max = Phi_max_sub;
        Phi_max_sub = Phi_max / 10;
    }
    H_end = H_histogram[255*hw+255];
    threshold = 0;
    avg_threshold = 0;

    for (s = 0; s < 256; s++)
    {
        for (t = 0; t < 256; t++)
        {
            i = s * hw + t;
            P = P_histogram[i];
            H = H_histogram[i];
            if ((P > tiny) && ((1.0 - P) > tiny))
            {
                Phi = log(P * (1.0 - P)) + H / P + (H_end - H) / (1.0 - P);
                if (Phi > Phi_max)
                {
                    Phi_max = Phi;
                    threshold = s;
                    avg_threshold = t;
                }
            }
        }
    }

    for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    n++;
                }
            }
            if (n > 0) {imm /= n;}
            imm /= 3;
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imx <= (double)threshold && imm <= (double)avg_threshold)
            {
                val = 0;
            } else {
                val = 255;
                if (imx > (double)threshold) {st += threshold; sn++;}
                if (imm > (double)avg_threshold) {st += avg_threshold; sn++;}
            }
            d_im[y][x] = val;
        }
    }
    if (sn > 0) {threshold = st * 3 / sn;}

    free(histogram);
    free(P_histogram);
    free(H_histogram);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBHT (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned y, x;
    int i, im, il, ir;
    il = 0;
    ir = 767;
    double wl = 0, wr = 0;
    BYTE val;
    int threshold = 384;
    double histogram[768] = {0};

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            histogram[im]++;
        }
    }

    for (i = 0; i < 768; i++)
    {
        histogram[i] /= (double)(width * height);
    }

    for (i = il; i <= threshold; i++)
    {
        wl += histogram[i];
    }
    for (i = threshold + 1; i <= ir; i++)
    {
        wr += histogram[i];
    }

    while (il <= ir)
    {
        threshold = (il + ir) / 2;
        if (wr>wl)
        {
            wr -= histogram[ir];
            ir--;
            wr += histogram[threshold] /2.0;
            wl -= histogram[threshold] /2.0;
        } else {
            wl -= histogram[il];
            il++;
            wl += histogram[threshold + 1] /2.0;
            wr -= histogram[threshold + 1] /2.0;
        }
    }

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ( ( im >= threshold ) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiMod (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, im;
    double T, Tw, Tb, Tn, iw, ib;
    BYTE val;
    int threshold = 0;

    T = 384.0;
    Tn = 0;
    while ( T != Tn )
    {
        Tn = T;
        Tb = 0;
        Tw = 0;
        ib = 0;
        iw = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                im = p_im[y][x].s;
                if ( im > T)
                {
                    Tw += im;
                    iw++;
                } else {
                    Tb += im;
                    ib++;
                }
            }
        }
        if (iw == 0 && ib == 0)
        {
            T = Tn;
        } else if (iw == 0) {
            T = Tb/ib;
        } else if (ib == 0) {
            T = Tw/iw;
        } else {
            T = ((Tw/iw) + (Tb/ib)) / 2.0;
        }
    }
    T += delta;
    threshold = (int)(T + 0.5);
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im >= T) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiModC (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, d, im, ims;
    double T, Tw, Tb, Tn, iw, ib;
    unsigned thres[3];
    BYTE val;
    int threshold = 0;

    for (d = 0; d < 3; d++)
    {
        thres[d] = 0;
        T = 127.5;
        Tn = 0;
        while ( T != Tn )
        {
            Tn = T;
            Tb = 0;
            Tw = 0;
            ib = 0;
            iw = 0;
            for (y = 0; y < height; y++ )
            {
                for (x = 0; x < width; x++)
                {
                    im = p_im[y][x].c[d];
                    if ( im > T)
                    {
                        Tw += im;
                        iw++;
                    } else {
                        Tb += im;
                        ib++;
                    }
                }
            }
            if (iw == 0 && ib == 0)
            {
                T = Tn;
            } else if (iw == 0) {
                T = Tb/ib;
            } else if (ib == 0) {
                T = Tw/iw;
            } else {
                T = ((Tw/iw) + (Tb/ib)) / 2.0;
            }
        }
        T += delta;
        if (T < 0) {T = 0;}
        if (T > 255) {T = 255;}
        thres[d] = (int)T;
    }
    threshold = 0;
    for (d = 0; d < 3; d++)
    {
        threshold += thres[d];
    }
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            ims = 0;
            for (d = 0; d < 3; d++)
            {
                im = p_im[y][x].c[d];
                ims += ((im >= thres[d]) ? 255 : 0);
            }
            val = (BYTE) ((ims >= 384) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTColor (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x;
    int threshold;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            p_im[y][x] = IMTdiffS (p_im[y][x]);
        }
    }
    threshold = IMTFilterTBiMod(p_im, d_im, height, width, delta);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTChistian (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imMg, imVg, imm, imv, t, st = 0, sn = 0;
    int threshold = 0;
    int h = height;
    int w = width;
    BYTE val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }

    imMg = 0;
    imVg = 0;
    n = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (double)p_im[y][x].s;
            imMg += imx;
            imVg += (imx * imx);
            n++;
        }
    }
    if (n == 0) {n = 1;}
    imMg /= n;
    imMg /= 3;
    imVg /= n;
    imVg /= 9;
    imVg -= (imMg * imMg);
    if (imVg < 0) {imVg = -imVg;}
    imVg = sqrt(imVg);
    if (imVg == 0) {imVg = 1;}
    for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            imv = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    imv += (imx * imx);
                    n++;
                }
            }
            if (n == 0) {n = 1;}
            imm /= n;
            imm /= 3;
            imv /= n;
            imv /= 9;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imx < lower_bound)
            {
                t = lower_bound;
                val = 0;
            } else if (imx > upper_bound)
            {
                t = upper_bound;
                val = 255;
            } else {
                t = (imm + sensitivity * (imMg - imm + (imv / imVg) * (imm - imMg)) + delta);
                val = (BYTE) ((imx >= t) ? 255 : 0);
            }
            st += t;
            sn++;
            d_im[y][x] = val;
        }
    }
    if (sn == 0) { sn = 1;}
    threshold =  (st * 3.0 / sn);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTDalg (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int region_size, int delta)
{
    unsigned x, y, i, j;

    unsigned whg, wwn, iy0, ix0, iyn, ixn, tx, tt;
    unsigned  wwidth = region_size;
    double sw, swt, dsr, dst;
    BYTE val;
    int histogram[768] = {0};
    int threshold = 0;

    whg = (height + wwidth - 1) / wwidth;
    wwn = (width + wwidth-1) / wwidth;
    for (y = 0; y < whg; y++)
    {
        iy0 = y * wwidth;
        iyn = iy0 + wwidth;
        if (iyn > height) {iyn = height;}
        for (x = 0; x < wwn; x++)
        {
            ix0 = x * wwidth;
            ixn = ix0 + wwidth;
            if (ixn > width) {ixn = width;}
            sw = 0;
            for (j = iy0; j < iyn; j++)
            {
                for (i = ix0; i < ixn; i++)
                {
                    tx = p_im[j][i].s;
                    sw += tx;
                }
            }
            tt = 767;
            swt = 0;
            if (sw > 0)
            {
                for ( i = 0; i < 768; i++ )
                {
                    histogram[i] = 0;
                }
                for (j = iy0; j < iyn; j++)
                {
                    for (i = ix0; i < ixn; i++)
                    {
                        tx = p_im[j][i].s;
                        histogram[tx]++;
                    }
                }
                while ( swt < sw && tt > 0)
                {
                    dsr = sw - swt;
                    swt += (histogram[tt]*767);
                    dst = swt - sw;
                    tt--;
                }
                if (dst > dsr) {tt++;}
            }
            tt += delta;
            for (j = iy0; j < iyn; j++)
            {
                for (i = ix0; i < ixn; i++)
                {
                    tx = p_im[j][i].s;
                    val = (BYTE) ( ( tx > tt ) ? 255 : 0 );
                    d_im[j][i] = val;
                }
            }
            threshold += tt;
        }
    }
    threshold /= whg;
    threshold /= wwn;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTDither (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned x, y, dx, dy, i, j, l, m, n;
    unsigned blw, blh, s = 0, sn = 0;
    int threshold = 383;

    int dithermn[16] = {0};
    int dithermdy[16] = {0};
    int dithermdx[16] = {0};
    int erm[16] = {0};
    int val0, val;
    int erv, w;
    BYTE vali;
    static int ditherm[] = {
        14, 13, 1, 2,
        4, 6, 11, 9,
        0, 3, 15, 12,
        10, 8, 5, 7
    };


    blw = (width + 3)/4;
    blh = (height + 3)/4;
    for ( y = 0; y < 16; y++ )
    {
        x = ditherm[y];
        dithermn[x] = y;
        dy = y / 4;
        dx = y - dy * 4;
        dithermdy[x] = dy;
        dithermdx[x] = dx;
    }

    for ( i = 0; i < blh; i++ )
    {
        for ( j = 0; j < blw; j++)
        {
            for ( l = 0; l < 16; l++ )
            {
                erm[l] = 0;
            }
            for ( l = 0; l < 16; l++ )
            {
                m = dithermn[l];
                dy = dithermdy[l];
                y = i * 4 + dy;
                if ( y < height)
                {
                    dx = dithermdx[l];
                    x = j * 4 + dx;
                    if ( x < width)
                    {
                        val0 = p_im[y][x].s;
                        val0 += erm[l];
                        val = (( val0 >= threshold ) ? 255 : 0 );
                        erv = val0 - val * 3;
                        vali = (BYTE) (val);
                        if (val > 0) {s++;}
                        sn++;
                        d_im[y][x] = vali;
                        w = 0;
                        if ( dx > 0 )
                        {
                            n = ditherm[m - 1];
                            if (n > l) {w++; w++;}
                        }
                        if (dx < 3 && x < width)
                        {
                            n = ditherm[m + 1];
                            if (n > l) {w++; w++;}
                        }
                        if (dy > 0)
                        {
                            n = ditherm[m - 4];
                            if (n > l) {w++; w++;}
                            if (dx > 0)
                            {
                                n = ditherm[m - 5];
                                if (n > l) {w++;}
                            }
                            if (dx < 3 && x < width)
                            {
                                n = ditherm[m - 3];
                                if (n > l) {w++;}
                            }
                        }
                        if (dy < 3 && y < height)
                        {
                            n = ditherm[m + 4];
                            if (n > l) {w++; w++;}
                            if (dx > 0)
                            {
                                n = ditherm[m + 3];
                                if (n > l) {w++;}
                            }
                            if (dx < 3 && x < width)
                            {
                                n = ditherm[m + 5];
                                if (n > l) {w++;}
                            }
                        }
                        if (w > 0)
                        {
                            erv /= w;
                            if (dx > 0)
                            {
                                n = ditherm[m - 1];
                                if (n > l) {erm[n] += (2 * erv);}
                            }
                            if (dx < 3 && x < width)
                            {
                                n = ditherm[m + 1];
                                if (n > l) {erm[n] += (2 * erv);}
                            }
                            if (dy > 0)
                            {
                                n = ditherm[m - 4];
                                if (n > l) {erm[n] += (2 * erv);}
                                if (dx > 0)
                                {
                                    n = ditherm[m - 5];
                                    if (n > l) {erm[n] += erv;}
                                }
                                if (dx < 3 && x < width)
                                {
                                    n = ditherm[m - 3];
                                    if (n > l) {erm[n] += erv;}
                                }
                            }
                            if (dy < 3 && y < height)
                            {
                                n = ditherm[m + 4];
                                if (n > l) {erm[n] += (2 * erv);}
                                if (dx > 0)
                                {
                                    n = ditherm[m + 3];
                                    if (n > l) {erm[n] += erv;}
                                }
                                if (dx < 3 && x < width)
                                {
                                    n = ditherm[m + 5];
                                    if (n > l) {erm[n] += erv;}
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (sn > 0) {threshold = 765 * s / sn;}

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTDjVuL (IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, int wbmode, double anisotropic, double doverlay, unsigned fposter)
{
    unsigned widthbg = (width + bgs - 1) / bgs;
    unsigned heightbg = (height + bgs - 1) / bgs;
    unsigned widthfg = (widthbg + fgs - 1) / fgs;
    unsigned heightfg = (heightbg + fgs - 1) / fgs;
    unsigned whcp, y, x, d, l, i, j, y0, x0, y1, x1, y0b, x0b, y1b, x1b, yb, xb, yf, xf, blsz;
    double immean, imwb;
    BYTE fgbase, bgbase;
    unsigned cnth, cntw;
    IMTpixel pim, fgim, bgim;
    double fgdist, bgdist, kover, fgpart, bgpart, fgk = 1.0;
    unsigned maskbl, maskover, bgsover, fgsum[3], bgsum[3], fgnum, bgnum;

    IMTpixel** fgt_im;
    fgt_im = (IMTpixel**)malloc(heightbg * sizeof(IMTpixel*));
    for (y = 0; y < heightbg; y++) {fgt_im[y] = (IMTpixel*)malloc(widthbg * sizeof(IMTpixel));}

    whcp = height;
    whcp += width;
    whcp /= 2;
    blsz = 1;
    if (level == 0)
    {
        while (bgs * blsz < whcp)
        {
            level++;
            blsz *= 2;
        }
    } else {
        for (l = 0; l < level; l++)
        {
            blsz *= 2;
        }
    }
    immean = IMTmean(p_im, height, width);
    imwb = IMTwb(p_im, immean, height, width);
    if (anisotropic == 0)
    {
        fgk = sqrt(1.5 - imwb);
    }
    if (wbmode == 0)
    {
        if (imwb < 0.5) {wbmode = -1;} else {wbmode = 1;}
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
    if (doverlay < 0) {doverlay = 0;}
    kover = doverlay + 1.0;
    for (l = 0; l < level; l++)
    {
        cnth = (heightbg + blsz - 1) / blsz;
        cntw = (widthbg + blsz - 1) / blsz;
        maskbl = bgs * blsz;
        maskover = (kover * maskbl);
        bgsover = (kover * blsz);
        for (i = 0; i < cnth; i++)
        {
            y0 = i * maskbl;
            y1 = y0 + maskover;
            if (y1 > height) {y1 = height;}
            y0b = i * blsz;
            y1b = y0b + bgsover;
            if (y1b > heightbg) {y1b = heightbg;}
            for (j = 0; j < cntw; j++)
            {
                x0 = j * maskbl;
                x1 = x0 + maskover;
                if (x1 > width) {x1 = width;}
                x0b = j * blsz;
                x1b = x0b + bgsover;
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
                if (fgnum > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        fgsum[d] /= fgnum;
                        fgim.c[d] = fgsum[d];
                    }
                }
                if (bgnum > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        bgsum[d] /= bgnum;
                        bgim.c[d] = bgsum[d];
                    }
                }
                pim = IMTmeanIc(p_im, y0, x0, y1, x1);

                fgdist = IMTdist(pim, fgim);
                bgdist = IMTdist(pim, bgim);
                fgpart = 1.0;
                bgpart = 1.0;
                if ((fgdist + bgdist)> 0)
                {
                    fgpart += (2.0 * fgdist / (fgdist + bgdist));
                    bgpart += (2.0 * bgdist / (fgdist + bgdist));
                }
                fgim = IMTaverageIc(fgt_im, fgim, y0b, x0b, y1b, x1b, fgpart);
                bgim = IMTaverageIc(bg_im, bgim, y0b, x0b, y1b, x1b, bgpart);
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
    if (fposter != 0)
    {
        double imsh = IMTFilterPosterize(fg_im, heightfg, widthfg, fposter);
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

    return wbmode;
}

///////////////////////////////////////////////////////////////////////////////

int IMTFilterTEnt (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned y, x, i, t, im, cn = 768;
    double sum = 0, pTB, hhB, pTW, hhW, jMax, j;
    double histogram[768] = {0};
    double pT[768] = {0};
    double epsilon = 0;
    double hB[768] = {0};
    double hW[768] = {0};
    int threshold = 0;
    BYTE val;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            histogram[im]++;
        }
    }
    /*
    * DAVID
    *
    * For all i,
    *     normalizedHist[i] = (double) hist[i]/sum(hist)
    */
    // Normalize histogram, that is makes the sum of all bins equal to 1.
    for (i = 0; i < cn; ++i)
    {
        sum += histogram[i];
    }
    if (sum == 0)
    {
        return 384;
    }

    for (i = 0; i < cn; i++)
    {
        histogram[i] /= sum;
    }
    /*
    * DAVID
    *
    * pT = cumulative_sum(normalizedHist)
    */
    pT[0] = histogram[0];
    for (i = 1; i < cn; i++)
    {
        pT[i] = pT[i - 1] + histogram[i];
    }

    for (t = 0; t < cn; t++)
    {
        // Black entropy
        pTB = pT[t];        // DAVID
        if (pTB > epsilon)
        {
            hhB = 0;
            for (i = 0; i <= t; i++)
            {
                if (histogram[i] > epsilon)
                {
                    hhB -= histogram[i] / pTB * log(histogram[i] / pTB);
                }
            }
            hB[t] = hhB;
        } else {
            hB[t] = 0;
        }

        // White  entropy
        pTW = 1 - pT[t];
        if (pTW > epsilon)
        {
            hhW = 0;
            for (i = t + 1; i < cn; ++i)
            {
                if (histogram[i] > epsilon)
                {
                    hhW -= histogram[i] / pTW * log(histogram[i] / pTW);
                }
            }
            hW[t] = hhW;
        } else {
            hW[t] = 0;
        }
    }

    // Find histogram index with maximum entropy
    // DAVID: ...where entropy[i] is defined to be (black_entropy[i] + white_entropy[i])
    jMax = hB[0] + hW[0];

    for (t = 1; t < cn; ++t)
    {
        j = hB[t] + hW[t];
        if (j > jMax)
        {
            jMax = j;
            threshold = t;
        }
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) (((int)im >= threshold) ? 255 : 0);
            d_im[y][x] = val;
        }
    }

    return threshold;
}
////////////////////////////////////////////////////////////////////////////////

int IMTFilterTEqBright (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned x, y, i;

    unsigned tx, tt, threshold = 0;
    double imx, sw, swt, dsr = 0, dst = 0;
    int histogram[768] = {0};
    BYTE val;

    for ( i = 0; i < 768; i++ )
    {
        histogram[i] = 0;
    }
    sw = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            tx = p_im[y][x].s;
            sw += tx;
            histogram[tx]++;
        }
    }
    tt = 767;
    swt = 0;
    while ( swt < sw && tt > 0)
    {
        dsr = sw - swt;
        imx = histogram[tt];
        swt += (imx * 767);
        dst = swt - sw;
        tt--;
    }
    if (dst > dsr)
    {
        tt++;
        swt -= (imx*767);
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            tx = p_im[y][x].s;
            val = (BYTE) ( ( tx > tt ) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }
    threshold = tt;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterGatosBG (IMTpixel** p_im, BYTE** d_im, IMTpixel** bg_im, unsigned height, unsigned width, int radius)
{
    int x, y, i, j, xt, yt;
    unsigned d;
    double imx;
    double bin_sum;
    double p_sum[3] = {0.0};
    BYTE val;
    int h = height;
    int w = width;
    int threshold = 0, st = 0, sn = 0;

    if (radius < 0) {radius = -radius;}

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            if (d_im[y][x] != 0)
            { // white
                bg_im[y][x] = p_im[y][x];
            } else {
                for (d = 0; d < 3; d++)
                {
                    p_sum[d] = 0;
                }
                bin_sum = 0;
                for (i = -radius; i <= radius; i++)
                {
                    yt = y + i;
                    if (yt >= 0 && yt < h)
                    {
                        for (j = -radius; j <= radius; j++)
                        {
                            xt = x + j;
                            if (xt >= 0 && xt < w)
                            {
                                if (d_im[yt][xt] != 0) // white
                                {
                                    bin_sum++;
                                    for (d = 0; d < 3; d++)
                                    {
                                        imx = (double)p_im[yt][xt].c[d];
                                        p_sum[d] += imx;
                                    }
                                    imx = (double)p_im[yt][xt].s;
                                    p_sum[3] += imx;
                                }
                            }
                        }
                    }
                }
                if (bin_sum > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        imx = p_sum[d] / (double)bin_sum;
                        val = (BYTE)MIN(MAX((int)0, (int)(imx + 0.5)), (int)255);
                        bg_im[y][x].c[d] = val;
                    }
                    bg_im[y][x] = IMTcalcS (bg_im[y][x]);
                } else {
                    bg_im[y][x] = IMTset(255, 255, 255);
                    st++;
                }
            }
            sn++;
        }
    }

    if (sn > 0) {threshold = 768 * st / sn;}

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTGatos (IMTpixel** p_im, BYTE** d_im, IMTpixel** bg_im, BYTE** g_im, unsigned height, unsigned width, double q, double p1, double p2)
{
    unsigned x, y, t, s;
    double imx, imk;
    int sum = 0;
    unsigned delta_numerator = 0;
    unsigned delta_denominator = 0;
    int bin_sum = 0;
    double delta, b;
    BYTE val;
    int threshold = 0, st = 0, sn = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            t = bg_im[y][x].s;
            s = p_im[y][x].s;
            if (t > s)
            {
                delta_numerator += (t - s);
            }
        }
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (d_im[y][x] == 0)
            { // black
                delta_denominator++;
            }
        }
    }

    delta = (double)delta_numerator;
    delta /= (double)delta_denominator;
    delta /= 3;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if(d_im[y][x] != 0) // white
            {
                bin_sum++;
                sum += (double)(bg_im[y][x].s);
            }
        }
    }
    sum /= 3;

    b = sum / bin_sum;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (double)(p_im[y][x].s);
            imk = (double)(bg_im[y][x].s);
            imx /= 3;
            imk /= 3;
            imx = imk -imx;
            imk = q * delta * (((1 - p2) / (1 + exp(((-4 * imk) / (b * (1 - p1))) + ((2 * (1 + p1)) / (1 - p1))))) + p2);
            val = (BYTE) ((imx > imk) ? 0 : 255);
            if (val > 0) {st++;}
            g_im[y][x] = val;
            sn++;
        }
    }
    if (sn > 0) {threshold = 765 * st / sn;}

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTGrad (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int x, y, xp, xn, yp, yn;
    int im, imp, imn;
    double GX, GY, G, SG = 0, SGI = 0, SI = 0, SN = 0, S = 0;
    int threshold = 0;
    BYTE val;
    int h = height;
    int w = width;

    for (y = 0; y < h; y++)
    {
        yp = y -1;
        if (yp < 0){yp = 0;}
        yn = y + 1;
        if (yn >= h) {yn = h - 1;}
        for (x = 0; x < w; x++)
        {
            im = p_im[y][x].s;
            xp = x -1;
            if (xp < 0) {xp = 0;}
            xn = x + 1;
            if (xn >= w) {xn = w - 1;}
            imp = p_im[y][xp].s;
            imn = p_im[y][xn].s;
            GX = (double)(imn - imp);
            if (GX < 0){GX = -GX;}
            imp = p_im[yp][x].s;
            imn = p_im[yn][x].s;
            GY = (double)(imn - imp);
            if (GY < 0){GY = -GY;}
            G = sqrt(GX * GX + GY * GY);
            SG += G;
            SGI += (double(im)*G);
            SI += double(im);
            SN++;
        }
    }

    if (SG==0)
    {
        S = SI / SN;
    } else {
        S = SGI / SG;
    }

    threshold = (int)(S);

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ( ( im >= S ) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTHalftone2 (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned y, x, im, n, s, threshold = 0;

    n = 0;
    s = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            n++;
            if (im < 153)
            {
                d_im[2 * y][2 * x] = 0;
                d_im[2 * y + 1][2 * x] = 0;
                d_im[2 * y][2 * x + 1] = 0;
                d_im[2 * y + 1][2 * x + 1] = 0;
            } else if (im < 306) {
                d_im[2 * y][2 * x] = 255;
                d_im[2 * y + 1][2 * x] = 0;
                d_im[2 * y][2 * x + 1] = 0;
                d_im[2 * y + 1][2 * x + 1] = 0;
                s++;
            } else if (im < 459) {
                d_im[2 * y][2 * x] = 255;
                d_im[2 * y + 1][2 * x] = 0;
                d_im[2 * y][2 * x + 1] = 0;
                d_im[2 * y + 1][2 * x + 1] = 255;
                s++;
                s++;
            } else if (im < 612) {
                d_im[2 * y][2 * x] = 255;
                d_im[2 * y][2 * x + 1] = 255;
                d_im[2 * y + 1][2 * x] = 0;
                d_im[2 * y + 1][2 * x + 1] = 255;
                s++;
                s++;
                s++;
            } else {
                d_im[2 * y][2 * x] = 255;
                d_im[2 * y][2 * x + 1] = 255;
                d_im[2 * y + 1][2 * x] = 255;
                d_im[2 * y + 1][2 * x + 1] = 255;
                s++;
                s++;
                s++;
                s++;
            }
        }
    }
    threshold = s * 64 / n;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterThreshold (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x;
    BYTE val;
    int im, threshold = 0;

    threshold = 382 + delta;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im > threshold) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTJanni (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned y, x, i, cn = 768, im;
    double histogram[768] = {0};
    int gmin=0, gmax=256, gmid;
    double spg = 0;
    BYTE val;
    unsigned threshold = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            histogram[im]++;
        }
    }
    for (i = 0; i< cn; i++)
    {
        histogram[i] /= (double)(width * height);
    }

    i = 0;
    while (histogram[i]==0 && i < cn)
    {
        i++;
    }
    gmin = i;
    i = cn-1;
    while (histogram[i]==0 && i > 0)
    {
        i--;
    }
    gmax = i;

    gmid = (gmin+gmax)/2;

    for (i = (gmid + 1); (int)i <= gmax; i++)
    {
        spg += histogram[i];
    }
    threshold = gmin + (int)((gmax - gmin) * spg);
    threshold = (threshold + gmid)/2;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im >= threshold ) ? 255 : 0);
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTKMeans (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned knum, unsigned iters)
{
    unsigned knum2 = knum / 2;
    int y, x, i, j;
    double kptb, kptn;

    unsigned d, knm, ct, t, st = 0, sn = 0;
    double kdmin, cstep;
    unsigned imm[3] = {0};
    BYTE val;
    int h = height;
    int w = width;
    int threshold = 0;
    int k = 0;
    int dl, dls = 1;

    cstep = 255.0 / (knum - 1);

    double** kpt;
    kpt = (double**)malloc(knum * sizeof(double*));
    for (y = 0; y < (int)knum; y++) {kpt[y] = (double*)malloc(4 * sizeof(double));}
    double** ksum;
    ksum = (double**)malloc(knum * sizeof(double*));
    for (y = 0; y < (int)knum; y++) {ksum[y] = (double*)malloc(4 * sizeof(double));}
    double* kdist;
    kdist = (double*)malloc(knum * sizeof(double));

    for (i = 0; i < (int)knum; i++)
    {
        ct = (int)(cstep * i + 0.5);
        for (j = 0; j < 3; j++)
        {
            kpt[i][j] = ct;
        }
    }
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            for (d = 0; d < 3; d++)
            {
                imm[d] += p_im[y][x].c[d];
            }
        }
    }
    for (d = 0; d < 3; d++)
    {
        imm[d] /= height;
        imm[d] /= width;
    }

    while (dls > 0 && k < (int)iters)
    {
        for (i = 0; i < (int)knum; i++)
        {
            for (j = 0; j < 4; j++)
            {
                ksum[i][j] = 0;
            }
        }
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                for (i = 0; i < (int)knum; i++)
                {
                    kdist[i] = IMTdist3c2p(p_im[y][x], kpt[i]);
                }
                kdmin = 10;
                knm = 0;
                for (i = 0; i < (int)knum; i++)
                {
                    if (kdist[i] < kdmin)
                    {
                        kdmin = kdist[i];
                        knm = i;
                    }
                }
                for (j = 0; j < 3; j++)
                {
                    ksum[knm][j] += p_im[y][x].c[j];
                }
                ksum[knm][3]++;
            }
        }
        dls = 0;
        for (i = 0; i < (int)knum; i++)
        {
            if (ksum[i][3] > 0)
            {
                for (j = 0; j < 3; j++)
                {
                    kptb = kpt[i][j];
                    kptn = (ksum[i][j] / ksum[i][3]);
                    kptn = (kptb + kptn) / 2;
                    dl = kptb - kptn;
                    dl *= dl;
                    dls += dl;
                    kpt[i][j] = kptn;
                }
            }
            else
            {
                for (j = 0; j < 3; j++)
                {
                    kptb = kpt[i][j];
                    kptn = (kptb * iters + imm[j]) / (iters + 1);
                    dl = kptb - kptn;
                    dl *= dl;
                    dls += dl;
                    kpt[i][j] = kptn;
                }
            }
        }
        k++;
    }

    for (j = 0; j < 3; j++)
    {
        ksum[0][j] = 0;
        for (i = 0; i < (int)knum; i++)
        {
            ksum[0][j] += kpt[i][j];
        }
        ksum[0][j] /= knum;
    }
    kptb = 0;
    for (j = 0; j < 3; j++)
    {
        kptb += (kpt[knum2][j] - ksum[0][j]);
    }
    for (i = 0; i < (int)knum; i++)
    {
        kpt[i][3] = 0;
        for (j = 0; j < 3; j++)
        {
            kpt[i][3] += kpt[i][j];
        }
    }
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            for (i = 0; i < (int)knum; i++)
            {
                kdist[i] = IMTdist3c2p(p_im[y][x], kpt[i]);
            }
            kdmin = 10;
            knm = 0;
            for (i = 0; i < (int)knum; i++)
            {
                if (kdist[i] < kdmin)
                {
                    kdmin = kdist[i];
                    knm = i;
                }
            }
            if (knum - knum2 * 2 == 0)
            {
                val = (BYTE) ((knm >= knum2) ? 255 : 0);
                t = kpt[knum2][3];
                st += t;
                sn++;
            } else {
                if (knm == knum2)
                {
                    val = (BYTE) ((kptb >= 0) ? 255 : 0);
                } else {
                    val = (BYTE) ((knm >= knum2) ? 255 : 0);
                }
            }
            d_im[y][x] = val;
        }
    }
    if (sn > 0) {threshold = (st /sn);}

    free(kdist);
    for (y = 0; y < (int)knum; y++) {free(kpt[y]);}
    free(kpt);
    for (y = 0; y < (int)knum; y++) {free(ksum[y]);}
    free(ksum);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTNiblack (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, t, st = 0, sn = 0;
    int threshold = 0;
    int h = height;
    int w = width;
    BYTE val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }

    for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            imv = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    imv += (imx * imx);
                    n++;
                }
            }
            if (n == 0) {n = 1;}
            imm /= n;
            imm /= 3;
            imv /= n;
            imv /= 9;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imx < lower_bound)
            {
                t = lower_bound;
                val = 0;
            } else if (imx > upper_bound)
            {
                t = upper_bound;
                val = 255;
            } else {
                t = (imm + sensitivity * imv + delta);
                val = (BYTE) ((imx >= t) ? 255 : 0);
            }
            st += t;
            sn++;
            d_im[y][x] = val;
        }
    }
    if (sn == 0) {sn = 1;}
    threshold =  (st * 3.0 / sn);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTOtsu (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned im, y, x, i;
    double w = 0, u = 0, uT = 0;
    double  histmean = 0.0;
    double  work1, work2, work3 = 0.0;
    unsigned threshold = 0;
    BYTE val;
    double histogram[768] = {0};

    for (i = 0; i < 768; i++)
    {
        histogram[i] = 0;
    }
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            histogram[im]++;
        }
    }
    for (i = 0; i < 768; i++)
    {
        histogram[i] /= (double)(width * height);
        uT += (histogram[i] * (double)(i + 1));
    }

    histmean = uT / (double)(width * height);

    for (i = 0; i < 768; i++)
    {
        w += histogram[i];
        u += ((i + 1) * histogram[i]);
        work1 = (uT * w - u);
        work2 = (work1 * work1) / ( w * (1.0f-w) );

        if (work2>work3)
        {
            work3=work2;
            threshold = i;
        }
    }

    threshold -= 1;

    if(threshold == 0)
    {
        threshold = (int)(histmean/2.0);
    }

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ( ( im >= threshold ) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTQuadMod (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, im, nw, nb, n;
    double T, TG, Trw, Trb, Tw, Tb, Tn, iw, ib, sw, sb, s;
    BYTE val;
    int threshold = 0;

    T = 384.0;
    Tn = 0;
    while ( T != Tn )
    {
        Tn = T;
        Tb = 0;
        Tw = 0;
        ib = 0;
        iw = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                im = p_im[y][x].s;
                if ( im > T)
                {
                    Tw += im;
                    iw++;
                } else {
                    Tb += im;
                    ib++;
                }
            }
        }
        if (iw == 0 && ib == 0)
        {
            T = Tn;
        } else if (iw == 0) {
            T = Tb/ib;
        } else if (ib == 0) {
            T = Tw/iw;
        } else {
            T = ((Tw/iw) + (Tb/ib)) / 2.0;
        }
    }
    T += delta;
    TG = T;
    threshold = (int)(T + 0.5);
    sw = 0;
    nw = 0;
    sb = 0;
    nb = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im >= T) ? 255 : 0 );
            if (val == 0)
            {
                sb += im;
                nb++;
            } else {
                sw += im;
                nw++;
            }
            d_im[y][x] = val;
        }
    }
    if (nb > 0) {sb /= nb;}
    if (nw > 0) {sw /= nw;}
    T = sw;
    Tn = 0;
    while ( T != Tn )
    {
        Tn = T;
        Tb = 0;
        Tw = 0;
        ib = 0;
        iw = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                if (d_im[y][x] > 0)
                {
                    im = p_im[y][x].s;
                    if ( im > T)
                    {
                        Tw += im;
                        iw++;
                    } else {
                        Tb += im;
                        ib++;
                    }
                }
            }
        }
        if (iw == 0 && ib == 0)
        {
            T = Tn;
        } else if (iw == 0) {
            T = Tb/ib;
        } else if (ib == 0) {
            T = Tw/iw;
        } else {
            T = ((Tw/iw) + (Tb/ib)) / 2.0;
        }
    }
    Trw = T;
    T = sb;
    Tn = 0;
    while ( T != Tn )
    {
        Tn = T;
        Tb = 0;
        Tw = 0;
        ib = 0;
        iw = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                if (d_im[y][x] == 0)
                {
                    im = p_im[y][x].s;
                    if ( im > T)
                    {
                        Tw += im;
                        iw++;
                    } else {
                        Tb += im;
                        ib++;
                    }
                }
            }
        }
        if (iw == 0 && ib == 0)
        {
            T = Tn;
        } else if (iw == 0) {
            T = Tb/ib;
        } else if (ib == 0) {
            T = Tw/iw;
        } else {
            T = ((Tw/iw) + (Tb/ib)) / 2.0;
        }
    }
    Trb = T;
    s = 0;
    n = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            if (im >= Trb && im <= Trw)
            {
                s += im;
                n++;
            }
        }
    }
    if (n > 0) {s /= n;} else {s = 384.0;}
    T = s;
    Tn = 0;
    while ( T != Tn )
    {
        Tn = T;
        Tb = 0;
        Tw = 0;
        ib = 0;
        iw = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                if (d_im[y][x] > 0)
                {
                    im = p_im[y][x].s;
                    if ( im > T)
                    {
                        Tw += im;
                        iw++;
                    } else {
                        Tb += im;
                        ib++;
                    }
                }
            }
        }
        if (iw == 0 && ib == 0)
        {
            T = Tn;
        } else if (iw == 0) {
            T = Tb/ib;
        } else if (ib == 0) {
            T = Tw/iw;
        } else {
            T = ((Tw/iw) + (Tb/ib)) / 2.0;
        }
    }
    T += delta;
    T += TG;
    T /= 2;
    threshold = (int)(T + 0.5);

    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im >= T) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }
    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTRot (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, bool weight)
{
    unsigned y, x, i, im;
    unsigned isz = 768, il = 0, ir = isz - 1;
    double  wl = 0, wr = 0;
    double kw, wi;
    unsigned threshold = 0;
    BYTE val;
    double histogram[768] = {};
    double hists = 0;

    for (i = 0; i < isz; i++)
    {
        histogram[i] = 0;
    }
    for (y = 0; y < height; y++)
    {
        for ( x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            histogram[im]++;
        }
    }
    for (i = 0; i < isz; i++)
    {
        if (weight)
        {
            kw = (double)(isz);
            wi = 2*(kw-(double)(i))/kw;
            histogram[i] *= wi;
        }
        hists += histogram[i];
    }
    for (i = 0; i < isz; i++)
    {
        histogram[i] /= hists;
    }

    while (il < ir-1)
    {
        if (wl < wr)
        {
            il++;
            for (i = 0; i <= il; i++)
            {
                wl += histogram[i];
            }
        } else {
            ir--;
            for (i = ir; i < isz; i++)
            {
                wr += histogram[i];
            }
        }
    }

    threshold = il;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im >= threshold ) ? 255 : 0);
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTSauvola (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int dynamic_range, int lower_bound, int upper_bound, double delta)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, ima, t, st = 0, sn = 0;
    int threshold = 0;
    int h = height;
    int w = width;
    BYTE val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }

   for (y = 0; y < height; y++)
    {
        y1 = y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = y + 1;
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = x + 1;
            x2 += radius;
            if (x2 > w) {x2 = w;}
            imm = 0;
            imv = 0;
            n = 0;
            for (yf = y1; yf < y2; yf++)
            {
                for (xf = x1; xf < x2; xf++)
                {
                    imx = (double)p_im[yf][xf].s;
                    imm += imx;
                    imv += (imx * imx);
                    n++;
                }
            }
            if (n == 0) {n = 1;}
            imm /= n;
            imm /= 3;
            imv /= n;
            imv /= 9;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            ima = 1.0 - imv / (double)dynamic_range;
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imx < lower_bound)
            {
                t = lower_bound;
                val = 0;
            } else if (imx > upper_bound)
            {
                t = upper_bound;
                val = 255;
            } else {
                t = imm + (1.0 - sensitivity * ima) + delta;
                val = (BYTE) ((imx >= t) ? 255 : 0);
            }
            st += t;
            sn++;
            d_im[y][x] = val;
        }
    }
    if (sn == 0) {sn = 1;}
    threshold =  (st * 3.0 / sn);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTText (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned contour, unsigned radius)
{
    unsigned wr, ws, s, sn, n, z;
    int y, x, y0, x0, yk, xk, yf, xf, ys, xs, im, imf, ims, imd;
    BYTE val;
    int h = height;
    int w = width;

    WORD** b_im;
    b_im = (WORD**)malloc(height * sizeof(WORD*));
    for (y = 0; y < h; y++) {b_im[y] = (WORD*)malloc(width * sizeof(WORD));}

    wr = 2 * radius;
    ws = wr * wr - 1;

    int threshold = 0;

    z = 0;
    for (y = 0; y < h; y++)
    {
        y0 = y - radius;
        yk = y + radius;
        if (y0 < 0) {y0 = 0;}
        if (yk > h) {yk = h;}
        for (x = 0; x < w; x++)
        {
            x0 = x - radius;
            xk = x + radius;
            if (x0 < 0) {x0 = 0;}
            if (xk > w) {xk = w;}
            n = 0;
            im = p_im[y][x].s;
            for (yf = y0; yf < yk; yf++)
            {
                for (xf = x0; xf < xk; xf++)
                {
                    imf = p_im[yf][xf].s;
                    sn = 0;
                    s = 0;
                    for (ys = yf - 1; ys < yf + 2; ys++)
                    {
                        if (ys >= 0 && ys < h)
                        {
                            for (xs = xf - 1; xs < xf + 2; xs++)
                            {
                                if (xs >= 0 && xs < w)
                                {
                                    if (xs != xf && ys != yf)
                                    {
                                        ims = p_im[ys][xs].s;
                                        imd = imf - ims;
                                        if (imd < 0) {imd = -imd;}
                                        if (imd > (int)contour) {s++;}
                                        sn++;
                                    }
                                }
                            }
                        }
                    }
                    s *= 2;
                    if (sn > 0) {s /= sn;}
                    n += s;
                }
            }
            if (z < n) {z = n;}
            b_im[y][x] = n;
        }
    }
    z /= 2;
    threshold = (384 * z / ws);
    for (y = 0; y < h; y++ )
    {
        for (x = 0; x < w; x++)
        {
            im = b_im[y][x];
            val = (BYTE) ((im >= (int)z) ? 255 : 0);
            d_im[y][x] = val;
        }
    }

    for (y = 0; y < h; y++){free(b_im[y]);}
    free(b_im);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTTsai (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int shift)
{

    unsigned y, x, cn = 768, i, im;
    BYTE val;
    int threshold;
    int histogram[768] = {0};
    double criterion = 0.0;
    double m1, m2, m3;
    double cd, c0, c1, z0, z1, pd, p0, p1;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            histogram[im]++;
        }
    }

    m1 = m2 = m3 = 0.0;

    for (i = 0; i < cn; i++)
    {
        m1 += i * (double)histogram[i];
        m2 += i * i * (double)histogram[i];
        m3 += i * i * i * (double)histogram[i];
    }

    cd = m2 - m1 * m1;
    c0 = (-m2 * m2 + m1 * m3) / cd;
    c1 = (-m3 + m2 * m1) / cd;

    z0 = 0.5 * (-c1 - sqrt(c1 * c1 - 4.0 * c0));
    z1 = 0.5 * (-c1 + sqrt(c1 * c1 - 4.0 * c0));

    pd = z1 - z0;
    p0 = (z1 - m1) / pd;
    p1 = 1.0 - p0;

    for (threshold = 0; threshold < (int)cn; threshold++)
    {
        criterion += (double)histogram[threshold];
        if (criterion > p1) break;
    }

    if(threshold == 255) {threshold = 0;}

    threshold += shift * 3;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) (((int)im >= threshold) ? 255 : 0);
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTWhiteRohrer(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int x_lookahead, int y_lookahead, int bias_mode, int bias_factor, int f_factor, int g_factor)
{
    int x, y;
    int h = height;
    int w = width;
    BYTE val;

    int WR1_F_OFFSET = 255;
    int WR1_G_OFFSET = 255;
    int BIN_FOREGROUND = 0;
    int BIN_BACKGROUND = 255;
    int WR1_BIAS_CROSSOVER = 93;
    int WR1_BIAS = 20;
    double WR1_BLACK_BIAS_FACTOR = 0.0;
    double WR1_WHITE_BIAS_FACTOR = -0.25;
    int wr1_f_tab[512] = {
        -62,  -62,  -61,  -61,  -60,  -60,  -59,  -59,
            -58,  -58,  -57,  -57,  -56,  -56,  -54,  -54,
            -53,  -53,  -52,  -52,  -51,  -51,  -50,  -50,
            -49,  -49,  -48,  -48,  -47,  -47,  -46,  -46,
            -45,  -45,  -44,  -44,  -43,  -43,  -42,  -42,
            -41,  -41,  -41,  -41,  -40,  -40,  -39,  -39,
            -38,  -38,  -37,  -37,  -36,  -36,  -36,  -36,
            -35,  -35,  -34,  -34,  -33,  -33,  -33,  -33,
            -32,  -32,  -31,  -31,  -31,  -31,  -30,  -30,
            -29,  -29,  -29,  -29,  -28,  -28,  -27,  -27,
            -27,  -27,  -26,  -26,  -25,  -25,  -25,  -25,
            -24,  -24,  -24,  -24,  -23,  -23,  -23,  -23,
            -22,  -22,  -22,  -22,  -21,  -21,  -21,  -21,
            -20,  -20,  -20,  -20,  -19,  -19,  -19,  -19,
            -18,  -18,  -18,  -18,  -17,  -17,  -17,  -17,
            -16,  -16,  -16,  -16,  -16,  -16,  -15,  -15,
            -15,  -15,  -14,  -14,  -14,  -14,  -14,  -14,
            -13,  -13,  -13,  -13,  -13,  -13,  -12,  -12,
            -12,  -12,  -12,  -12,  -11,  -11,  -11,  -11,
            -11,  -11,  -10,  -10,  -10,  -10,  -10,  -10,
            -9,   -9,   -9,   -9,   -9,   -9,   -8,   -8,
            -8,   -8,   -8,   -8,   -8,   -8,   -7,   -7,
            -7,   -7,   -7,   -7,   -7,   -7,   -6,   -6,
            -6,   -6,   -6,   -6,   -6,   -6,   -5,   -5,
            -5,   -5,   -5,   -5,   -5,   -5,   -4,   -4,
            -3,   -3,   -2,   -2,   -2,   -2,   -2,   -2,
            -2,   -2,   -2,   -2,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,    0,    0,
            1,    1,    2,    2,    2,    2,    2,    2,
            2,    2,    2,    2,    2,    2,    2,    2,
            2,    2,    2,    2,    2,    2,    2,    2,
            2,    2,    2,    2,    2,    2,    2,    2,
            2,    2,    2,    2,    2,    2,    2,    2,
            2,    2,    2,    2,    2,    2,    2,    2,
            2,    2,    2,    2,    2,    2,    2,    2,
            3,    3,    3,    3,    3,    3,    3,    3,
            3,    3,    3,    3,    3,    3,    3,    3,
            3,    3,    3,    3,    3,    3,    4,    4,
            4,    4,    4,    4,    4,    4,    4,    4,
            4,    4,    4,    4,    4,    4,    4,    4,
            4,    4,    4,    4,    4,    4,    4,    4,
            4,    4,    5,    5,    5,    5,    5,    5,
            5,    5,    5,    5,    5,    5,    5,    5,
            5,    5,    6,    6,    6,    6,    6,    6,
            6,    6,    6,    6,    6,    6,    6,    6,
            6,    6,    6,    6,    6,    6,    6,    6,
            6,    6,    6,    6,    6,    6,    6,    6,
            6,    6,    7,    7,    7,    7,    7,    7,
            7,    7,    7,    7,    7,    7,    7,    7,
            7,    7,    7,    7,    7,    7,    7,    7,
            7,    7,    7,    7,    8,    8,    8,    8,
            8,    8,    8,    8,    8,    8,    8,    8,
            8,    8,    8,    8,    8,    8,    8,    8,
            8,    8,    9,    9,    9,    9,    9,    9,
            9,    9,    9,    9,    9,    9,    9,    9,
            9,    9,    9,    9,    9,    9,    9,    9,
            9,    9,    9,    9,    9,    9,    9,    9,
            9,    9,   10,   10,   10,   10,   10,   10,
            10,   10,   10,   10,   10,   10,   10,   10,
            10,   10,   10,   10,   10,   10,   10,    0
    };
    int wr1_g_tab[512] = {
        -126, -126, -125, -125, -124, -124, -123, -123,
        -122, -122, -121, -121, -120, -120, -119, -119,
        -118, -118, -117, -117, -116, -116, -115, -115,
        -114, -114, -113, -113, -112, -112, -111, -111,
        -110, -110, -109, -109, -108, -108, -107, -107,
        -106, -106, -105, -105, -104, -104, -103, -103,
        -102, -102, -101, -101, -100, -100,  -99,  -99,
        -98,  -98,  -97,  -97,  -96,  -96,  -95,  -95,
        -94,  -94,  -93,  -93,  -92,  -92,  -91,  -91,
        -90,  -90,  -89,  -89,  -88,  -88,  -87,  -87,
        -86,  -86,  -85,  -85,  -84,  -84,  -83,  -83,
        -82,  -82,  -81,  -81,  -80,  -80,  -79,  -79,
        -78,  -78,  -77,  -77,  -76,  -76,  -75,  -75,
        -74,  -74,  -73,  -73,  -72,  -72,  -71,  -71,
        -70,  -70,  -69,  -69,  -68,  -68,  -67,  -67,
        -66,  -66,  -65,  -65,  -64,  -64,  -63,  -63,
        -61,  -61,  -59,  -59,  -57,  -57,  -54,  -54,
        -52,  -52,  -50,  -50,  -48,  -48,  -46,  -46,
        -44,  -44,  -42,  -42,  -41,  -41,  -39,  -39,
        -37,  -37,  -36,  -36,  -34,  -34,  -33,  -33,
        -31,  -31,  -30,  -30,  -29,  -29,  -27,  -27,
        -26,  -26,  -25,  -25,  -24,  -24,  -23,  -23,
        -22,  -22,  -21,  -21,  -20,  -20,  -19,  -19,
        -18,  -18,  -17,  -17,  -16,  -16,  -15,  -15,
        -14,  -14,  -14,  -14,  -13,  -13,  -12,  -12,
        -12,  -12,  -11,  -11,  -10,  -10,  -10,  -10,
        -9,   -9,   -8,   -8,   -8,   -8,   -7,   -7,
        -7,   -7,   -6,   -6,   -6,   -6,   -5,   -5,
        -5,   -5,   -4,   -4,   -2,   -2,   -2,   -2,
        -2,   -2,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,    0,    0,
        1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    2,    2,    2,    2,    2,    2,
        4,    4,    5,    5,    5,    5,    6,    6,
        6,    6,    7,    7,    7,    7,    8,    8,
        8,    8,    9,    9,   10,   10,   10,   10,
        11,   11,   12,   12,   12,   12,   13,   13,
        14,   14,   14,   14,   15,   15,   16,   16,
        17,   17,   18,   18,   19,   19,   20,   20,
        21,   21,   22,   22,   23,   23,   24,   24,
        25,   25,   26,   26,   27,   27,   29,   29,
        30,   30,   31,   31,   33,   33,   34,   34,
        36,   36,   37,   37,   39,   39,   41,   41,
        42,   42,   44,   44,   46,   46,   48,   48,
        50,   50,   52,   52,   54,   54,   57,   57,
        59,   59,   61,   61,   63,   63,   64,   64,
        65,   65,   66,   66,   67,   67,   68,   68,
        69,   69,   70,   70,   71,   71,   72,   72,
        73,   73,   74,   74,   75,   75,   76,   76,
        77,   77,   78,   78,   79,   79,   80,   80,
        81,   81,   82,   82,   83,   83,   84,   84,
        85,   85,   86,   86,   87,   87,   88,   88,
        89,   89,   90,   90,   91,   91,   92,   92,
        93,   93,   94,   94,   95,   95,   96,   96,
        97,   97,   98,   98,   99,   99,  100,  100,
        101,  101,  102,  102,  103,  103,  104,  104,
        105,  105,  106,  106,  107,  107,  108,  108,
        109,  109,  110,  110,  111,  111,  112,  112,
        113,  113,  114,  114,  115,  115,  116,  116,
        117,  117,  118,  118,  119,  119,  120,  120,
        121,  121,  122,  122,  123,  123,  124,  124,
        125,  125,  126,  126,  127,  127,  127,    0
    };

    int u, prevY, Y = 0;
    int f, g;
    int x_ahead, y_ahead;
    int t, tx, rx, tbias;
    int offset = WR1_BIAS;
    unsigned im, threshold = 0, sum, sqr_sum;
    double mu = 0.0, s_dev = 0.0;
    int nZ, n;
    int *Z;
    int st = 0, sn = 0;

    x_lookahead = x_lookahead % width;

    if (bias_mode == 0)
    {
        sum = 0;
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                im = p_im[y][x].s;
                im += 2;
                im /= 3;
                sum += im;
            }
        }
        mu = (double)sum / (width*height);

        sqr_sum = 0;
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                im = p_im[y][x].s;
                im += 2;
                im /= 3;
                sqr_sum += (im * im);
            }
        }
        s_dev = sqrt((double)sqr_sum / (width*height) - mu * mu);
        offset = (int)(s_dev - 40);
    } else {
        offset = bias_mode;
    }

    nZ = 2 * width + 1;
    Z = new int[nZ];
    for(n = 0; n < nZ; ++n){ Z[n] = 0;}
    Z[0] = prevY = (int)mu;

    for (y = 0; y< 1 + y_lookahead; y++)
    {
        if (y < y_lookahead) {t = width;} else {t = x_lookahead;}
        for (x=0; x < t; x++)
        {
            im = p_im[y][x].s;
            im += 2;
            im /= 3;
            u = im;
            f = -wr1_f_tab[WR1_F_OFFSET - (u - prevY)];
            Y = prevY + f;
            if (y == 1)
            {
                Z[x] = (int)mu;
            }
            else
            {
                g = -wr1_g_tab[WR1_G_OFFSET - (Y - Z[x])];
                Z[x] = Z[x] + g;
            }
        }

    }
    x_ahead = 1 + x_lookahead;
    y_ahead = 1 + y_lookahead;

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            threshold = bias_factor;
            tx = 256 - Z[x_ahead];
            tbias = -offset;
            if (tx < WR1_BIAS_CROSSOVER)
            {
                rx = tx - tbias - (int)(WR1_BLACK_BIAS_FACTOR * (WR1_BIAS_CROSSOVER - tx));
            }
            else if (tx >= WR1_BIAS_CROSSOVER)
            {
                rx = tx + tbias + (int)(WR1_WHITE_BIAS_FACTOR * (tx - WR1_BIAS_CROSSOVER));
            }
            else
            {
                rx = tx;
            }
            if (rx < BIN_FOREGROUND) {rx = BIN_FOREGROUND;}
            if (rx > BIN_BACKGROUND) {rx = BIN_BACKGROUND;}
            rx = 256 - rx;
            threshold *= rx;
            threshold /= 100;
            st += threshold;
            sn++;
            im = p_im[y][x].s;
            im += 2;
            im /= 3;
            if (im < threshold) {val = 0;} else {val = 255;}
            d_im[y][x] = val;
            x_ahead++;
            if (x_ahead >= w)
            {
                x_ahead = 1;
                y_ahead++;
            }
            if (y_ahead < h)
            {
                prevY = Y;
                im = p_im[y_ahead][x_ahead].s;
                im += 2;
                im /= 3;
                f = -wr1_f_tab[WR1_F_OFFSET - (im - prevY)];
                Y = prevY + f_factor * f / 100;
                g = -wr1_g_tab[WR1_G_OFFSET - (Y - Z[x_ahead])];
                Z[x_ahead] = Z[x_ahead] + g_factor * g / 100;
            }
            else
            {
                Z[x_ahead] = Z[x_ahead - 1];
            }
        }
    }
    if (sn > 0) {threshold = 3 * st / sn;}

    delete [] Z;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

