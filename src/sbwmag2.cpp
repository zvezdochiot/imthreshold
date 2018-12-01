//    Zlib license
//
// Magnification BW image.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSBWMag2Title()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Magnification BW image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterSBWMag2Usage()
{
    printf("Usage : imthreshold-sbwmag2 [options] <input_file>(BW) <output_file>(BW)\n\n");
    printf("options:\n");
    printf("          -r      reduce (bool, optional, default = false)\n");
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
    bool freduce = 0;
    bool fhelp = false;
    int threshold = 0;
    while ((opt = getopt(argc, argv, ":rh")) != -1)
    {
        switch(opt)
        {
            case 'r':
                freduce = true;
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

    ImthresholdFilterSBWMag2Title();

    if(optind + 2 > argc || fhelp)
    {
        ImthresholdFilterSBWMag2Usage();
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
            if (FreeImage_GetBPP(dib) == 1)
            {
                FIBITMAP* dst_dib;
                unsigned width = FreeImage_GetWidth(dib);
                unsigned height = FreeImage_GetHeight(dib);
                unsigned width2, height2;
                if (freduce)
                {
                    width2 = (width + 1) / 2;
                    height2 = (height + 1) / 2;
                } else {
                    width2 = width * 2;
                    height2 = height * 2;
                }
                printf("Width= %d\n", width2);
                printf("Height= %d\n", height2);
                BYTE** d_im = BWalloc(height, width);
                BYTE** r_im = BWalloc(height2, width2);

                ImthresholdGetDataBW(dib, d_im);
                FreeImage_Unload(dib);
                if (freduce)
                {
                    threshold = IMTFilterSBWReduce2(d_im, r_im, height, width, height2, width2);
                    printf("Reduce= %d\n", threshold);
                } else {
                    threshold = IMTFilterSBWMag2(d_im, r_im, height, width, height2, width2);
                    printf("Mag= %d\n", threshold);
                }

                BWfree(d_im, height);
                dst_dib = FreeImage_Allocate(width2, height2, 1);
                ImthresholdSetDataBW(dst_dib, r_im);
                BWfree(r_im, height2);

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
                printf("%s\n", "Unsupported color mode.");
                FreeImage_Unload(dib);
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
