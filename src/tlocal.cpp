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
    printf("Local adaptive thresholding image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterTSauvolaUsage()
{
    printf("Usage : imthreshold-tlocal [options] <input_image> <output_image>(BW)\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'abutaleb'\n");
    printf("                    'bernsen'\n");
    printf("                    'bimod'\n");
    printf("                    'blur'\n");
    printf("                    'chistian'\n");
    printf("                    'edge'\n");
    printf("                    'niblack'\n");
    printf("                    'sauvola' (default)\n");
    printf("          -c N    contrast limit (int, optional, default = 128)\n");
    printf("          -d N.N  delta (double, optional, default = -5.0)\n");
    printf("          -i      invert (bool, optional, default = false)\n");
    printf("          -l N    lower bound (int, optional, default = 0)\n");
    printf("          -g N    dynamic range (int, optional, default = 128)\n");
    printf("          -n      norm (bool, optional, default = false)\n");
    printf("          -r N    radius (int, optional, default = 7)\n");
    printf("          -s N.N  sensitivity (double, optional, default = 0.5)\n");
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
    double sensitivity = 0.5;
    unsigned contrast_limit = 128;
    int dynamic_range = 128;
    int lower_bound = 0;
    int upper_bound = 255;
    double delta = -5.0;
    bool finv = false;
    bool fnorm = false;
    bool fmirror = false;
    bool fhelp = false;
    int threshold = 0;
    char *namefilter;
    namefilter="sauvola";
    while ((opt = getopt(argc, argv, ":f:c:d:ig:l:nr:s:u:zh")) != -1)
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
            case 'i':
                finv = true;
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
            unsigned y;
            IMTpixel** p_im;
            p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
            for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
            BYTE** d_im;
            d_im = (BYTE**)malloc(height * sizeof(BYTE*));
            for (y = 0; y < height; y++) {d_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

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
            if (strcmp(namefilter, "abutaleb") == 0)
            {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTAbutaleb(p_im, d_im, height, width, radius);
            } else if (strcmp(namefilter, "bernsen") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Contrast= %d\n", contrast_limit);
                threshold = IMTFilterTBernsen(p_im, d_im, height, width, radius, contrast_limit);
            } else if (strcmp(namefilter, "bimod") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTBiModRegion ( p_im, d_im, height, width, delta, radius);
            } else if (strcmp(namefilter, "blur") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** b_im;
                b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTFilterGaussBlur (p_im, b_im, height, width, radius);
                IMTFilterSCompare (p_im, b_im, height, width);
                for (y = 0; y < height; y++){free(b_im[y]);}
                free(b_im);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTBiMod (p_im, d_im, height, width, delta);
            } else if (strcmp(namefilter, "chistian") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTChistian(p_im, d_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
            } else if (strcmp(namefilter, "edge") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** b_im;
                b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTFilterGaussBlur (p_im, b_im, height, width, radius);
                IMTFilterSEdge (p_im, b_im, height, width);
                for (y = 0; y < height; y++){free(b_im[y]);}
                free(b_im);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTBiMod (p_im, d_im, height, width, delta);
            } else if (strcmp(namefilter, "niblack") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                threshold = IMTFilterTNiblack(p_im, d_im, height, width, radius, sensitivity, lower_bound, upper_bound, delta);
            } else {
                printf("Filter= sauvola\n");
                printf("Sensitivity= %f\n", sensitivity);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);
                printf("Delta= %f\n", delta);
                printf("Dynamic= %d\n", dynamic_range);
                threshold = IMTFilterTSauvola(p_im, d_im, height, width, radius, sensitivity, dynamic_range, lower_bound, upper_bound, delta);
            }
            printf("Threshold= %d\n", threshold / 3);
            for (y = 0; y < height; y++){free(p_im[y]);}
            free(p_im);
            if (finv)
            {
                printf("Invert= true\n");
                IMTFilterInvertBW(d_im, height, width);
            }
            dst_dib = FreeImage_Allocate(width, height, 1);
            ImthresholdSetDataBW(dst_dib, d_im);
            for (y = 0; y < height; y++){free(d_im[y]);}
            free(d_im);

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
