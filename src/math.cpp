//    Zlib license
//
// BGFG separate based Multi-scale separate.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterMathTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Math opertion of two image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterMathUsage()
{
    printf("Usage : imthreshold-math [options] <input_file> <input_file> <output_file>\n\n");
    printf("options:\n");
    printf("          -m str  name metod:\n");
    printf("                    'average'\n");
    printf("                    'distance'\n");
    printf("                    'divide'\n");
    printf("                    'minus'\n");
    printf("                    'multiply'\n");
    printf("                    'norm'\n");
    printf("                    'plus'\n");
    printf("                    'threshold (default)'\n");
    printf("          -h      this help\n");
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif // FREEIMAGE_LIB

    int opt;
    bool fhelp = false;
    char *namefilter;
    namefilter="threshold";
    while ((opt = getopt(argc, argv, ":m:h")) != -1)
    {
        switch(opt)
        {
            case 'm':
                namefilter = optarg;
                break;
            case 'h':
                fhelp = true;
                break;
            case ':':
                printf("option needs a value\n");
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
        }
    }

    ImthresholdFilterMathTitle ();

    if(optind + 3 > argc || fhelp)
    {
        ImthresholdFilterMathUsage ();
        return 0;
    }
    const char *src_filename = argv[optind];
    const char *math_filename = argv[optind + 1];
    const char *dst_filename = argv[optind + 2];

    FreeImage_SetOutputMessage (FreeImageErrorHandler);

    printf ("Input= %s\n", src_filename);
    FIBITMAP *dib = ImthresholdGenericLoader (src_filename, 0);
    if (dib)
    {
        if (FreeImage_GetImageType(dib) == FIT_BITMAP)
        {
            unsigned width = FreeImage_GetWidth (dib);
            unsigned height = FreeImage_GetHeight (dib);
            unsigned y;

            IMTpixel** p_im;
            p_im = (IMTpixel**)malloc (height * sizeof(IMTpixel*));
            for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc (width * sizeof(IMTpixel));}

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);

            printf("Input= %s\n", math_filename);
            FIBITMAP *dib = ImthresholdGenericLoader(math_filename, 0);
            if (dib)
            {
                if (FreeImage_GetImageType(dib) == FIT_BITMAP)
                {
                    unsigned widthm = FreeImage_GetWidth (dib);
                    unsigned heightm = FreeImage_GetHeight (dib);

                    if ((height == heightm) && (width == widthm))
                    {
                        FIBITMAP* dst_dib;

                        IMTpixel** m_im;
                        m_im = (IMTpixel**)malloc (height * sizeof(IMTpixel*));
                        for (y = 0; y < height; y++) {m_im[y] = (IMTpixel*)malloc (width * sizeof(IMTpixel));}

                        ImthresholdGetData(dib, m_im);
                        
                        if (strcmp(namefilter, "average") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathAverage (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        }
                        else if (strcmp(namefilter, "distance") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathDistance (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        }
                        else if (strcmp(namefilter, "divide") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathDivide (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        }
                        else if (strcmp(namefilter, "minus") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathMinus (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        }
                        else if (strcmp(namefilter, "multiply") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathMultiply (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        }
                        else if (strcmp(namefilter, "norm") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathNorm (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        }
                        else if (strcmp(namefilter, "plus") == 0)
                        {
                            printf("Filter= %s\n", namefilter);

                            IMTFilterMathPlus (p_im, m_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 24);
                            ImthresholdSetData (dst_dib, p_im);
                        } else {
                            printf("Filter= threshold\n");

                            BYTE** d_im;
                            d_im = (BYTE**)malloc(height * sizeof(BYTE*));
                            for (y = 0; y < height; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

                            IMTFilterMathThreshold (p_im, m_im, d_im, height, width);
                            dst_dib = FreeImage_Allocate (width, height, 1);
                            ImthresholdSetDataBW (dst_dib, d_im);
                            
                            for (y = 0; y < height; y++){free(d_im[y]);}
                            free(d_im);
                        }
                        FreeImage_Unload(dib);
                        for (y = 0; y < height; y++){free(m_im[y]);}
                        free(m_im);

                        if (dst_dib)
                        {
                            FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename (dst_filename);
                            if(out_fif != FIF_UNKNOWN)
                            {
                                FreeImage_Save (out_fif, dst_dib, dst_filename, 0);
                                printf("Output= %s\n", dst_filename);
                            }
                            FreeImage_Unload(dst_dib);
                        }
                    } else {
                        printf("%s\n", "Size math uncorect.");
                        FreeImage_Unload (dib);
                    }
                } else {
                    printf("%s\n", "Unsupported color mode.");
                    FreeImage_Unload (dib);
                }
            }
            for (y = 0; y < height; y++){free(p_im[y]);}
            free(p_im);

            printf("\n");
        } else {
            printf("%s\n", "Unsupported format type.");
            FreeImage_Unload (dib);
        }
    }

    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise ();
#endif // FREEIMAGE_LIB

    return 0;
}
