//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

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
            if ((unsigned)m_im[y][x] != value)
            {
                gim = IMTset(255, 255, 255);
            }
            g_im[y][x] = gim;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterSeparateBGFGL (IMTpixel** p_im, BYTE** m_im, IMTpixel** fg_im, IMTpixel** bg_im, unsigned height, unsigned width, unsigned bgs, unsigned fgs, unsigned level, float doverlay)
{
    unsigned widthbg = (width + bgs - 1) / bgs;
    unsigned heightbg = (height + bgs - 1) / bgs;
    unsigned widthfg = (widthbg + fgs - 1) / fgs;
    unsigned heightfg = (heightbg + fgs - 1) / fgs;
    unsigned whcp, y, x, d, l, i, j, y0, x0, y1, x1, y0b, x0b, y1b, x1b, yb, xb, yf, xf, blsz;
    BYTE fgbase, bgbase, mim;
    unsigned cnth, cntw;
    IMTpixel pim, fgim, bgim;
    float fgdist, bgdist, kover, fgpart, bgpart;
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
        maskover = (unsigned)(kover * maskbl);
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
                fgdist = (float)IMTdist(pim, fgim);
                bgdist = (float)IMTdist(pim, bgim);
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
        y1 = (((y0 + fgs) < heightbg) ? (y0 + fgs) : heightbg);
        for (x = 0; x < widthfg; x++)
        {
            x0 = x * fgs;
            x1 = (((x0 + fgs) < widthbg) ? (x0 + fgs) : widthbg);
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
            fgdist = (float)IMTdist(pim, fgim);
            bgdist = (float)IMTdist(pim, bgim);
            m_im[y][x] = (BYTE)((fgdist < bgdist) ? 0 : 255);
        }
    }

    IMTfree(fgt_im, heightbg);
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterSeparateDelta (IMTpixel** p_im, BYTE** m_im, IMTpixel** g_im, unsigned height, unsigned width, int value, float kdelta)
{
    unsigned y, x, d;
    int im, img, imd;
    float simd, simp;
    IMTpixel gim;
    BYTE val;

    simd = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            im = (int)m_im[y][x];
            for (d = 0; d < 3; d++)
            {
                img = (int)p_im[y][x].c[d];
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
            im = (int)m_im[y][x];
            simp = 0;
            for (d = 0; d < 3; d++)
            {
                img = (int)p_im[y][x].c[d];
                imd = im - img;
                if (imd < 0) {imd = -imd;}
                simp += imd;
            }
            simp /= 3;
            simp *= value;
            gim = p_im[y][x];
            if (simp > simd)
            {
				val = ByteClamp(im);
                gim = IMTset(val, val, val);
            }
            g_im[y][x] = gim;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
