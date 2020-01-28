//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

////////////////////////////////////////////////////////////////////////////////

int IMTFilterThreshold (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int threshold)
{
    unsigned y, x;
    BYTE val;
    int im;

    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            val = (BYTE) ((im >= threshold) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterThresholdLayer (IMTpixel** p_im, WORD** t_im, BYTE** d_im, unsigned height, unsigned width)
{
    unsigned y, x;
    BYTE val;
    int im, imt, threshold = 0;
    double imts;

    imts = 0;
    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = p_im[y][x].s;
            imt = t_im[y][x];
            imts += imt;
            val = (BYTE) ((im >= imt) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }
    imts /= height;
    imts /= width;
    threshold = (int)(imts + 0.5);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

void IMTFilterTLayerToImg (WORD** t_im, IMTpixel** d_im, unsigned height, unsigned width)
{
    unsigned y, x;
    BYTE val;
    int im;

    for (y = 0; y < height; y++ )
    {
        for (x = 0; x < width; x++)
        {
            im = t_im[y][x];
            im += 1;
            im /= 3;
            val = ByteClamp(im);
            d_im[y][x] = IMTset(val, val, val);
        }
    }
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
    int hw = 256; // square histogram width
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

int IMTFilterTBernsenLayer(IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, unsigned contrast_limit)
{
    int x, y, i, j, xt, yt;
    unsigned im, t;
    unsigned c, minimum, maximum;
    WORD val;
    int h = height;
    int w = width;
    int threshold = 0, st = 0, sn = 0;

    if (radius < 0) {radius = -radius;}

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
                val = p_im[y][x].s;
            } else {
                t = (maximum + minimum) / 2;
                st += t;
                sn++;
                val = Byte3Clamp((int)t);
            }
            t_im[y][x] = val;
        }
    }
    if (sn > 0) {threshold =  st / sn;}

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBernsen(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, unsigned contrast_limit)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTBernsenLayer(p_im, t_im, height, width, radius, contrast_limit);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBHTValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x;
    int i, im, il = 0, ir = 767;
    double wl = 0, wr = 0;
    double histogram[768] = {0};
    int threshold = 384;

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

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBHT (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int threshold = 384;

    threshold = IMTFilterTBHTValue (p_im, height, width);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiModValueBound (IMTpixel** p_im, unsigned height, unsigned width, int lower_bound, int upper_bound)
{
    unsigned y, x;
    int im;
    double T, Tw, Tb, Tn, iw, ib;
    int threshold = 0;

    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }

    T = (lower_bound + upper_bound);
    T /= 2;
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
                if ((im >= lower_bound) && (im <= upper_bound))
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
    threshold = (int)(T + 0.5);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiModValueIc (IMTpixel** p_im, unsigned y0, unsigned x0, unsigned y1, unsigned x1)
{
    unsigned y, x, im;
    double T, Tw, Tb, Tn, iw, ib;
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
        for (y = y0; y < y1; y++ )
        {
            for (x = x0; x < x1; x++)
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
    threshold = (int)(T + 0.5);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiModValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    int threshold = 0;

    threshold = IMTFilterTBiModValueIc (p_im, 0, 0, height, width);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiMod (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    int threshold = 0;

    threshold = IMTFilterTBiModValue (p_im, height, width);
    threshold += delta;
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiModLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int delta)
{
    unsigned y, x, y0, x0, y1, x1, r;
    double TG, T, Ts;
    WORD val;
    int threshold = 0;

    if (radius < 0) {radius = -radius;}
    r = radius;
    TG = IMTFilterTBiModValueIc (p_im, 0, 0, height, width);
    if (r > 0)
    {
        Ts = 0;
        TG *= (1 - sensitivity);
        for (y = 0; y < height; y++ )
        {
            y0 = 0;
            if (y > r) {y0 = y - r;}
            y1 = y + r;
            if (y1 > height) {y1 = height;}
            for (x = 0; x < width; x++ )
            {
                x0 = 0;
                if (x > r) {x0 = x - r;}
                x1 = x + r;
                if (x1 > width) {x1 = width;}
                T = IMTFilterTBiModValueIc (p_im, y0, x0, y1, x1);
                T *= sensitivity;
                T += TG;
                T += delta;
                Ts += T;
                val = Byte3Clamp((int)(T + 0.5));
                t_im[y][x] = val;
            }
        }
        Ts /= height;
        Ts /= width;
        threshold = (int)(Ts + 0.5);
    } else {
        threshold = TG;
        threshold += delta;
        val = Byte3Clamp((int)(threshold + 0.5));
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                t_im[y][x] = val;
            }
        }
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTBiModRegion (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int delta)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTBiModLayer (p_im, t_im, height, width, radius, sensitivity, delta);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

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
    threshold = IMTFilterTBiModValue (p_im, height, width);
    threshold += delta;
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTChistianLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imMg, imVg, imm, imv, t, st = 0, sn = 0;
    int threshold = 0;
    int h = height;
    int w = width;
    WORD val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }
    lower_bound *= 3;
    upper_bound *= 3;

    imMg = 765.0;
    imVg = 0.0;
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
            imv /= n;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            if (imm < imMg)
            {
                imMg = imm;
            }
            if (imv > imVg)
            {
                imVg = imv;
            }
        }
    }
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
            imv /= n;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imx < lower_bound)
            {
                t = lower_bound;
            } else if (imx > upper_bound)
            {
                t = upper_bound;
            } else {
                t = (imm + sensitivity * (imMg - imm + (imv / imVg) * (imm - imMg)) + delta);
            }
            val = Byte3Clamp((int)(t + 0.5));
            st += t;
            sn++;
            t_im[y][x] = val;
        }
    }
    if (sn == 0) { sn = 1;}
    threshold =  (st / sn);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTChistian (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTChistianLayer (p_im, t_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTDalg (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int region_size, int delta)
{
    unsigned x, y, i, j;

    unsigned whg, wwn, iy0, ix0, iyn, ixn, tx, tt;
    unsigned wwidth = region_size;
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
    IMTpixel pim, gim, tim, fgim, bgim;
    double fgdist, bgdist, kover, fgpart, bgpart, fgk = 1.0;
    unsigned maskbl, maskover, bgsover, fgsum[3], bgsum[3], fgnum, bgnum;

    IMTpixel** fgt_im = IMTalloc(heightbg, widthbg);

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
    if (doverlay < 0) {doverlay = 0;}
    kover = doverlay + 1.0;
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
            y1 = (((y0 + maskover) < height) ? (y0 + maskover) : height);
            y0b = i * blsz;
            y1b = (((y0b + bgsover) < heightbg) ? (y0b + bgsover) : heightbg);
            for (j = 0; j < cntw; j++)
            {
                x0 = j * maskbl;
                x1 = (((x0 + maskover) < width) ? (x0 + maskover) : width);
                x0b = j * blsz;
                x1b = (((x0b + bgsover) < widthbg) ? (x0b + bgsover) : widthbg);

                gim = IMTmeanIc(p_im, y0, x0, y1, x1);

                fgim = IMTmeanIc(fgt_im, y0b, x0b, y1b, x1b);
                bgim = IMTmeanIc(bg_im, y0b, x0b, y1b, x1b);

                fgdist = IMTdist(gim, fgim);
                bgdist = IMTdist(gim, bgim);

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
                        tim = IMTrefilter1p (pim, gim);
                        fgdist = IMTdist(tim, fgim);
                        bgdist = IMTdist(tim, bgim);
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
    IMTFilterSReduce (fgt_im, fg_im, heightbg, widthbg, fgs);
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
            bgdist = IMTdist(pim, bgim);
            fgdist = IMTdist(pim, fgim);
            m_im[y][x] = (BYTE) ((fgdist < bgdist) ? 0 : 255);
        }
    }

    IMTfree(fgt_im, heightbg);

    return wbmode;
}

///////////////////////////////////////////////////////////////////////////////

int IMTFilterTEntValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, i, t, im, cn = 768;
    double sum = 0, pTB, hhB, pTW, hhW, jMax, j;
    double histogram[768] = {0};
    double pT[768] = {0};
    double epsilon = 0;
    double hB[768] = {0};
    double hW[768] = {0};
    int threshold = 0;

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
        pTB = pT[t]; // DAVID
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

    return threshold;
}
////////////////////////////////////////////////////////////////////////////////

int IMTFilterTEnt (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int threshold = 0;

    threshold = IMTFilterTEntValue (p_im, height, width);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}
////////////////////////////////////////////////////////////////////////////////

int IMTFilterTEqBrightValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned x, y, i, tx, tt;
    double imx, sw, swt, dsr = 0, dst = 0;
    int histogram[768] = {0};
    int threshold = 0;

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
    threshold = tt;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTEqBright (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int threshold = 0;

    threshold = IMTFilterTEqBrightValue (p_im, height, width);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

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
                        val = ByteClamp((int)(imx + 0.5));
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
            t = (unsigned)bg_im[y][x].s;
            s = (unsigned)p_im[y][x].s;
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
                sum += (double)bg_im[y][x].s;
            }
        }
    }
    sum /= 3;

    b = sum / bin_sum;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (double)p_im[y][x].s;
            imk = (double)bg_im[y][x].s;
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

int IMTFilterTGradValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    int x, y, xp, xn, yp, yn;
    int im, imp, imn;
    double GX, GY, G, SG = 0, SGI = 0, SI = 0, SN = 0, S = 0;
    int threshold = 0;
    int h = (int)height;
    int w = (int)width;

    for (y = 0; y < h; y++)
    {
        yp = y -1;
        if (yp < 0) {yp = 0;}
        yn = y + 1;
        if (yn >= h) {yn = h - 1;}
        for (x = 0; x < w; x++)
        {
            im = (int)p_im[y][x].s;
            xp = x -1;
            if (xp < 0) {xp = 0;}
            xn = x + 1;
            if (xn >= w) {xn = w - 1;}
            imp = (int)p_im[y][xp].s;
            imn = (int)p_im[y][xn].s;
            GX = (double)(imn - imp);
            if (GX < 0) {GX = -GX;}
            imp = (int)p_im[yp][x].s;
            imn = (int)p_im[yn][x].s;
            GY = (double)(imn - imp);
            if (GY < 0) {GY = -GY;}
            G = sqrt(GX * GX + GY * GY);
            SG += G;
            SGI += ((double)im * G);
            SI += (double)im;
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

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTGrad (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int threshold = 0;

    threshold = IMTFilterTGradValue (p_im, height, width);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTGravureLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imMg, imVg, imm, imv, t, st = 0, sn = 0;
    int threshold = 0;
    int h = height;
    int w = width;
    WORD val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }
    lower_bound *= 3;
    upper_bound *= 3;

    imMg = IMTFilterTBiModValue (p_im, height, width);
    imVg = 0;
    n = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imx = (double)p_im[y][x].s;
            imx -= imMg;
            imVg += (imx * imx);
            n++;
        }
    }
    if (n == 0) {n = 1;}
    imVg /= n;
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
            imv /= n;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            imx = (double)p_im[y][x].s;
            imx /= 3;
            if (imx < lower_bound)
            {
                t = lower_bound;
            } else if (imx > upper_bound)
            {
                t = upper_bound;
            } else {
                t = (imm + sensitivity * (imMg - imm + (imv / imVg) * (imm - imMg)) + delta);
            }
            val = Byte3Clamp((int)(t + 0.5));
            st += t;
            sn++;
            t_im[y][x] = val;
        }
    }
    if (sn == 0) { sn = 1;}
    threshold =  (st / sn);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTGravure (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTGravureLayer (p_im, t_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

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

int IMTFilterTJanniValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x, i, cn = 768, im;
    double histogram[768] = {0};
    unsigned gmin=0, gmax=256, gmid;
    double spg = 0;
    int threshold = 0;

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

    for (i = (gmid + 1); i <= gmax; i++)
    {
        spg += histogram[i];
    }
    threshold = (int)(gmin + ((gmax - gmin) * spg));
    threshold = (threshold + gmid)/2;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTJanni (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int threshold = 0;

    threshold = IMTFilterTJanniValue (p_im, height, width);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

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
    int h = (int)height;
    int w = (int)width;
    int threshold = 0;
    int k = 0;
    int dl, dls = 1;
    double** kpt;
    double** ksum;
    double* kdist;

    cstep = 255.0 / (knum - 1);

    kpt = (double**)malloc(knum * sizeof(double*));
    for (y = 0; y < (int)knum; y++) {kpt[y] = (double*)malloc(4 * sizeof(double));}
    ksum = (double**)malloc(knum * sizeof(double*));
    for (y = 0; y < (int)knum; y++) {ksum[y] = (double*)malloc(4 * sizeof(double));}
    kdist = (double*)malloc(knum * sizeof(double));

    for (i = 0; i < (int)knum; i++)
    {
        ct = (unsigned)(cstep * i + 0.5);
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
                imm[d] += (unsigned)p_im[y][x].c[d];
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
                kdmin = 10.0;
                knm = 0;
                for (i = 0; i < (int)knum; i++)
                {
                    if (kdist[i] < kdmin)
                    {
                        kdmin = kdist[i];
                        knm = (unsigned)i;
                    }
                }
                for (j = 0; j < 3; j++)
                {
                    ksum[knm][j] += (double)p_im[y][x].c[j];
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
                    knm = (unsigned)i;
                }
            }
            if (knum - knum2 * 2 == 0)
            {
                val = (BYTE) ((knm >= knum2) ? 255 : 0);
                t = (unsigned)kpt[knum2][3];
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
    if (sn > 0) {threshold = (int)(st / sn + 0.5);}

    free(kdist);
    for (y = 0; y < (int)knum; y++) {free(kpt[y]);}
    free(kpt);
    for (y = 0; y < (int)knum; y++) {free(ksum[y]);}
    free(ksum);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTMscaleLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, unsigned radius, double sensitivity, double doverlay, double delta)
{
    unsigned whcp, y, x, l, i, j, y0, x0, y1, x1, blsz, rsz;
    double immean, st, kover, sensdiv, senspos, sensinv;
    unsigned pim, immin, immax, imt, cnth, cntw, level = 0;
    unsigned maskbl, maskover;
    WORD tim;
    int threshold = 0;

    whcp = height;
    whcp += width;
    whcp /= 2;
    blsz = 1;
    while (blsz < whcp)
    {
        level++;
        blsz *= 2;
    }
    rsz = 1;
    while (rsz < radius)
    {
        if (level > 1) {level--;}
        rsz *= 2;
    }
    immin = (unsigned)p_im[0][0].s;
    immax = immin;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            pim = (unsigned)p_im[y][x].s;
            if (pim < immin) {immin = pim;}
            if (pim > immax) {immax = pim;}
        }
    }
    immean = (double)(immax + immin);
    immean /= 2.0;
    immean += 0.5;
    tim = Byte3Clamp((int)immean);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            t_im[y][x] = tim;
        }
    }
    if (doverlay < 0) {doverlay = 0;}
    kover = doverlay + 1.0;
    if (sensitivity < 0)
    {
        sensitivity = -sensitivity;
        sensdiv = sensitivity;
        sensdiv += 1.0;
        sensinv = 1.0 / sensdiv;
        senspos = sensitivity / sensdiv;
    } else {
        sensdiv = sensitivity;
        sensdiv += 1.0;
        senspos = 1.0 / sensdiv;
        sensinv = sensitivity / sensdiv;
    }
    for (l = 0; l < level; l++)
    {
        cnth = (height + blsz - 1) / blsz;
        cntw = (width + blsz - 1) / blsz;
        maskbl = blsz;
        maskover = (unsigned)(kover * maskbl);
        for (i = 0; i < cnth; i++)
        {
            y0 = i * maskbl;
            y1 = y0 + maskover;
            if (y1 > height) {y1 = height;}
            for (j = 0; j < cntw; j++)
            {
                x0 = j * maskbl;
                x1 = x0 + maskover;
                if (x1 > width) {x1 = width;}

                immin = (unsigned)p_im[y0][x0].s;
                immax = immin;
                for (y = y0; y < y1; y++)
                {
                    for (x = x0; x < x1; x++)
                    {
                        pim = (unsigned)p_im[y][x].s;
                        if (pim < immin) {immin = pim;}
                        if (pim > immax) {immax = pim;}
                    }
                }
                immean = (double)(immax + immin);
                immean /= 2.0;
                immean *= sensinv;
                for (y = y0; y < y1; y++)
                {
                    for (x = x0; x < x1; x++)
                    {
                        imt = (unsigned)t_im[y][x];
                        imt *= senspos;
                        imt += immean;
                        imt += 0.5;
                        t_im[y][x] = Byte3Clamp((int)imt);
                    }
                }
            }
        }
        blsz /= 2;
    }
    st = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imt = (unsigned)t_im[y][x];
            imt += delta;
            t_im[y][x] = Byte3Clamp((int)imt);
            st += imt;
        }
    }
    st /= height;
    st /= width;
    st += 0.5;
    threshold = (int)st;

    return threshold;
}

///////////////////////////////////////////////////////////////////////////////

int IMTFilterTMscale (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned radius, double sensitivity, double doverlay, double delta)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTMscaleLayer (p_im, t_im, height, width, radius, sensitivity, doverlay, delta);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTNiblackLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    unsigned y, x, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, t, st = 0, sn = 0;
    int threshold = 0;
    int h = (int)height;
    int w = (int)width;
    WORD val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }
    lower_bound *= 3;
    upper_bound *= 3;

    for (y = 0; y < height; y++)
    {
        y1 = (int)y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = (int)(y + 1);
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = (int)x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = (int)(x + 1);
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
            imv /= n;
            imv -= (imm * imm);
            if (imv < 0) {imv = -imv;}
            imv = sqrt(imv);
            imx = (double)p_im[y][x].s;
            if (imx < (double)lower_bound)
            {
                t = (double)lower_bound;
            } else if (imx > (double)upper_bound)
            {
                t = (double)upper_bound;
            } else {
                t = (imm + sensitivity * imv + delta);
            }
            val = Byte3Clamp((int)(t + 0.5));
            st += t;
            sn++;
            t_im[y][x] = val;
        }
    }
    sn = (sn < 0.0 || sn > 0.0) ? sn : 1.0;
    threshold =  (int)(st / sn + 0.5);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTNiblack (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int lower_bound, int upper_bound, double delta)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTNiblackLayer (p_im, t_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTOtsuValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned im, y, x, i;
    double w = 0, u = 0, uT = 0;
    double histmean = 0.0;
    double work1, work2, work3 = 0.0;
    int threshold = 0;
    double histogram[768] = {0};

    for (i = 0; i < 768; i++)
    {
        histogram[i] = 0;
    }
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++)
        {
            im = (unsigned)p_im[y][x].s;
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
            threshold = (int)i;
        }
    }

    threshold -= 1;

    if(threshold <= 0)
    {
        threshold = (int)(histmean/2.0);
    }

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTOtsu (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width)
{
    int threshold = 0;

    threshold = IMTFilterTOtsuValue (p_im, height, width);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTQuadModValue (IMTpixel** p_im, unsigned height, unsigned width)
{
    int T, TG, Trw, Trb;
    int threshold = 0;

    TG = IMTFilterTBiModValue (p_im, height, width);
    Trw = IMTFilterTBiModValueBound (p_im, height, width, TG, 765);
    Trb = IMTFilterTBiModValueBound (p_im, height, width, 0, TG);
    T = IMTFilterTBiModValueBound (p_im, height, width, Trb, Trw);
    T += TG;
    T++;
    T /= 2;
    threshold = T;

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTQuadMod (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    int threshold = 0;

    threshold = IMTFilterTQuadModValue (p_im, height, width);
    threshold += delta;
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTRotValue (IMTpixel** p_im, unsigned height, unsigned width, bool weight)
{
    unsigned y, x, i, im;
    unsigned isz = 768, il = 0, ir = isz - 1;
    double wl = 0, wr = 0;
    double kw, wi;
    unsigned threshold = 0;
    double histogram[768] = {0};
    double hists = 0;

    for (i = 0; i < isz; i++)
    {
        histogram[i] = 0;
    }
    for (y = 0; y < height; y++)
    {
        for ( x = 0; x < width; x++)
        {
            im = (unsigned)p_im[y][x].s;
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

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTRot (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, bool weight)
{
    int threshold = 0;

    threshold = IMTFilterTRotValue (p_im, height, width, weight);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTSauvolaLayer (IMTpixel** p_im, WORD** t_im, unsigned height, unsigned width, int radius, double sensitivity, int dynamic_range, int lower_bound, int upper_bound, double delta)
{
    unsigned x, y, n;
    int y1, x1, y2, x2, yf, xf;
    double imx, imm, imv, ima, t, st = 0, sn = 0;
    int threshold = 0;
    int h = (int)height;
    int w = (int)width;
    WORD val;

    if (radius < 0) {radius = -radius;}
    if (upper_bound < lower_bound)
    {
        upper_bound += lower_bound;
        lower_bound = upper_bound - lower_bound;
        upper_bound -= lower_bound;
    }
    lower_bound *= 3;
    upper_bound *= 3;

    for (y = 0; y < height; y++)
    {
        y1 = (int)y;
        y1 -= radius;
        if (y1 < 0) {y1 = 0;}
        y2 = (int)(y + 1);
        y2 += radius;
        if (y2 > h) {y2 = h;}
        for (x = 0; x < width; x++)
        {
            x1 = (int)x;
            x1 -= radius;
            if (x1 < 0) {x1 = 0;}
            x2 = (int)(x + 1);
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
            imv /= n;
            imv -= (imm * imm);
            imv = (imv < 0) ? -imv : imv;
            imv = sqrt(imv);
            ima = 1.0 - imv / (double)dynamic_range;
            imx = (double)p_im[y][x].s;
            if (imx < (double)lower_bound)
            {
                t = (double)lower_bound;
            } else if (imx > (double)upper_bound)
            {
                t = (double)upper_bound;
            } else {
                t = imm * (1.0 - sensitivity * ima) + delta;
            }
            val = Byte3Clamp((int)(t + 0.5));
            st += t;
            sn++;
            t_im[y][x] = val;
        }
    }
    sn = (sn < 0.0 || sn > 0.0) ? sn : 1.0;
    threshold = (int)(st / sn + 0.5);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTSauvola (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int radius, double sensitivity, int dynamic_range, int lower_bound, int upper_bound, double delta)
{
    int threshold = 0;
    WORD** t_im = TLalloc(height, width);

    threshold = IMTFilterTSauvolaLayer (p_im, t_im, height, width, radius, sensitivity, dynamic_range, lower_bound, upper_bound, delta);
    threshold = IMTFilterThresholdLayer (p_im, t_im, d_im, height, width);

    TLfree(t_im, height);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTText (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, unsigned contour, unsigned radius)
{
    unsigned wr, ws, s, sn, n, z;
    int y, x, y0, x0, yk, xk, yf, xf, ys, xs, im, imf, ims, imd;
    BYTE val;
    int h = (int)height;
    int w = (int)width;
    int threshold = 0;

    WORD** b_im = TLalloc(height, width);

    wr = 2 * radius;
    ws = wr * wr - 1;

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
            im = (int)p_im[y][x].s;
            for (yf = y0; yf < yk; yf++)
            {
                for (xf = x0; xf < xk; xf++)
                {
                    imf = (int)p_im[yf][xf].s;
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
                                        ims = (int)p_im[ys][xs].s;
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
            b_im[y][x] = (WORD)n;
        }
    }
    z /= 2;
    threshold = (int)(384 * z / ws);
    for (y = 0; y < h; y++ )
    {
        for (x = 0; x < w; x++)
        {
            im = (int)b_im[y][x];
            val = (BYTE) ((im >= (int)z) ? 255 : 0);
            d_im[y][x] = val;
        }
    }

    TLfree(b_im, height);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTTsaiValue (IMTpixel** p_im, unsigned height, unsigned width, int shift)
{

    unsigned y, x, cn = 768, i, im;
    int threshold = 0;
    int histogram[768] = {0};
    double criterion = 0.0;
    double m1, m2, m3;
    double cd, c0, c1, z0, z1, pd, p0, p1;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = (unsigned)p_im[y][x].s;
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

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTTsai (IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int shift)
{
    int threshold = 0;

    threshold = IMTFilterTTsaiValue (p_im, height, width, shift);
    threshold = IMTFilterThreshold (p_im, d_im, height, width, threshold);

    return threshold;
}

////////////////////////////////////////////////////////////////////////////////

int IMTFilterTWhiteRohrer(IMTpixel** p_im, BYTE** d_im, unsigned height, unsigned width, int x_lookahead, int y_lookahead, int bias_mode, int bias_factor, int f_factor, int g_factor)
{
    int x, y;
    int h = (int)height;
    int w = (int)width;
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
    int nZ = (int)(2 * width + 1), n;
    int Z[nZ];
    int st = 0, sn = 0;

    x_lookahead = x_lookahead % width;

    if (bias_mode == 0)
    {
        sum = 0;
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                im = (unsigned)p_im[y][x].s;
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
                im = (unsigned)p_im[y][x].s;
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

    for(n = 0; n < nZ; ++n) {Z[n] = 0;}
    Z[0] = prevY = (int)mu;

    for (y = 0; y< 1 + y_lookahead; y++)
    {
        if (y < y_lookahead) {t = (int)width;} else {t = x_lookahead;}
        for (x=0; x < t; x++)
        {
            im = (unsigned)p_im[y][x].s;
            im += 2;
            im /= 3;
            u = (int)im;
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
            threshold = (unsigned)((bias_factor < 0) ? -bias_factor : bias_factor);
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
            im = (unsigned)p_im[y][x].s;
            im += 2;
            im /= 3;
            val = (BYTE) ((im < threshold) ? 0 : 255);
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
                im = (unsigned)p_im[y_ahead][x_ahead].s;
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
    if (sn > 0) {threshold = (unsigned)Byte3Clamp((int)(3 * st / sn));}

    return (int)threshold;
}

////////////////////////////////////////////////////////////////////////////////
