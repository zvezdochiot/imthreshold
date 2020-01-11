//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

////////////////////////////////////////////////////////////////////////////////

double IMTFilterDeNoiseDiff1p (IMTpixel** p_im, unsigned height, unsigned width, double kdenoise)
{
    unsigned y, x, d, n;
    double korigin = 1.0;
    int val, valp, vald, valdp, vali, histn[512];
    double ka, kn, valdn, valda, valds, valdso, valdsd, valdsn, histdelta[512];
    IMTpixel** t_im = IMTalloc(height, width);

    for (d = 0; d < 512; d++)
    {
        histdelta[d] = 0.0;
        histn[d] = 0;
    }
    n = 0;
    valdso = 0;
    for (d = 0; d < 3; d++)
    {
        for (y = 0; y < height; y++)
        {
            valp = (int)p_im[y][0].c[d];
            valdp = 0;
            for (x = 0; x < width; x++)
            {
                val = (int)p_im[y][x].c[d];
                vald = val - valp;
                valp = val;
                valds = (double)vald;
                valds /= 256.0;
                valds *= valds;
                valdso += valds;
                vali = valdp + 256;
                valdp = vald;
                histn[vali]++;
                ka = 1.0;
                ka /= (double)(histn[vali]);
                kn = 1.0 - ka;
                valda = (double)vald;
                valda *= ka;
                valdn = histdelta[vali];
                valdn *= kn;
                valdn += valda;
                histdelta[vali] = valdn;
                n++;
            }
        }
    }
    for (d = 0; d < 3; d++)
    {
        for (x = 0; x < width; x++)
        {
            valp = (int)p_im[0][x].c[d];
            valdp = 0;
            for (y = 0; y < height; y++)
            {
                val = (int)p_im[y][x].c[d];
                vald = val - valp;
                valp = val;
                valds = (double)vald;
                valds /= 256.0;
                valds *= valds;
                valdso += valds;
                vali = valdp + 256;
                valdp = vald;
                histn[vali]++;
                ka = 1.0;
                ka /= (double)(histn[vali]);
                kn = 1.0 - ka;
                valda = (double)vald;
                valda *= ka;
                valdn = histdelta[vali];
                valdn *= kn;
                valdn += valda;
                histdelta[vali] = valdn;
                n++;
            }
        }
    }
    valdso /= (double)n;
    valdso *= 2.0;
    valdso = sqrt(valdso);

    if (kdenoise == 0.0)
    {
        valdsd = 0;
        for (d = 0; d < 3; d++)
        {
            for (y = 0; y < height; y++)
            {
                valp = (int)p_im[y][0].c[d];
                valdp = 0;
                for (x = 0; x < width; x++)
                {
                    val = (int)p_im[y][x].c[d];
                    vald = val - valp;
                    vali = valdp + 256;
                    valdp = vald;
                    valdn = histdelta[vali];
                    val -= vald;
                    val += valdn;
                    valp = val;
                    valds = valdn;
                    valds /= 256.0;
                    valds *= valds;
                    valdsd += valds;
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            for (x = 0; x < width; x++)
            {
                valp = (int)p_im[0][x].c[d];
                valdp = 0;
                for (y = 0; y < height; y++)
                {
                    val = (int)p_im[y][x].c[d];
                    vald = val - valp;
                    vali = valdp + 256;
                    valdp = vald;
                    valdn = histdelta[vali];
                    val -= vald;
                    val += valdn;
                    valp = val;
                    valds = valdn;
                    valds /= 256.0;
                    valds *= valds;
                    valdsd += valds;
                }
            }
        }
        valdsd /= (double)n;
        valdsd *= 2.0;
        valdsd = sqrt(valdsd);

        valdsn = valdso + valdsd;
        if (valdsn > 0)
        {
            kdenoise = valdso / valdsn;
        }
        kdenoise /= 2;
    }
    korigin = 1.0 - kdenoise;

    for (d = 0; d < 3; d++)
    {
        for (y = 0; y < height; y++)
        {
            valp = (int)p_im[y][0].c[d];
            valdp = 0;
            for (x = 0; x < width; x++)
            {
                val = (int)p_im[y][x].c[d];
                vald = val - valp;
                vali = valdp + 256;
                valdp = vald;
                valdn = histdelta[vali];
                valdn *= kdenoise;
                valda = (double)vald;
                valda *= korigin;
                valda += valdn;
                valdp = (int)valda;
                valda += 0.5;
                valp = val;
                val -= vald;
                val += (int)valda;
                t_im[y][x].c[d] = ByteClamp(val);
            }
        }
    }
    for (d = 0; d < 3; d++)
    {
        for (x = 0; x < width; x++)
        {
            valp = (int)t_im[0][x].c[d];
            valdp = 0;
            for (y = 0; y < height; y++)
            {
                val = (int)t_im[y][x].c[d];
                vald = val - valp;
                vali = valdp + 256;
                valdp = vald;
                valdn = histdelta[vali];
                valdn *= kdenoise;
                valda = (double)vald;
                valda *= korigin;
                valda += valdn;
                valdp = (int)valda;
                valda += 0.5;
                valp = val;
                val -= vald;
                val += (int)valda;
                p_im[y][x].c[d] = ByteClamp(val);
            }
        }
    }

    IMTfree(t_im, height);

    return kdenoise;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterDeNoiseDiff (IMTpixel** p_im, unsigned height, unsigned width, unsigned radius, double kdenoise)
{
    unsigned i;
    double kdenoises = 0.0;
    if (radius > 0)
    {
        for (i = 0; i < radius; i++)
        {
            kdenoises += IMTFilterDeNoiseDiff1p (p_im, height, width, kdenoise);
        }
        kdenoises /= (double)radius;
    }

    return kdenoises;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterRS (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned d;
    int y, x, yp2, xp2, yp1, xp1, yn1, xn1, yn2, xn2;
    double imx = 0.0, ims = 0.0, imd = 0.0;
    int h = (int)height;
    int w = (int)width;
    IMTpixel** t_im = IMTalloc(height, width);

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
                imx = (double)p_im[yp2][xp2].c[d];
                imx *= imx;
                ims -= imx;
                imx = (double)p_im[yp2][xp1].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yp2][x].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yp2][xn1].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yp2][xn2].c[d];
                imx *= imx;
                ims -= imx;
                imx = (double)p_im[yp1][xp2].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yp1][xp1].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[yp1][x].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[yp1][xn1].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[yp1][xn2].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[y][xp2].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[y][xp1].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[y][xn1].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[y][xn2].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yn1][xp2].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yn1][xp1].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[yn1][x].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[yn1][xn1].c[d];
                imx *= imx;
                ims += imx;
                ims += imx;
                ims += imx;
                ims += imx;
                ims += imx;
                imx = (double)p_im[yn1][xn2].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yn2][xp2].c[d];
                imx *= imx;
                ims -= imx;
                imx = (double)p_im[yn2][xp1].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yn2][x].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yn2][xn1].c[d];
                imx *= imx;
                ims -= imx;
                ims -= imx;
                imx = (double)p_im[yn2][xn2].c[d];
                imx *= imx;
                ims -= imx;
                ims /= 81.0;
                imd += (ims < 0.0) ? -ims : ims;
                imx = (double)p_im[y][x].c[d];
                imx *= imx;
                ims += imx;
                ims = (ims < 0.0) ? -ims : ims;
                ims = sqrt(ims);
                t_im[y][x].c[d] = ByteClamp((int)(ims + 0.5));
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

    IMTfree(t_im, height);

    return imd;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterShrink (IMTpixel** p_im, unsigned height, unsigned width, int thres)
{
    unsigned d, y, x, n;
    int imdy, imdx, imd;
    double ims;
    IMTpixel im, imy, imx, imf;

    if (thres < 0)
    {
        ims = IMTmean (p_im, height, width);
        ims = IMTdev (p_im, ims, height, width);
        ims /= -thres;
        thres = (int)ims;
    }
    thres = (int)ByteClamp(thres);

    n=0;
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
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
                    imd = (int)((im.c[d] > imy.c[d]) ? (im.c[d] - imy.c[d]) : (imy.c[d] - im.c[d]));
                    imdy = (imdy < imd) ? imd : imdy;
                }
            }
            imdx = 256;
            if (x > 0)
            {
                imdx = 0;
                imx = p_im[y][x - 1];
                for (d = 0; d < 3; d++)
                {
                    imd = (int)((im.c[d] > imx.c[d]) ? (im.c[d] - imx.c[d]) : (imx.c[d] - im.c[d]));
                    imdx = (imdx < imd) ? imd : imdx;
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
    for ( y = height; y > 0; y-- )
    {
        for ( x = width; x > 0; x-- )
        {
            imd = 0;
            imdy = 256;
            im = p_im[y - 1][x - 1];
            if (y < height)
            {
                imdy = 0;
                imy = p_im[y][x - 1];
                for (d = 0; d < 3; d++)
                {
                    imd = (int)((im.c[d] > imy.c[d]) ? (im.c[d] - imy.c[d]) : (imy.c[d] - im.c[d]));
                    imdy = (imdy < imd) ? imd : imdy;
                }
            }
            imdx = 256;
            if (x < width)
            {
                imdx = 0;
                imx = p_im[y - 1][x];
                for (d=0; d < 3; d++)
                {
                    imd = (int)((im.c[d] > imx.c[d]) ? (im.c[d] - imx.c[d]) : (imx.c[d] - im.c[d]));
                    imdx = (imdx < imd) ? imd : imdx;
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
            if (im.s != p_im[y - 1][x - 1].s) {n++;}
            p_im[y - 1][x - 1] = im;
        }
    }
    ims = (double)(n) / height / width * 2.0 / 3.0;

    return ims;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterNoiseVariance (IMTpixel** p_im, unsigned height, unsigned width, int radius)
{
    unsigned x, y, n;
    unsigned r, y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, imn;
    double noise = 0.0;

    r = (unsigned)((radius < 0) ? -radius : radius);

    imn = 0;
    for (y = 0; y < height; y++)
    {
        y1 = (y > r) ? (y - r) : 0;
        y2 = ((y + r) < height) ? (y + r + 1) : height;
        for (x = 0; x < width; x++)
        {
            x1 = (x > r) ? (x - r) : 0;
            x2 = ((x + r) < width) ? (x + r + 1) : width;
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
    unsigned r, y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, kvar, var, multiplier;
    int ivar;

    noise *= noise;
    r = (unsigned)((radius < 0) ? -radius : radius);

    for (y = 0; y < height; y++)
    {
        y1 = (y > r) ? (y - r) : 0;
        y2 = ((y + r) < height) ? (y + r + 1) : height;
        for (x = 0; x < width; x++)
        {
            x1 = (x > r) ? (x - r) : 0;
            x2 = ((x + r) < width) ? (x + r + 1) : width;
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
                var = (double)p_im[y][x].c[d];
                var += kvar;
                ivar = (int)(var + 0.5);
                d_im[y][x].c[d] = ByteClamp(ivar);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
