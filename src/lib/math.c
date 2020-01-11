//  Zlib license
//
// ImThreshold library.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>
//  Homepage: https://sourceforge.net/projects/imthreshold/

#include "../imthreshold.h"

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathAverage (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                im += imm;
                im /= 2;
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathDistance (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                im -= imm;
                if (im < 0) {im = -im;}
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathDivide (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, d;
    double im, imm;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = (double)p_im[y][x].c[d];
                im++;
                imm = (double)m_im[y][x].c[d];
                imm++;
                im /= imm;
                im *= 256;
                im += delta;
                im -= 0.5;
                p_im[y][x].c[d] = ByteClamp((int)im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathGeometric (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, d;
    double im, imm;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = (double)p_im[y][x].c[d];
                im++;
                imm = (double)m_im[y][x].c[d];
                imm++;
                im *= imm;
                im = sqrt(im);
                im += delta;
                im -= 0.5;
                p_im[y][x].c[d] = ByteClamp((int)im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathHarmonic (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, d;
    double im, imm;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = (double)p_im[y][x].c[d];
                im++;
                imm = (double)m_im[y][x].c[d];
                imm++;
                im = 2.0 / (1.0 / im + 1.0 /imm);
                im += delta;
                im -= 0.5;
                p_im[y][x].c[d] = ByteClamp((int)im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathMax (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                if (imm > im) {im = imm;}
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathMin (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                if (imm < im) {im = imm;}
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathMinus (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                im = im - imm;
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathMirror (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                imm -= im;
                im -= imm;
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathMultiply (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, d;
    double im, imm;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            for (d = 0; d < 3; d++)
            {
                im = (double)p_im[y][x].c[d];
                im++;
                imm = (double)m_im[y][x].c[d];
                imm++;
                im *= imm;
                im /= 256.0;
                im += delta;
                im -= 0.5;
                p_im[y][x].c[d] = ByteClamp((int)im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathNorm (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x, d;
    double im, imm, ims, k;

    ims = 0;
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            imm = (double)m_im[y][x].s;
            ims += imm;
        }
    }
    ims /= (double)height;
    ims /= (double)width;
    ims++;
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            imm = (double)m_im[y][x].s;
            im++;
            k = imm / ims;
            for (d = 0; d < 3; d++)
            {
                im = (double)p_im[y][x].c[d];
                im *= k;
                im -= 0.5;
                im += delta;
                p_im[y][x].c[d] = ByteClamp((int)im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathPlus (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width, int delta)
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
                imm = (int)m_im[y][x].c[d];
                im = im + imm;
                im += delta;
                p_im[y][x].c[d] = ByteClamp(im);
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

double IMTFilterMathSharpenBadMetric (IMTpixel** p_im, IMTpixel** m_im, unsigned height, unsigned width)
{
    unsigned y, x, d, y1, x1, y2, x2, yf, xf, n;
    double im, imm, imf1, imf2, ims1, ims2, imd, imd1, imd2, imdc, imsd, imsd1, imsd2, imsdc, k332, sharpenbadmetric;

    imsd = 0;
    imsd1 = 0;
    imsd2 = 0;
    imsdc = 0;
    k332 = (3 * 3 * 2 + 1);
    k332 /= (3 * 3 * 2 - 1);
    for (y = 0; y < height; y++)
    {
        y1 = ((y > 1) ? (y - 1) : 0);
        y2 = (((y + 1) < height) ? (y + 2) : height);
        for (x = 0; x < width; x++)
        {
            x1 = ((x > 1) ? (x - 1) : 0);
            x2 = (((x + 1) < width) ? (x + 2) : width);
            for (d = 0; d < 3; d++)
            {
                im = (double)p_im[y][x].c[d];
                imm = (double)m_im[y][x].c[d];
                ims1 = 0;
                ims2 = 0;
                n = 0;
                for (yf = y1; yf < y2; yf++)
                {
                    for (xf = x1; xf < x2; xf++)
                    {
                        imf1 = (double)p_im[yf][xf].c[d];
                        imf2 = (double)m_im[yf][xf].c[d];
                        ims1 += imf1;
                        ims2 += imf2;
                        n++;
                    }
                }
                if (n > 0)
                {
                    ims1 /= n;
                    ims2 /= n;
                }
                imd1 = im - ims1;
                imd2 = imm - ims2;
                im += imd1;
                imm += imd2;
                imd = im - imm;
                imf1 = (double)m_im[y][x].c[d];
                imf1 += imd;
                p_im[y][x].c[d] = ByteClamp((int)(imf1 + 0.5));
                imd1 /= 255.0;
                imd2 /= 255.0;
                imd /= 255.0;
                imd *= imd;
                imdc = imd1 * imd2;
                imd1 *= imd1;
                imd2 *= imd2;
                imsd += imd;
                imsd1 += imd1;
                imsd2 += imd2;
                imsdc += imdc;
            }
        }
    }
    imsd2 *= imsd1;
    if (imsd2 < 0.0 || imsd2 > 0.0)
    {
        imsd /= imsd2;
        imsd *= imsdc;
        imsd *= 2.0;
    } else {
        imsd /= (double)height;
        imsd /= (double)width;
        imsd /= 3.0;
    }
    if (imsd < 0) {imsd = -imsd;}
    imsd = sqrt(imsd);
    imsd = -imsd;
    imsd *= exp(-1);
    imsd += k332;
    sharpenbadmetric = imsd;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < 3; d++)
            {
                imd = (double)p_im[y][x].c[d];
                imd -= (double)m_im[y][x].c[d];
                imd = (imd < 0) ? -imd : imd;
                p_im[y][x].c[d] = ByteClamp((int)(imd + 0.5));
            }
            p_im[y][x] = IMTcalcS (p_im[y][x]);
        }
    }
    return sharpenbadmetric;
}

///////////////////////////////////////////////////////////////////////////////

void IMTFilterMathThreshold (IMTpixel** p_im, IMTpixel** m_im, BYTE** d_im, unsigned height, unsigned width, int delta)
{
    unsigned y, x;
    int im, threshold;
    BYTE val;

    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            im = (int)p_im[y][x].s;
            im += delta;
            threshold = (int)m_im[y][x].s;
            val = (BYTE) ((im >= threshold) ? 255 : 0 );
            d_im[y][x] = val;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
