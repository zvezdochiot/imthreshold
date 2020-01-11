//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

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
                    imf = (int)IMTim[y][x].c[d];
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
            imm = ByteClamp((int)(spi + 0.5));
        }
        immt.c[d] = imm;
        ims += imm;
    }
    immt.s = ims;

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanIcM (IMTpixel** IMTim, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    unsigned imm, n = 0;
    double ims;
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
                imm = (unsigned)IMTim[y][x].s;
                ims += imm;
                n++;
            }
            j++;
        }
        i++;
    }
    if (n > 0) {ims /= n;}
    immean.s = Byte3Clamp((int)(ims + 0.5));
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
                    imm = (unsigned)IMTim[y][x].c[d];
                    ims += imm;
                }
                j++;
            }
            i++;
        }
        if (n > 0) {ims /= n;}
        immean.c[d] = ByteClamp((int)(ims + 0.5));
    }

    return immean;
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
                im = (unsigned)IMTim[y][x].s;
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
                        im = (int)IMTim[y][x].c[d];
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
        immt.s = Byte3Clamp(ims);
        for (d = 0; d < 3; d++)
        {
            imsc[d] /= n;
            immt.c[d] = ByteClamp(imsc[d]);
        }
    }

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanRadIcM (IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int im, imf, ims = 0;
    BYTE imm;
    IMTpixel immt;
    double spi, sp, p;

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
                    imf = (int)IMTim[y][x].c[d];
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
            imm = ByteClamp(im);
        } else {
            spi /= sp;
            imm = ByteClamp((int)(spi + 0.5));
        }
        immt.c[d] = imm;
        ims += (int)imm;
    }
    immt.s = Byte3Clamp(ims);

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTmeanBlIcM (IMTpixel** IMTim, IMTpixel IMTimm, double** sqfilt, double* linfilt, bool** fmask, unsigned y0, unsigned x0, unsigned y1, unsigned x1, unsigned dy, unsigned dx)
{
    unsigned y, x, d, i, j;
    int im, imf, imd, ims = 0;
    BYTE imm;
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
                    imf = (int)IMTim[y][x].c[d];
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
            imm = ByteClamp(im);
        } else {
            spi /= sp;
            imm = ByteClamp((int)(spi + 0.5));
        }
        immt.c[d] = imm;
        ims += (int)imm;
    }
    immt.s = Byte3Clamp(ims);

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
                    imf = (int)IMTim[y][x].c[d];
                    if (imf < imm) {imm = imf;}
                }
                j++;
            }
            i++;
        }
        immt.c[d] = ByteClamp(imm);
        ims += (int)immt.c[d];
    }
    immt.s = Byte3Clamp(ims);

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
                    imf = (int)IMTim[y][x].c[d];
                    if (imf > imm) {imm = imf;}
                }
                j++;
            }
            i++;
        }
        immt.c[d] = ByteClamp(imm);
        ims += immt.c[d];
    }
    immt.s = Byte3Clamp(ims);

    return immt;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterPMean (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double radius, int fmode, bool fneared)
{
    unsigned y, x, dx, dy, y0, x0, y1, x1, uradius, rn;
    int i, j, iradius;
    double l, radius2, p, sp, wb;
    IMTpixel IMTmeanS;
    double deltac[256] = {0};
    bool** fmask;
    double** fradial;

    uradius = (unsigned)((radius < 0) ? -radius : radius);
    iradius = (int)uradius;
    rn = 2 * uradius + 1;
    radius2 = radius * radius;

    fmask = (bool**)malloc(rn * sizeof(bool*));
    for (y = 0; y < rn; y++) {fmask[y] = (bool*)malloc(rn * sizeof(bool));}
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
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
                IMTmeanS = IMTmeanPtIcM(p_im, p_im[y][x], deltac, fmask, y0, x0, y1, x1, dy, dx);
                if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                d_im[y][x] = IMTmeanS;
            }
        }
        break;
    case 1:
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
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
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
                IMTmeanS = IMTmeanRadIcM(p_im, p_im[y][x], fradial, fmask, y0, x0, y1, x1, dy, dx);
                if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                d_im[y][x] = IMTmeanS;
            }
        }
        break;
    case 3:
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
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
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
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
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
                IMTmeanS = IMTmeanBlIcM(p_im, p_im[y][x], fradial, deltac, fmask, y0, x0, y1, x1, dy, dx);
                if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                d_im[y][x] = IMTmeanS;
            }
        }
        break;
    case 6:
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
                IMTmeanS = IMTmeanMinIcM(p_im, p_im[y][x], fmask, y0, x0, y1, x1, dy, dx);
                if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                d_im[y][x] = IMTmeanS;
            }
        }
        break;
    case 7:
        for (y = 0; y < height; y++)
        {
            y0 = (y > uradius) ? (y - uradius) : 0;
            dy = (y < uradius) ? (uradius - y) : 0;
            y1 = (y + uradius < height) ? (y + uradius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x0 = (x > uradius) ? (x - uradius) : 0;
                dx = (x < uradius) ? (uradius - x) : 0;
                x1 = (x + uradius < width) ? (x + uradius + 1) : width;
                IMTmeanS = IMTmeanMaxIcM(p_im, p_im[y][x], fmask, y0, x0, y1, x1, dy, dx);
                if (radius < 0) {IMTmeanS = IMTrefilter1p(p_im[y][x], IMTmeanS);}
                d_im[y][x] = IMTmeanS;
            }
        }
        break;
    }

    for (y = 0; y < rn; y++) {free(fradial[y]);}
    free(fradial);
    for (y = 0; y < rn; y++) {free(fmask[y]);}
    free(fmask);
}

////////////////////////////////////////////////////////////////////////////////
