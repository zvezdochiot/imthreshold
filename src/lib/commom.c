//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

////////////////////////////////////////////////////////////////////////////////

BYTE ByteClamp(int c)
{
    BYTE buff[3] = {(BYTE)c, 255, 0};
    return buff[ (int)(c < 0) + (int)((unsigned)c > 255) ];
}

////////////////////////////////////////////////////////////////////////////////

WORD Byte3Clamp(int c)
{
    WORD buff[3] = {(WORD)c, 765, 0};
    return buff[ (int)(c < 0) + (int)((unsigned)c > 765) ];
}

////////////////////////////////////////////////////////////////////////////////

unsigned IndexClamp(int i, unsigned threshold)
{
    unsigned buff[3] = {(unsigned)i, threshold, 0};
    return buff[ (int)(i < 0) + (int)((unsigned)i > threshold) ];
}

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
        ims += (unsigned)im.c[d];
    }
    im.s = (WORD)ims;

    return im;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTccorS (IMTpixel im)
{
    unsigned d;
    int ims, imm, t, ts;

    im = IMTcalcS(im);

    ims = im.s;
    imm = ims / 3;
    ts = 0;
    for (d = 0; d < 3; d++)
    {
        t = (int)im.c[d];
        t = (t > imm) ? (t - imm) : (imm - t);
        ts += t;
    }
    ims += ts;
    im.s = Byte3Clamp(ims);

    return im;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTRGBtoRYB4 (IMTpixel pim, int direct)
{
    int R, G, B, r, y, b;

    if (direct < 0)
    {
        r = (int)pim.c[0];
        y = (int)pim.c[1];
        b = (int)pim.c[2];
        R = (r + r + r + r + r - y - y - y + b + 1) / 3;
        G = (y + y + y + y + y + y + y + y + y - r - r - r - b - b - b + 1) / 3;
        B = (r - y - y - y + b + b + b + b + b + 1) / 3;
        pim.c[0] = ByteClamp(R);
        pim.c[1] = ByteClamp(G);
        pim.c[2] = ByteClamp(B);
    }
    else
    {
        R = (int)pim.c[0];
        G = (int)pim.c[1];
        B = (int)pim.c[2];
        r = (R + R + R + G + 2) / 4;
        y = (R + G + G + B + 2) / 4;
        b = (G + B + B + B + 2) / 4;
        pim.c[0] = ByteClamp(r);
        pim.c[1] = ByteClamp(y);
        pim.c[2] = ByteClamp(b);
    }
    pim = IMTcalcS(pim);
    return pim;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel** IMTalloc (unsigned height, unsigned width)
{
    IMTpixel** im;
    unsigned y;

    im = (IMTpixel**)malloc (height * sizeof(IMTpixel*));
    for (y = 0; y < height; y++) {im[y] = (IMTpixel*)malloc (width * sizeof(IMTpixel));}

    return im;
}

////////////////////////////////////////////////////////////////////////////////

void IMTfree (IMTpixel** im, unsigned height)
{
    unsigned y;

    for (y = 0; y < height; y++) {free(im[y]);}
    free(im);
}

////////////////////////////////////////////////////////////////////////////////

BYTE** BWalloc (unsigned height, unsigned width)
{
    BYTE** im;
    unsigned y;

    im = (BYTE**)malloc(height * sizeof(BYTE*));
    for (y = 0; y < height; y++) {im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

    return im;
}

////////////////////////////////////////////////////////////////////////////////

void BWfree (BYTE** im, unsigned height)
{
    unsigned y;

    for (y = 0; y < height; y++) {free(im[y]);}
    free(im);
}

////////////////////////////////////////////////////////////////////////////////

WORD** TLalloc (unsigned height, unsigned width)
{
    WORD** im;
    unsigned y;

    im = (WORD**)malloc(height * sizeof(WORD*));
    for (y = 0; y < height; y++) {im[y] = (WORD*)malloc(width * sizeof(WORD));}

    return im;
}

////////////////////////////////////////////////////////////////////////////////

void TLfree (WORD** im, unsigned height)
{
    unsigned y;

    for (y = 0; y < height; y++) {free(im[y]);}
    free(im);
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTdiffS (IMTpixel im)
{
    BYTE immax, immin;
    unsigned d;

    immax = im.c[0];
    immin = im.c[0];
    for (d = 1; d < 3; d++)
    {
        if (im.c[d] > immax) {immax = im.c[d];}
        if (im.c[d] < immin) {immin = im.c[d];}
    }
    im.s = (WORD)(immax - immin) * 3;

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
        im = (int)IMTim.c[d];
        imf = (int)IMTimf.c[d];
        imd = im + im - imf;
        immt.c[d] = ByteClamp(imd);
    }
    immt = IMTcalcS (immt);

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTinterpolation (IMTpixel** p_im, unsigned height, unsigned width, float y, float x)
{
    unsigned d, y1, x1, y2, x2;
    float p11, p21, p12, p22, ky, kx, k11, k21, k12, k22, t;
    IMTpixel res;

    y1 = IndexClamp((int)y, (height - 1));
    x1 = IndexClamp((int)x, (width - 1));
    y2 = IndexClamp((int)(y1 + 1), (height - 1));
    x2 = IndexClamp((int)(x1 + 1), (width - 1));
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
        p11 = (float)p_im[y1][x1].c[d];
        p21 = (float)p_im[y2][x1].c[d];
        p12 = (float)p_im[y1][x2].c[d];
        p22 = (float)p_im[y2][x2].c[d];
        t = p11 * k11 + p21 * k21 + p12 * k12 + p22 * k22;
        res.c[d] = ByteClamp((int)(t + 0.5));
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

float IMTmean (IMTpixel** IMTim, unsigned height, unsigned width)
{
    unsigned x, y;
    float imx, immean = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (float)IMTim[y][x].s;
            immean += imx;
        }
    }
    immean /= height;
    immean /= width;
    immean /= 3.0;

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

float IMTdev (IMTpixel** IMTim, float immean, unsigned height, unsigned width)
{
    unsigned x, y;
    float imx, imd, imdev = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (float)IMTim[y][x].s / 3.0;
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

float IMTwb (IMTpixel** IMTim, float immean, unsigned height, unsigned width)
{
    unsigned x, y;
    float imx;
    long imwn = 0;
    float imwb;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (float)IMTim[y][x].s / 3.0;
            if (imx > immean) {imwn++;}
        }
    }
    imwb = (float)imwn;
    imwb /= height;
    imwb /= width;

    return imwb;
}

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

float IMTdist3c2p(IMTpixel IMTim, float* IMTimc)
{
    unsigned d;
    float imd, imds = 0;

    for (d = 0; d < 3; d++)
    {
        imd = (float)IMTim.c[d];
        imd -= (float)IMTimc[d];
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
    float ims;
    IMTpixel immean;

    ims = 0;
    for (y = y0; y < y1; y++)
    {
        for (x = x0; x < x1; x++)
        {
            imm = (unsigned)IMTim[y][x].s;
            ims += (float)imm;
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
                imm = (unsigned)IMTim[y][x].c[d];
                ims += (float)imm;
            }
        }
        if (n > 0)
        {
            ims /= n;
            if (ims > 255) {ims = 255;}
        }
        immean.c[d] = ByteClamp((int)(ims + 0.5));
    }
    immean = IMTcalcS (immean);

    return immean;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmaxIc (IMTpixel** IMTim, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
    unsigned y, x;
    unsigned ny, nx;
    WORD imm, immax;
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
    unsigned ny, nx;
    WORD imm, immin;
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

IMTpixel IMTaverageIc (IMTpixel** IMTim, IMTpixel IMTima, unsigned y0, unsigned x0, unsigned y1, unsigned x1, float part)
{
    unsigned y, x, d, n = 0;
    int imm;
    IMTpixel immean;
    float imx, ims, parts = 1.0 /((float)part + 1.0);

    ims = 0;
    for (y = y0; y < y1; y++)
    {
        for (x = x0; x < x1; x++)
        {
            imx = (float)IMTim[y][x].s;
            imx *= part;
            imx += (float)IMTima.s;
            imx *= parts;
            imm = (int)(imx + 0.5);
            IMTim[y][x].s = (WORD)Byte3Clamp(imm);
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
                imx = (float)IMTim[y][x].c[d];
                imx *= part;
                imx += (float)IMTima.c[d];
                imx *= parts;
                imm = (int)(imx + 0.5);
                IMTim[y][x].c[d] = ByteClamp(imm);
                ims += imm;
            }
        }
        if (n > 0) {ims /= n;}
        immean.c[d] = ByteClamp((int)(ims + 0.5));
    }
    immean = IMTcalcS (immean);

    return immean;
}

///////////////////////////////////////////////////////////////////////////////

void IMTBlurMask (IMTpixel** p_im, BYTE** m_im, unsigned height, unsigned width, int radius)
{
    unsigned y, x, y0, x0, y1, x1, i, j, c0, c1, c2, n, k, l;
    unsigned r = (unsigned)((radius < 0) ? -radius : radius);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (m_im[y][x] == 0)
            {
                y0 = ((y > r) ? (y - r) : 0);
                y1 = (((y + r) < height) ? (y + r) : height);
                x0 = ((x > r) ? (x - r) : 0);
                x1 = (((x + r) < width) ? (x + r) : width);
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
                            k = r + 1;
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
                    p_im[y][x].c[0] = ByteClamp((int)(c0 / n));
                    p_im[y][x].c[1] = ByteClamp((int)(c1 / n));
                    p_im[y][x].c[2] = ByteClamp((int)(c2 / n));
                }
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
