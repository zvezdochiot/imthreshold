//    Zlib license
//
// AutoInvert colors filter image.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterAutoInvTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("AutoInvert colors filter image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterAutoInvUsage()
{
    printf("Usage : imthreshold-fautoinv [options] <input_file> <output_file>\n\n");
    printf("options:\n");
    printf("          -p N.N  pass (double, optional, default = 0.125)\n");
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
    double pass = 0.125;
    bool fhelp = false;
    while ((opt = getopt(argc, argv, ":p:h")) != -1)
    {
        switch(opt)
        {
            case 'p':
                pass = atof(optarg);
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

    ImthresholdFilterAutoInvTitle();

    if(optind + 2 > argc || fhelp > 0)
    {
        ImthresholdFilterAutoInvUsage();
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
            unsigned bpp = FreeImage_GetBPP(dib);
            IMTinfo p_info;
            double imwbf = 0;

            IMTpixel** p_im = IMTalloc(height, width);

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            p_info = IMTFilterInfo(p_im, height, width, bpp);
            imwbf = p_info.wb + pass * p_info.std;
            printf("Mean= %f\n", p_info.mean);
            printf("Std= %f\n", p_info.std);
            printf("W/B= %f\n", p_info.wb);
            printf("Pass= %f\n", pass);
            if (imwbf < 0.5)
            {
                printf("Status= Invert\n");
                IMTFilterInvert(p_im, height, width);
            } else {
                printf("Status= ReWrite\n");
            }
            dst_dib = FreeImage_Allocate(width, height, 24);
            ImthresholdSetData(dst_dib, p_im);
            IMTfree(p_im, height);

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

