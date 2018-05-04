//    Zlib license
//
// Despeckle filter (for BW only).
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterDespeckleTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Despeckle filter (for BW only).\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterDespeckleUsage()
{
    printf("Usage : imthreshold-fdespeckle [options] <input_image>(BW) <output_image>(BW)\n\n");
    printf("options:\n");
    printf("          -m str  name method:\n");
    printf("                    'hist'\n");
    printf("                    'invert'\n");
    printf("                    'mag2'\n");
    printf("                    'neuro'\n");
    printf("                    'simple' (default)\n");
    printf("          -a N    aperture size (int, optional, default = 3)\n");
    printf("          -i      invert (bool, optional, default = false)\n");
    printf("          -k N.N  lambda neuro learnen (double, optional, default = 0.1)\n");
    printf("          -l N    learnen number (int, optional, default = 1)\n");
    printf("          -h      this help\n");
}

// ----------------------------------------------------------

int main(int argc, char *argv[])
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif // FREEIMAGE_LIB

    int opt;
    unsigned Ksize = 3;
    double lambda = 0.1;
    unsigned lnum = 1;
    bool finv = false;
    bool fhelp = false;
    char *namefilter;
    namefilter="simple";
    while ((opt = getopt(argc, argv, ":m:a:ik:l:h")) != -1)
    {
        switch(opt)
        {
            case 'm':
                namefilter = optarg;
                break;
            case 'a':
                Ksize = atof(optarg);
                break;
            case 'i':
                finv = true;
                break;
            case 'k':
                lambda = atof(optarg);
                break;
            case 'l':
                lnum = atof(optarg);
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

    ImthresholdFilterDespeckleTitle();

    if(optind + 2 > argc || fhelp || Ksize < 1)
    {
        ImthresholdFilterDespeckleUsage();;
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
            FIBITMAP *despeckled;
            if (FreeImage_GetBPP(dib) == 1)
            {
                unsigned width = FreeImage_GetWidth(dib);
                unsigned height = FreeImage_GetHeight(dib);
                unsigned y;

                BYTE** p_im;
                p_im = (BYTE**)malloc(height * sizeof(BYTE*));
                for (y = 0; y < height; y++) {p_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

                printf("Aperture= %d\n", Ksize);

                ImthresholdGetDataBW(dib, p_im);
                if (finv) {IMTFilterInvertBW(p_im, height, width);}
                if (strcmp(namefilter, "hist") == 0) {
                    printf("Method= %s\n", namefilter);
                    double kdp = 0.0;
                    kdp = IMTFilterDphist(p_im, height, width, Ksize);
                    printf("Despeckle= %f\n", kdp);
                } else if (strcmp(namefilter, "invert") == 0) {
                    printf("Method= %s\n", namefilter);
                    IMTFilterInvertBW(p_im, height, width);
                } else if (strcmp(namefilter, "mag2") == 0) {
                    printf("Method= %s\n", namefilter);
                    unsigned threshold = 0;
                    threshold = IMTFilterDMag2(p_im, height, width, Ksize);
                    printf("Despeckle= %d\n", threshold);
                } else if (strcmp(namefilter, "neuro") == 0) {
                    printf("Method= %s\n", namefilter);
                    printf("Lambda= %f\n", lambda);
                    printf("Learnen= %d\n", lnum);
                    IMTFilterDNeuro2(p_im, height, width, Ksize, lambda, lnum);
                } else {
                    printf("Method= simple\n");
                    IMTFilterDespeck2(p_im, height, width, Ksize);
                }
                despeckled = FreeImage_Allocate(width, height, 1);
                ImthresholdSetDataBW(despeckled, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else {
                despeckled = ImthresholdFilterNone(dib);
                printf("%s\n", "Unsupported color mode.");
            }

            if (despeckled)
            {
                FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
                if(out_fif != FIF_UNKNOWN)
                {
                    FreeImage_Save(out_fif, despeckled, output_filename, 0);
                    printf("Output= %s\n\n", output_filename);
                }
                FreeImage_Unload(despeckled);
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
