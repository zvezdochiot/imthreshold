//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

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

void IMTFilterDespeck2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    unsigned k2 = Ksize/2;
    unsigned kmax = Ksize - k2;
    unsigned y, x, y0, x0, y1, x1, y2, x2;
    unsigned n, val;
    BYTE** d_im = BWalloc(height, width);

    for (y = 0; y < height; y++)
    {
        y0 = ((y > k2) ? (y - k2) : 0);
        y2 = (((y + kmax) < height) ? (y + kmax) : height);
        for (x = 0; x < width; x++)
        {
            x0 = ((x > k2) ? (x - k2) : 0);
            x2 = (((x + kmax) < width) ? (x + kmax) : width);
            val = 0;
            n = 0;
            for(y1 = y0; y1 < y2; y1++)
            {
                for(x1 = x0; x1 < x2; x1++)
                {
                    n++;
                    if (p_im[y1][x1] > 0) {val++;}
                }
            }
            val *= 2;
            if (val == n)
            {
                val = (unsigned)p_im[y][x];
            } else if (val > n) {
                val = 255;
            } else {
                val = 0;
            }
            d_im[y][x] = (BYTE)val;
        }
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            p_im[y][x] = d_im[y][x];
        }
    }

    BWfree(d_im, height);
}

////////////////////////////////////////////////////////////////////////////////

unsigned IMTFilterDMag2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    unsigned height2 = height * 2, width2 = width * 2, y, threshold;
    BYTE** d_im = BWalloc(height2, width2);

    threshold = 0;
    for (y = 0; y < Ksize; y++)
    {
        threshold += IMTFilterSBWMag2(p_im, d_im, height, width, height2, width2);
        threshold += IMTFilterSBWReduce2(d_im, p_im, height2, width2, height, width);
    }
    threshold /= Ksize;
    threshold /= 2;

    BWfree(d_im, height2);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterDNeuro2 (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize, float lambda, unsigned lnum)
{
    int i, j, y, x, y1, x1;
    unsigned n, l;
    int val;
    int h = (int)height;
    int w = (int)width;
    int k = (int)Ksize;
    int k2 = k / 2;
    float z, s, er, sigma, dw;
    float** weight;
    BYTE** d_im;

    weight = (float**)malloc(Ksize * sizeof(float*));
    for (y = 0; y < k; y++) {weight[y] = (float*)malloc(Ksize * sizeof(float));}

    d_im = BWalloc(height, width);

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

    for (y = 0; y < k; y++) {free(weight[y]);}
    free(weight);
    BWfree(d_im, height);
}

////////////////////////////////////////////////////////////////////////////////

float IMTFilterDphist (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    unsigned y, x, f, g, i, j, k, l, m, n, val, hmin, imin, hmax, imax, nrep, nreps;
    unsigned hist[512] = {0};
    float kdp;

    int** d_im;
    d_im = (int**)malloc(height * sizeof(int*));
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
            d_im[y][x] = (int)val;
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
                    val = (unsigned)d_im[y][x];
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
                        d_im[y][x] = (int)imax;
                        hist[imax]++;
                    }
                }
            }
            hist[imin] = 0;
            nreps += nrep;
        }
    }
    kdp = (float)nreps;
    kdp /= (float)n;

    for (y = 0; y < height; y++) {free(d_im[y]);}
    free(d_im);

    return kdp;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterDMinMax (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    unsigned k2 = Ksize;
    unsigned kmax = Ksize + 1;
    unsigned y, x, y0, x0, y1, x1, y2, x2;
    unsigned val;
    BYTE** d_im = BWalloc(height, width);

    for (y = 0; y < height; y++)
    {
        y0 = ((y > k2) ? (y - k2) : 0);
        y2 = (((y + kmax) < height) ? (y + kmax) : height);
        for (x = 0; x < width; x++)
        {
            x0 = ((x > k2) ? (x - k2) : 0);
            x2 = (((x + kmax) < width) ? (x + kmax) : width);
            val = 255;
            for(y1 = y0; y1 < y2; y1++)
            {
                for(x1 = x0; x1 < x2; x1++)
                {
                    if (p_im[y1][x1] == 0)
                    {
                        val = 0;
                        y1 = y2;
                        x1 = x2;
                    }
                }
            }
            d_im[y][x] = (BYTE)val;
        }
    }
    for (y = 0; y < height; y++)
    {
        y0 = ((y > k2) ? (y - k2) : 0);
        y2 = (((y + kmax) < height) ? (y + kmax) : height);
        for (x = 0; x < width; x++)
        {
            x0 = ((x > k2) ? (x - k2) : 0);
            x2 = (((x + kmax) < width) ? (x + kmax) : width);
            val = 0;
            for(y1 = y0; y1 < y2; y1++)
            {
                for(x1 = x0; x1 < x2; x1++)
                {
                    if (d_im[y1][x1] > 0)
                    {
                        val = 255;
                        y1 = y2;
                        x1 = x2;
                    }
                }
            }
            p_im[y][x] = (BYTE)val;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterDSmearing (BYTE** p_im, unsigned height, unsigned width, unsigned Ksize)
{
    unsigned th = Ksize * 100;
    unsigned tv = Ksize * 200;
    unsigned y, x, y0, x0, yt, xt;
    unsigned tt;
    BYTE val;
    BYTE** dh_im = BWalloc(height, width);
    BYTE** dv_im = BWalloc(height, width);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dh_im[y][x] = p_im[y][x];
            dv_im[y][x] = p_im[y][x];
        }
    }
    for (y = 0; y < height; y++)
    {
        x0 = 0;
        for (x = 0; x < width; x++)
        {
            val = p_im[y][x];
            if (val == 0)
            {
                tt = x - x0;
                if (tt < th)
                {
                    for (xt = x0; xt < x; xt++)
                    {
                        dh_im[y][xt] = 0;
                    }
                }
                x0 = x;
            }
        }
    }
    for (x = 0; x < width; x++)
    {
        y0 = 0;
        for (y = 0; y < height; y++)
        {
            val = p_im[y][x];
            if (val == 0)
            {
                tt = y - y0;
                if (tt < tv)
                {
                    for (yt = y0; yt < y; yt++)
                    {
                        dv_im[yt][x] = 0;
                    }
                }
                y0 = y;
            }
        }
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (dh_im[y][x] == 0 && dv_im[y][x] == 0)
            {
                p_im[y][x] = 0;
            }
        }
    }

    BWfree(dv_im, height);
    BWfree(dh_im, height);
}

////////////////////////////////////////////////////////////////////////////////
