//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSMirror (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, tim, pim;

    tim = (unsigned)IMTFilterTBiModValueIc (p_im, 0, 0, height, width);
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            pim = (unsigned)p_im[y][x].s;
            if (pim > tim)
            {
                pim = 765 - pim;
            }
            pim *= 2;
            p_im[y][x].s = (WORD)pim;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterSNorm (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, im, immin, immax, imd, threshold = 0;

    immin = 765;
    immax = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = (unsigned)p_im[y][x].s;
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
                im = (unsigned)p_im[y][x].s;
                im -= immin;
                im *= 765;
                im /= imd;
                p_im[y][x].s = (WORD)im;
            }
        }
    }
    threshold = immax + immin;
    threshold /= 6;

    return (int)threshold;
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
            ims = (int)pim.s;
            ims -= 384;
            imsd = 765 + (int)IMTdist(pim, bim);
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
            ims = (int)pim.s;
            ims -= (int)bim.s;
            imsd = (int)IMTdist(pim, bim);
            if (ims < 0) {ims = 384 - imsd;} else {ims = 384 + imsd;}
            if (ims < 0) {ims = 0;}
            if (ims > 765) {ims = 765;}
            p_im[y][x].s = (WORD)ims;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTFilterIllumCorr (IMTpixel** p_im, IMTpixel** b_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned y, x, d, im, imb;
    double res;
    int i, k;
    IMTpixel mean; // dominant color
    BYTE val;
    long max, palette[768] = {0};
    int max_pos[3];

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                im = (unsigned)b_im[y][x].c[d];
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
        val = ByteClamp(max_pos[d]); // dominant color
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
                im = (unsigned)p_im[y][x].c[d];
                im++;
                imb = (unsigned)b_im[y][x].c[d];
                imb++;
                res = (double)im;
                res /= imb;
                res *= mean.c[d];
                res += im;
                res /= 2;
                val = ByteClamp((int)(res + 0.5));
                d_im[y][x].c[d] = val;
                d_im[y][x].s += val;
            }
        }
    }
    return mean;
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
                t = (unsigned)(255 - p_im[y][x].c[d]);
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

IMTpixel IMTFilterGreyNorm (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, d, n;
    int im;
    double di, dist, sd, mins = 768.0, maxs = 0.0, means, ds, ming, maxg, meang, dg;
    IMTpixel imd;
    double pc[3] = {0.227, 0.453, 0.320}, mc[3], sc[3], ac[3];
    BYTE grey;

    n = height * width;
    ming = mins;
    maxg = maxs;
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
                im = (int)p_im[y][x].c[d];
                mc[d] += (double)im;
            }
            im = (int)p_im[y][x].s;
            di = (double)im;
            if (di < mins) {mins = di;}
            if (di > maxs) {maxs = di;}
        }
    }
    mins /= 3;
    maxs /= 3;
    means = (maxs + mins) / 2;
    ds = (maxs > mins) ? (maxs - mins) : ((means > 127.5) ? (255.0 - means) : means);
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
                im = (int)p_im[y][x].c[d];
                di = (double)im;
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
    if (sd < 0.0 || sd > 0.0)
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
                im = (int)p_im[y][x].c[d];
                di = (double)im;
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
    dg = (maxg > ming) ? (maxg - ming) : 1.0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dist = 0;
            for (d = 0; d < 3; d++)
            {
                im = (int)p_im[y][x].c[d];
                di = (double)im;
                di -= mc[d];
                di *= pc[d];
                di *= ac[d];
                dist += di;
            }
            dist -= meang;
            dist /= dg;
            dist *= ds;
            dist += means;
            grey = ByteClamp((int)(dist + 0.5));
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
        imd.c[d] = ByteClamp((int)(ac[d] * 255.0 + 0.5));
    }
    imd.s = Byte3Clamp((int)means);

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
            im = (int)p_im[y][x].s;
            si += (double)im;
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
                im = (int)p_im[y][x].c[d];
                st += (double)im;
            }
        }
        kt = (st < 0.0 || st > 0.0) ? (si / st) : 1.0;
        imd.c[d] = ByteClamp((int)(255 * kt / 2));
        for ( y = 0; y < height; y++ )
        {
            for ( x = 0; x < width; x++ )
            {
                im = (int)p_im[y][x].c[d];
                vt = (double)(im);
                vt *= kt;
                p_im[y][x].c[d] = ByteClamp((int)(vt + 0.5));
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

double IMTFilterLevelMean (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, double contour, double thres, int lower_bound, int upper_bound)
{
    unsigned r, x, y, x0, y0, x2, y2, xf, yf, d, n;
    int im, mean, t;
    double imx, val, km;
    int d_bound;

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
    if (thres < 0) {thres = -thres;}

    d_bound = upper_bound - lower_bound;
    r = (unsigned)((radius < 0) ? -radius : radius);

    for (y = 0; y < height; y++)
    {
        y0 = ((y > r) ? (y - r) : 0);
        y2 = (((y + r + 1) < height) ? (y + r + 1) : height);
        for (x = 0; x < width; x++)
        {
            x0 = ((x > r) ? (x - r) : 0);
            x2 = (((x + r + 1) < width) ? (x + r + 1) : width);
            mean = 0;
            n = 0;
            for (yf = y0; yf< y2; yf++)
            {
                for (xf = x0; xf < x2; xf++)
                {
                    mean += (int)p_im[yf][xf].s;
                    n++;
                }
            }
            mean /= n;
            im = (int)p_im[y][x].s;
            km = 1.0;
            if (im > 0)
            {
                t = im - mean;
                t = (t > -thres && t < thres) ? 0 : t;
                t *= contour;
                imx = (double)mean / 3.0;
                imx -= lower_bound;
                imx *= 255.0;
                imx /= d_bound;
                if (imx < 0.0) {imx = 0.0;}
                if (imx > 255.0) {imx = 255.0;}
                imx *= 3.0;
                imx += t;
                km = (double)imx / im;
            }
            for (d = 0; d < 3; d++)
            {
                val = (double)p_im[y][x].c[d];
                val *= km;
                d_im[y][x].c[d] = ByteClamp((int)(val + 0.5));
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

    if (thres < 0.0) {thres = -thres;}
    if (thres > 1.0) {thres = 1.0;}
    if (fpart < -1.0) {fpart = -1.0;}
    if (fpart > 1.0) {fpart = 1.0;}
    apart = fpart;
    if (apart < 0.0) {apart = -apart;}

    ks = 0.0;
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            im = (int)p_im[y][x].s;
            imx = (double)im;
            imx /= 765.0;
            if (fpart > 0.0)
            {
                if (imx < thres)
                {
                    tx = thres;
                    imx = tx - sqrt(tx * tx - imx * imx);
                } else {
                    tx = 1.0 - thres;
                    imx = 1.0 - imx;
                    imx = 1.0 - tx + sqrt(tx * tx - imx * imx);
                }
            } else {
                if (imx < thres)
                {
                    tx = thres;
                    imx -= thres;
                    imx = sqrt(tx * tx - imx * imx);
                } else {
                    tx = 1.0 - thres;
                    imx -= thres;
                    imx = 1.0 - sqrt(tx * tx - imx * imx);
                }
            }
            imx *= 765.0;
            k = imx + 1.0;
            k /= (im + 1.0);
            k -= 1.0;
            k *= apart;
            k += 1.0;
            ks += k;
            for (d = 0; d < 3; d++)
            {
                im = (int)p_im[y][x].c[d];
                imx = (double)im;
                imx *= k;
                d_im[y][x].c[d] = ByteClamp((int)(imx + 0.5));
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    ks /= (double)height;
    ks /= (double)width;

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
                im = (int)p_im[y][x].c[d];
                imm = (int)d_im[y][x].c[d];
                imd = im - imm;
                im += imd;
                ims += (double)imd;
                d_im[y][x].c[d] = ByteClamp(im);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    ims /= 3.0;
    ims /= (double)width;
    ims /= (double)height;

    return ims;
}

///////////////////////////////////////////////////////////////////////////////

double IMTFilterMirrorPart (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, double part)
{
    unsigned y, x, d;
    int im, imm, imd;
    double imx, ims = 0.0;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = (int)p_im[y][x].c[d];
                imm = (int)d_im[y][x].c[d];
                imd = im - imm;
                imx = (double)imd;
                imx *= part;
                imd = (int)(imx + 0.5);
                im += imd;
                ims += (double)imd;
                d_im[y][x].c[d] = ByteClamp(im);
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
                im = (int)p_im[y][x].c[d];
                imm = (im < 128) ? (im + 127) : (im - 128);
                p_im[y][x].c[d] = ByteClamp(imm);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

unsigned IMTFilterKMeans (IMTpixel** IMTim, unsigned height, unsigned width, unsigned ncluster, unsigned iters)
{
    unsigned y, x, i, j, d, y0, x0, y1, x1;
    unsigned nr, nr2, nc, k, d0, dn, dd, dy, dx;
    IMTpixel fgim, bgim;
    double fgdist, bgdist, cl;
    IMTpixel* fg_c = (IMTpixel*)malloc(ncluster * sizeof(IMTpixel));
    IMTcluster* fg_cs = (IMTcluster*)malloc(ncluster * sizeof(IMTcluster));
    cl = (double)ncluster;
    cl = sqrt(cl);
    nr = (unsigned)cl;
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
                    fg_c[k].c[d] = ByteClamp(((fg_cs[k].c[d] + i - 1)/ i));
                }
            }
        }
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            fgim = IMTim[y][x];
            bgdist = 2.0;
            i = 0;
            for (k = 0; k < ncluster; k++)
            {
                bgim = fg_c[k];
                fgdist = (double)IMTdist(fgim, bgim);
                if (fgdist < bgdist)
                {
                    bgdist = fgdist;
                    i = k;
                }
            }
            for (d = 0; d < 3; d++)
            {
                bgim.c[d] = fg_c[i].c[d];
            }
            bgim = IMTcalcS (bgim);
            IMTim[y][x] = bgim;
        }
    }
    free(fg_c);
    free(fg_cs);

    return iters;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterPosterize (IMTpixel** p_im, unsigned height, unsigned width, unsigned thres)
{
    unsigned y, x, d, n;
    unsigned imin, imax, imst, imc;
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
            imin = (unsigned)immin.c[d];
            xmin = (double)imin;
            xd = (double)(immax.c[d] - immin.c[d] + 1);
            xd /= 256.0;
            xd *= thres;
            imst = (unsigned)immax.c[d];
            imst++;
            while (imin < imst)
            {
                xmax = xmin + xd;
                imax = (unsigned)(xmax + 0.5);
                n = 0;
                sumc = 0.0;
                for (y = 0; y < height; y++)
                {
                    for (x = 0; x < width; x++)
                    {
                        imc = (unsigned)p_im[y][x].c[d];
                        if ((imc >= imin) && (imc < imax))
                        {
                            sumc += (double)imc;
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
                            imc = (unsigned)p_im[y][x].c[d];
                            if ((imc >= imin) && (imc < imax))
                            {
                                ime = (double)imc;
                                ime -= sumc;
                                ime = (ime < 0.0) ? -ime : ime;
                                ims += ime;
                                p_im[y][x].c[d] = ByteClamp((int)(sumc + 0.5));
                            }
                        }
                    }
                }
                xmin += xd;
                imin = (unsigned)(xmin + 0.5);
            }
        }
        ims /= 3;
        ims /= width;
        ims /= height;
    }

    return ims;
}

////////////////////////////////////////////////////////////////////////////////

double IMTFilterQuant (IMTpixel** p_im, unsigned height, unsigned width, unsigned quant)
{
    unsigned y, x, d, k, l, n;
    unsigned im, imt, imd;
    unsigned histogram[256];
    unsigned histthres[256];
    unsigned histquant[256];
    double histstep, histsum, histsumk, imds = 0.0;

    quant = (quant > 0) ? quant : 1;
    n = height * width;
    n = (n > 0) ? n : 1;
    histstep = (double)n;
    histstep /= quant;
    for (d = 0; d < 3; d++)
    {
        for (k = 0; k < 256; k++)
        {
            histogram[k] = 0;
            histthres[k] = 0;
            histquant[k] = 0;
        }
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                im = (unsigned)p_im[y][x].c[d];
                histogram[im]++;
            }
        }
        histsum = 0.0;
        for (k = 0; k < 256; k++)
        {
            histsum += histogram[k];
            l = (histsum > 0.0) ? (unsigned)((histsum - 1.0)/ histstep) : 0;
            histthres[k] = l;
        }
        for (l = 0; l < quant; l++)
        {
            histsum = 0;
            histsumk = 0;
            for (k = 0; k < 256; k++)
            {
                if (histthres[k] == l)
                {
                    histsum += histogram[k];
                    histsumk += (histogram[k] * k);
                }
            }
            histsumk = (histsum > 0) ? (histsumk / histsum) : histsumk;
            for (k = 0; k < 256; k++)
            {
                if (histthres[k] == l)
                {
                    histquant[k] = (unsigned)histsumk;
                }
            }
        }
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                im = (unsigned)p_im[y][x].c[d];
                imt = histquant[im];
                imd = (im < imt) ? (imt - im) : (im - imt);
                p_im[y][x].c[d] = ByteClamp((int)imt);
                imds += imd;
            }
        }
    }
    imds /= n;
    imds /= 3;

    return imds;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterMonoColor (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned x, y, d, maxc;
    BYTE maxcv;
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
            p_im[y][x].c[maxc] = maxcv;
            p_im[y][x].s = (WORD)maxcv;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterWhiteFill (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, yy, xx;
    int sw, niter = 0;
    WORD im, imf;

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
