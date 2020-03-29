//    Zlib license
//
// Local adaptive thresholding image.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTSauvolaTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Local adaptive zero-layer image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterTSauvolaUsage()
{
    printf("Usage : imthreshold-tlayer [options] <input_image> <output_image>\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'bernsen'\n");
    printf("                    'bimod'\n");
    printf("                    'chistian'\n");
    printf("                    'gravure'\n");
    printf("                    'mscale'\n");
    printf("                    'niblack'\n");
    printf("                    'sauvola' (default)\n");
    printf("                    'size'\n");
    printf("          -c N    contrast limit (int, optional, default = 128)\n");
    printf("          -d N.N  delta (float, optional, default = -5.0)\n");
    printf("          -g N    dynamic range (int, optional, default = 128)\n");
    printf("          -l N    lower bound (int, optional, default = 0)\n");
    printf("          -n      norm (bool, optional, default = false)\n");
    printf("          -o N.N  overlay (float, optional, default = 0.5)\n");
    printf("          -r N    radius (int, optional, default = 7)\n");
    printf("          -s N.N  sensitivity (float, optional, default = 0.2)\n");
    printf("          -u N    upper bound (int, optional, default = 255)\n");
    printf("          -z      mirror of mean (bool, optional, default = false)\n");
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
    int radius = 7;
    float doverlay = 0.5;
    float sensitivity = 0.2;
    unsigned contrast_limit = 128;
    int dynamic_range = 128;
    int lower_bound = 0;
    int upper_bound = 255;
    float delta = -5.0;
    bool fnorm = false;
    bool fmirror = false;
    bool fhelp = false;
    int threshold = 0;
    char *namefilter;
    namefilter="sauvola";
    while ((opt = getopt(argc, argv, ":f:c:d:g:l:no:r:s:u:zh")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case 'c':
                contrast_limit = atof(optarg);
                break;
            case 'd':
                delta = atof(optarg);
                break;
            case 'g':
                dynamic_range = atof(optarg);
                break;
            case 'l':
                lower_bound = atof(optarg);
                break;
            case 'n':
                fnorm = true;
                break;
            case 'o':
                doverlay = atof(optarg);
                break;
            case 'r':
                radius = atof(optarg);
                break;
            case 's':
                sensitivity = atof(optarg);
                break;
            case 'u':
                upper_bound = atof(optarg);
                break;
            case 'z':
                fmirror = true;
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

    ImthresholdFilterTSauvolaTitle();

    if(optind + 2 > argc || fhelp || radius <= 0)
    {
        ImthresholdFilterTSauvolaUsage();
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
            WORD** t_im = TLalloc(height, width);
            IMTpixel** d_im = IMTalloc(height, width);

            printf("Radius= %d\n", radius);

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (fmirror)
            {
                IMTFilterSMirror(p_im, height, width);
                printf("Mirror= true\n");
            }
            if (fnorm)
            {
                threshold = IMTFilterSNorm(p_im, height, width);
                printf("Norm= %d\n", threshold);
            }
            if (strcmp(namefilter, "bernsen") == 0)
            {
                printf("Filter= %s\n", namefilter);
                printf("Contrast= %d\n", contrast_limit);
                threshold = IMTFilterTBernsenLayer(p_im, t_im, height, width, radius, contrast_limit);
            } else if (strcmp(namefilter, "bimod") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTBiModLayer (p_im, t_im, height, width, radius, sensitivity, delta);
            } else if (strcmp(namefilter, "chistian") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTChistianLayer(p_im, t_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
            } else if (strcmp(namefilter, "gravure") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTGravureLayer(p_im, t_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
            } else if (strcmp(namefilter, "mscale") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Overlay= %f\n", doverlay);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTMscaleLayer (p_im, t_im, height, width, radius, sensitivity, doverlay, delta);
            } else if (strcmp(namefilter, "niblack") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTNiblackLayer(p_im, t_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
            } else if (strcmp(namefilter, "size") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTSizeLayer(p_im, t_im, height, width, radius, lower_bound, upper_bound, (int)(delta + 0.5));
            } else {
                printf("Filter= sauvola\n");
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                printf("Dynamic= %d\n", dynamic_range);
                threshold = IMTFilterTSauvolaLayer(p_im, t_im, height, width, radius, sensitivity, dynamic_range, lower_bound, upper_bound, delta);
            }
            printf("Threshold= %d\n", threshold / 3);
            IMTfree(p_im, height);
            IMTFilterTLayerToImg (t_im, d_im, height, width);
            TLfree(t_im, height);
            dst_dib = FreeImage_Allocate(width, height, 24);
            ImthresholdSetData(dst_dib, d_im);
            IMTfree(d_im, height);

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
