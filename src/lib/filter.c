//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

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
    p_info.mid = (float)(p_info.max + p_info.min) / 510;
    p_info.mean = IMTmean(p_im, height, width);
    p_info.std = IMTdev(p_im, p_info.mean, height, width);
    p_info.wb = IMTwb(p_im, p_info.mean, height, width);
    p_info.mean /= 255;
    p_info.std /= 255;

    return p_info;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterAdSmooth (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, float radius)
{
    int y, x, yf, xf, yfp, xfp, yfn, xfn, i, j, d;
    int h = height;
    int w = width;

    float f = -1.0 / 8.0 / radius / radius;
    float p_gx, p_gy, p_weight;
    float p_weightTotal[3] = {0};
    float p_total[3] = {0};

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
                        p_gx = (float)p_im[yf][xfn].c[d];
                        p_gx -= (float)p_im[yf][xfp].c[d];
                        p_gy = (float)p_im[yfn][xf].c[d];
                        p_gy -= (float)p_im[yfp][xf].c[d];
                        p_weight = exp((p_gx * p_gx + p_gy * p_gy) * f);
                        p_total[d] += (p_weight * (float)p_im[yf][xf].c[d]);
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
                    d_im[y][x].c[d] = ByteClamp((int)(p_total[d] / p_weightTotal[d] + 0.5));
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
    float cs[3];
    BYTE csb[3];

    BYTE** mg_im = BWalloc(height, width);
    BYTE** md_im = BWalloc(height, width);

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
                                    csb[d] = ByteClamp((int)(cs[d] + 0.5));
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

    BWfree(md_im, height);
    BWfree(mg_im, height);
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterPeron (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, float radius, float noise)
{
    unsigned y, x, d, r, yp, yn, xp, xn, i, j, level;
    int im, imf;
    float dr = 0.2;
    float fim, fimf;

    if (radius < 0) {radius = -radius;}
    if (noise < 0) {noise = -noise;}
    if (noise == 0) {noise = 1;}
    level = (radius / dr);
    for ( r = 0; r < level; r++ )
    {
        for ( y = 0; y < height; y++ )
        {
            yp = (y > 0) ? (y - 1) : y;
            yn = (y + 1 < height) ? (y + 2) : height;
            for ( x = 0; x < width; x++ )
            {
                xp = (x > 0) ? (x - 1) : x;
                xn = (x + 1 < width) ? (x + 2) : width;
                for (d = 0; d < 3; d++)
                {
                    im = (int)p_im[y][x].c[d];
                    fim = 0.0;
                    for (i = yp; i < yn; i++)
                    {
                        for (j = xp; j < xn; j++)
                        {
                            imf = (int)p_im[i][j].c[d];
                            imf -= im;
                            fimf = (float)imf;
                            fimf /= noise;
                            fimf *= fimf;
                            fimf++;
                            fimf = (float)imf / fimf;
                            fim += fimf;
                        }
                    }
                    fim /= 2;
                    fim *= dr;
                    imf = im + (int)(fim + 0.5);
                    d_im[y][x].c[d] = ByteClamp(imf);
                }
                d_im[y][x] = IMTcalcS (d_im[y][x]);
            }
        }
        IMTFilterCopy(d_im, p_im, height, width);
    }
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterRetinex (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, float sigma)
{
    unsigned d, k, ksz;
    int y, x, yf, xf, i, j;
    int im, imf, kimi, kszm;
    float dbi, dbj, gauss, imx, imxf, imxg, imxn, kim, kims;
    float imxs, imxns, div2 = (sqrt(5.0)+1.0)/2.0-1.0, sumgk = 0.0;
    int h = (int)height, w = (int)width;
    float wh = (float)(w * h);
    float* gaussKernel;

    radius = (radius < 0) ? -radius : radius;
    ksz = (unsigned)(2 * radius + 1);
    kszm = (int)(ksz * ksz);
    gaussKernel = (float*)malloc(kszm * sizeof(float));

    k = 0;
    for (i = -radius; i <= radius; i++)
    {
        dbi = (float)i;
        for (j = -radius; j <= radius; j++)
        {
            dbj = (float)j;
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
            im = (int)p_im[y][x].s;
            imx = ((float)im / 3.0 + 1.0) / 256.0;
            imxs += imx;
            imxg = 0;
            k = 0;
            for (i = -radius; i <= radius; i++)
            {
                yf = y + i;
                if (yf < 0) {yf = -yf;}
                if (yf >= h) {yf = 2 * (h - 1) - yf;}
                for (j = -radius; j <= radius; j++)
                {
                    xf = x + j;
                    if (xf < 0) {xf = -xf;}
                    if (xf >= w) {xf = 2 * (w - 1) - xf;}
                    imf = (int)p_im[yf][xf].s;
                    imxf = ((float)imf / 3.0 + 1.0) / 256.0;
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
                im = (int)p_im[y][x].c[d];
                imx = (float)im;
                im = (int)(imx * kim + 0.5);
                d_im[y][x].c[d] = ByteClamp(im);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
    imxs /= wh;
    kims /= wh;
    imxns /= wh;
    kimi = (int)(255 * kims / 2);
    free(gaussKernel);

    return kimi;
}

////////////////////////////////////////////////////////////////////////////////

void IMTSelGaussInitMatrix (float radius, float *mat, int num)
{
    int dx;
    float sd, c1, c2;

    // This formula isn't really correct, but it'll do
    sd = radius / 3.329042969;
    c1 = 1.0 / sqrt (2.0 * PI * sd);
    c2 = -2.0 * (sd * sd);

    for (dx = 0; dx <= num; dx++)
        mat[dx] = c1 * exp ((dx * dx)/ c2);
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSelGauss (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, float radius, int maxdelta)
{
    float *mat;
    unsigned short *imat;
    int numrad, i, j, x, y, xk, yk, h, w;
    float fsum, fscale;
    unsigned d;
    unsigned p_sum[3] = {0};
    unsigned p_fact[3] = {0};
    unsigned p_rowsum[3] = {0};
    unsigned p_rowfact[3] = {0};
    int di, tmp;

    numrad = (int)radius;
    mat = (float*) calloc (numrad + 1, sizeof(float));

    IMTSelGaussInitMatrix(radius, mat, numrad);

    h = (int)height;
    w = (int)width;

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
        imat[numrad - y] = imat[numrad + y] = (unsigned short)(mat[y] * fscale + 0.5);
    }

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
                                tmp = (int)p_im[y][x].c[d];
                                tmp -= (int)p_im[yk][xk].c[d];
                                if ( tmp >= -maxdelta && tmp <= maxdelta)
                                {
                                    di = imat[numrad + i];
                                    p_rowsum[d] += di * (float)p_im[yk][xk].c[d];
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
                    d_im[y][x].c[d] = ByteClamp((int)(p_sum[d] / p_fact[d]));
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }

    // free up buffers
    free (imat);
    free (mat);
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterSobel (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned y, x, d;
    int i, j, im, imf;

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
            for (d = 0; d < 3; d++)
            {
                imf = (int)p_im[y][x].c[d];
                im = imf * 9;
                for (i = -1; i <= 1; i++)
                {
                    for (j = -1; j <= 1; j++)
                    {
                        imf = (int)p_im[y + i][x + j].c[d];
                        im -= imf;
                    }
                }
                im = 255 - im;
                d_im[y][x].c[d] = ByteClamp(im);
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterUnRipple (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, float thres)
{

    unsigned d;
    int y, x, yf, xf, i, j;
    int im, imf, imd;
    float imx, p, sp, spi, si, di;
    int h = (int)height;
    int w = (int)width;

    if (radius < 0) {radius = -radius;}
    if (thres < 0) {thres = 0;}

    for ( y = 0; y < h; y++ )
    {
        for ( x = 0; x < w; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = (int)p_im[y][x].c[d];
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
                        imf = (int)p_im[yf][xf].c[d];
                        imd = im - imf;
                        if (imd < 0) {imd = -imd;}
                        imx = (float)(imd);
                        p = 1.0 / (imx + 1.0);
                        sp += p;
                        imx = (float)imf * p;
                        spi += imx;
                    }
                }
                imx = (float)(im);
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
                    im = (int)(si + 0.5);
                    d_im[y][x].c[d] = ByteClamp(im);
                }
            }
            d_im[y][x] = IMTcalcS (d_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTGaussLineMatrix (float *cmatrix, float radius)
{
    float std_dev;
    float sum, t, tt;
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
                t = (float)i;
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

void IMTFilterGaussBlur (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, float radius)
{
    int iradius;
    int y, x, yf, xf, i;
    unsigned d;
    float imc;
    float sc[3];
    float* gaussmat;
    IMTpixel** t_im;
    int h = (int)height;
    int w = (int)width;

    if (radius < 0) {radius = -radius;}
    iradius = (int)(2.0 * radius + 0.5) + 1;

    gaussmat = (float*)malloc((iradius) * sizeof(float));
    t_im = IMTalloc(height, width);

    IMTGaussLineMatrix (gaussmat, radius);

    if (iradius > 1)
    {
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                for (d = 0; d < 3; d++)
                {
                    imc = (float)p_im[y][x].c[d];
                    sc[d] = imc * gaussmat[0];
                }
                for (i = 1; i < iradius; i++)
                {
                    yf = y - i;
                    if (yf < 0) {yf = 0;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = (float)p_im[yf][x].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                    yf = y + i;
                    if (yf >= h) {yf = h - 1;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = (float)p_im[yf][x].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                }
                for (d = 0; d < 3; d++)
                {
                    t_im[y][x].c[d] = ByteClamp((int)(sc[d] + 0.5));
                }
            }
        }
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                for (d = 0; d < 3; d++)
                {
                    imc = (float)t_im[y][x].c[d];
                    sc[d] = imc * gaussmat[0];
                }
                for (i = 1; i < iradius; i++)
                {
                    xf = x - i;
                    if (xf < 0) {xf = 0;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = (float)t_im[y][xf].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                    xf = x + i;
                    if (xf >= w) {xf = w - 1;}
                    for (d = 0; d < 3; d++)
                    {
                        imc = (float)t_im[y][xf].c[d];
                        sc[d] += imc * gaussmat[i];
                    }
                }
                for (d = 0; d < 3; d++)
                {
                    d_im[y][x].c[d] = ByteClamp((int)(sc[d] + 0.5));
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

    IMTfree(t_im, height);
    free(gaussmat);
}

//////////////////////////////////////////////////////////////////////////////////////////////

int IMTFilterClusterBiModC (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned ncolor)
{
    unsigned y, x, d, k, l, m, n, im, imt, ims, imds, imdm;
    int imd;
    float T, Tw, Tb, Tn, iw, ib, cn[3];
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
            cn[d] = 0.0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = (unsigned)d_im[y][x].s;
                if (imt == m)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = (unsigned)p_im[y][x].c[d];
                        cn[d] += (float)im;
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
            Tn = 0.0;
            while ( T != Tn )
            {
                Tn = T;
                Tb = 0.0;
                Tw = 0.0;
                ib = 0.0;
                iw = 0.0;
                for (y = 0; y < height; y++ )
                {
                    for (x = 0; x < width; x++)
                    {
                        im = (unsigned)p_im[y][x].c[d];
                        imt = (unsigned)d_im[y][x].s;
                        if (imt == m)
                        {
                            if ((float)im > T)
                            {
                                Tw += (float)im;
                                iw++;
                            } else {
                                Tb += (float)im;
                                ib++;
                            }
                        }
                    }
                }
                if (iw == 0.0 && ib == 0.0)
                {
                    T = Tn;
                } else if (iw == 0.0) {
                    T = Tb/ib;
                } else if (ib == 0.0) {
                    T = Tw/iw;
                } else {
                    T = ((Tw/iw) + (Tb/ib)) / 2.0;
                }
            }
            thres[d] = (unsigned)ByteClamp((int)T);
        }
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = (unsigned)d_im[y][x].s;
                if (imt == m)
                {
                    ims = 0;
                    for (d = 0; d < 3; d++)
                    {
                        im = (unsigned)p_im[y][x].c[d];
                        ims += ((im >= thres[d]) ? 255 : 0);
                    }
                    if (ims >= 384) {d_im[y][x].s = (WORD)l;}
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            cn[d] = 0.0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = (unsigned)d_im[y][x].s;
                if (imt == m)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = (unsigned)p_im[y][x].c[d];
                        cn[d] += (float)im;
                    }
                }
            }
        }
        cd[m] = 0;
        if (n > 0)
        {
            for (d = 0; d < 3; d++)
            {
                cn[d] /= (float)n;
            }
            for (y = 0; y < height; y++ )
            {
                for (x = 0; x < width; x++)
                {
                    imt = (unsigned)d_im[y][x].s;
                    if (imt == m)
                    {
                        imds = 0;
                        for (d = 0; d < 3; d++)
                        {
                            imd = (int)p_im[y][x].c[d];
                            imd -= (int)(cn[d] + 0.5);
                            if (imd < 0) {imd = -imd;}
                            imds += imd;
                        }
                        cd[m] = (imds > cd[m]) ? imds : cd[m];
                    }
                }
            }
        }
        for (d = 0; d < 3; d++)
        {
            cn[d] = 0.0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = (unsigned)d_im[y][x].s;
                if (imt == l)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = (unsigned)p_im[y][x].c[d];
                        cn[d] += (float)im;
                    }
                }
            }
        }
        cd[l] = 0;
        if (n > 0)
        {
            for (d = 0; d < 3; d++)
            {
                cn[d] /= (float)n;
            }
            for (y = 0; y < height; y++ )
            {
                for (x = 0; x < width; x++)
                {
                    imt = (unsigned)d_im[y][x].s;
                    if (imt == l)
                    {
                        imds = 0;
                        for (d = 0; d < 3; d++)
                        {
                            imd = (int)p_im[y][x].c[d];
                            imd -= (int)(cn[d] + 0.5);
                            imd = (imd < 0) ? -imd : imd;
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
            cn[d] = 0.0;
        }
        n = 0;
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = (unsigned)d_im[y][x].s;
                if (imt == l)
                {
                    n++;
                    for (d = 0; d < 3; d++)
                    {
                        im = (unsigned)p_im[y][x].c[d];
                        cn[d] += (float)im;
                    }
                }
            }
        }
        if (n > 0)
        {
            for (d = 0; d < 3; d++)
            {
                cn[d] /= (float)n;
            }
        }
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++)
            {
                imt = (unsigned)d_im[y][x].s;
                if (imt == l)
                {
                    for (d = 0; d < 3; d++)
                    {
                        d_im[y][x].c[d] = ByteClamp((int)(cn[d] + 0.5));
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

float IMTFilterClusterBWC (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, unsigned radius)
{
    unsigned y, x, d, y1, x1, y2, x2, i, j, distw, distb, gpart, bc, bs;
    IMTpixel pim, gim, bim, fim, cimw, cimb, cim;
    unsigned bwcsw[3], bwcsb[3], bwcnw, bwcnb, cw, cb;
    float kwb = 0.5;

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

            y1 = (y > radius) ? (y - radius) : 0;
            y2 = (y + radius < height) ? (y + radius + 1) : height;
            for (x = 0; x < width; x++)
            {
                x1 = (x > radius) ? (x - radius) : 0;
                x2 = (x + radius < width) ? (x + radius + 1) : width;
                pim = p_im[y][x];
                cim = pim;
                bim = IMTmeanIc(p_im, y1, x1, y2, x2);
                bs = 0;
                for (d = 0; d < 3; d++)
                {
                    bc = (unsigned)bim.c[d];
                    bc *= gpart;
                    bc += (unsigned)gim.c[d];
                    bc /= (gpart + 1);
                    bim.c[d] = ByteClamp((int)bc);
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
                        bwcsw[d] = (unsigned)ByteClamp((int)bwcsw[d]);
                    }
                    cimw = IMTset((BYTE)bwcsw[0],(BYTE)bwcsw[1],(BYTE)bwcsw[2]);
                } else {
                    cimw = IMTset(255,255,255);
                }
                if (bwcnb > 0)
                {
                    for (d = 0; d < 3; d++)
                    {
                        bwcsb[d] /= bwcnb;
                        bwcsb[d] = (unsigned)ByteClamp((int)bwcsb[d]);
                    }
                    cimb = IMTset((BYTE)bwcsb[0],(BYTE)bwcsb[1],(BYTE)bwcsb[2]);
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
        kwb = (float)cw;
        kwb /= (float)(cw + cb);
    }

    return kwb;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterUnsharpMask (IMTpixel** p_im, IMTpixel** b_im, unsigned height, unsigned width, float amount, int threshold)
{
    unsigned y, x, d;
    int t, diff, adiff;
    float value;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                t = (int)p_im[y][x].c[d];
                diff = t - (int)b_im[y][x].c[d];
                adiff = (diff < 0) ? -diff : diff;
                diff = (2 * adiff < threshold) ? 0 : diff;

                value = (float)p_im[y][x].c[d];
                value += (amount * diff);
                p_im[y][x].c[d] = ByteClamp((int)(value + 0.5));
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterMorph (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, int radius, bool fdilate)
{
    unsigned y, x, d, y0, x0, y1, x1, yf, xf, r;
    BYTE val;


    r = (unsigned)((radius < 0) ? -radius : radius);
    if (r > 0)
    {
        for (y = 0; y < height; y++ )
        {
            y0 = (y > 1) ? (y - 1) : 0;
            y1 = (y + 1 < height) ? (y + 1) : height;
            for (x = 0; x < width; x++ )
            {
                x0 = (x > 1) ? (x - 1) : 0;
                x1 = (x + 1 < width) ? (x + 1) : width;
                for (d = 0; d < 3; d++)
                {
                    val = p_im[y0][x0].c[d];
                    for (yf = y0; yf < y1; yf++)
                    {
                        for (xf = x0; xf < x1; xf++)
                        {
                            if ((p_im[yf][xf].c[d] < val) == fdilate) {val = p_im[yf][xf].c[d];}
                        }
                    }
                    d_im[y][x].c[d] = val;
                }
                d_im[y][x] = IMTcalcS (d_im[y][x]);
            }
        }
    } else {
        for (y = 0; y < height; y++ )
        {
            for (x = 0; x < width; x++ )
            {
                d_im[y][x] = p_im[y][x];
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
