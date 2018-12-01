//    Zlib license
//
// D-algoritm thresholding image.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

// D-algoritm thresholding image.

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTDalgTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("D-algoritm thresholding image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterTDalgUsage()
{
    printf("Usage : imthreshold-tdalg [options] <input_file> <output_file>(BW)\n\n");
    printf("options:\n");
    printf("          -d N    delta (int, optional, default = 0)\n");
    printf("          -n      norm (bool, optional, default = false)\n");
    printf("          -r N    region size (int, optional, default = 4)\n");
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
    int region_size = 4;
    int delta = 0;
    bool fnorm = false;
    bool fhelp = false;
    int threshold = 0;
    while ((opt = getopt(argc, argv, ":d:nr:h")) != -1)
    {
        switch(opt)
        {
            case 'd':
                delta = atof(optarg);
                break;
            case 'n':
                fnorm = true;
                break;
            case 'r':
                region_size = atof(optarg);
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

    ImthresholdFilterTDalgTitle();

    if(optind + 2 > argc || fhelp || region_size <= 0)
    {
        ImthresholdFilterTDalgUsage();
        return 0;
    }
    const char *src_filename = argv[optind];
    const char *output_filename = argv[optind + 1];

    FreeImage_SetOutputMessage(FreeImageErrorHandler);

    printf("Input= %s\n", src_filename);
    FIBITMAP *dib = ImthresholdGenericLoader(src_filename, 0);
    if (dib)
    {
        if (FreeImage_GetImageType(dib) == FIT_BITMAP)
        {
            FIBITMAP* dst_dib;
            unsigned width = FreeImage_GetWidth(dib);
            unsigned height = FreeImage_GetHeight(dib);

            IMTpixel** p_im = IMTalloc(height, width);
            BYTE** d_im = BWalloc(height, width);

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (fnorm)
            {
                threshold = IMTFilterSNorm(p_im, height, width);
                printf("Norm= %d\n", threshold);
            }
            printf("Region= %d\n", region_size);
            printf("Delta= %d\n", delta);
            threshold = IMTFilterTDalg(p_im, d_im, height, width, region_size, delta);
            printf("Threshold= %d\n", threshold / 3);
            IMTfree(p_im, height);
            dst_dib = FreeImage_Allocate(width, height, 1);
            ImthresholdSetDataBW(dst_dib, d_im);
            BWfree(d_im, height);

            if (dst_dib)
            {
                FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
                if(out_fif != FIF_UNKNOWN)
                {
                    FreeImage_Save(out_fif, dst_dib, output_filename, 0);
                    printf("Output= %s\n\n", output_filename);
                }
                FreeImage_Unload(dst_dib);
            }
        } else {
            printf("%s\n", "Unsupported format type.");
            FreeImage_Unload(dib);
        }
    }

    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB

    return 0;
}
