//    Zlib license
//
// Half Reverse Interpolate Scale image (HRIS).
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSHRISTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Half Reverse Interpolate Scale image (HRIS).\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterSHRISUsage()
{
    printf("Usage : imthreshold-shris [options] <input_file> <output_file>\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'hris'\n");
    printf("                    'gsample'\n");
    printf("          -m      mode (int, optional, default = 2, {2,3})\n");
    printf("          -r      reduce scale (bool, optional)\n");
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
    unsigned smode = 2;
    bool reduce = false;
    bool fhelp = false;
    char *namefilter;
    namefilter="hris";
    while ((opt = getopt(argc, argv, ":f:m:rh")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case 'm':
                smode = atof(optarg);
                break;
            case 'r':
                reduce = true;
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

    ImthresholdFilterSHRISTitle();

    if(optind + 2 > argc || fhelp)
    {
        ImthresholdFilterSHRISUsage();
        return 0;
    }
    const char *src_filename = argv[optind];
    const char *output_filename = argv[optind + 1];

    if (smode < 2) {smode = 2;}
    if (strcmp(namefilter, "hris") == 0)
    {
        if (smode > 3) {smode = 3;}
    }

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
            unsigned width2;
            unsigned height2;
            printf("Mode= %d\n", smode);
            width2 = width * smode;
            height2 = height * smode;
            if (reduce > 0)
            {
                width2 = (width + smode - 1) / smode;
                height2 = (height + smode - 1) / smode;
            } else {
                width2 = width * smode;
                height2 = height * smode;
            }
            printf("Width= %d\n", width2);
            printf("Height= %d\n", height2);
            IMTpixel** p_im = IMTalloc(height, width);
            IMTpixel** d_im = IMTalloc(height2, width2);

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (reduce)
            {
                printf("Scale= Reduce.\n");
                IMTFilterSReduce(p_im, d_im, height, width, smode);
            } else {
                if (strcmp(namefilter, "gsample") == 0)
                {
                    printf("Scale= Up GSample.\n");
                    IMTFilterSGSampleUp(p_im, d_im, height, width, smode);
                } else {
                    printf("Scale= Up HRIS.\n");
                    IMTFilterSHRIS(p_im, d_im, height, width, smode);
                }
            }

            IMTfree(p_im, height);
            dst_dib = FreeImage_Allocate(width2, height2, 24);
            ImthresholdSetData(dst_dib, d_im);
            IMTfree(d_im, height2);

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

