//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

/////////////////////////////////////////////////////////////////////////////////////////////

float IMTFilterFindSkew (IMTpixel** p_im, unsigned height, unsigned width)
{
    unsigned y, x;
    int t, u;
    int h = (int)height;
    float kmin, kmax, kt, dkp, dkt, yt, smin, smax, st, s;
    float fskew;

    dkp = 1.0 / sqrt((float)(height * height + width * width));
    kmin = -0.5;
    kt = kmin;
    st = 0;
    for (y = 0; y < height; y++)
    {
        s = 0;
        yt = (float)y;
        for (x = 0; x < width; x++)
        {
            t = (int)yt;
            if (t >= 0 && t < h)
            {
                u = (int)p_im[t][x].s;
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
        yt = (float)y;
        for (x = 0; x < width; x++)
        {
            t = (int)yt;
            if (t >= 0 && t < h)
            {
                u = (int)p_im[t][x].s;
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
            yt = (float)y;
            for (x = 0; x < width; x++)
            {
                t = (int)yt;
                if (t >= 0 && t < h)
                {
                    u = (int)p_im[t][x].s;
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

void IMTFilterRotate (IMTpixel** p_im, IMTpixel** d_im, unsigned height, unsigned width, float angle)
{
    unsigned y, x;
    float yt, xt, yr, xr, ktc, kts;

    angle /= RADGRD;
    kts = sin(angle);
    ktc = cos(angle);
    for (y = 0; y < height; y++)
    {
        yt = (float)y;
        yt -= height / 2;
        for (x = 0; x < width; x++)
        {
            xt = (float)x;
            xt -= (float)width / 2;
            yr = ktc * yt - kts * xt;
            yr += (float)height / 2;
            xr = ktc * xt + kts * yt;
            xr += (float)width / 2;
            if (yr >= 0.0 && yr < height && xr >= 0.0 && xr < width)
            {
                d_im[y][x] = IMTinterpolation(p_im, height, width, yr, xr);
            } else {
                d_im[y][x] = IMTset(255,255,255);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
