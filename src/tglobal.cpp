//    Zlib license
//
// Global thresholding image.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTGlobalTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Global thresholding image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterTGlobalUsage()
{
    printf("Usage : imthreshold-tglobal [options] <input_image> <output_image>(BW)\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'bht'\n");
    printf("                    'bimod' (default)\n");
    printf("                    'dither'\n");
    printf("                    'entropy'\n");
    printf("                    'eqbright'\n");
    printf("                    'grad'\n");
    printf("                    'janni'\n");
    printf("                    'kmeans'\n");
    printf("                    'otsu'\n");
    printf("                    'rot'\n");
    printf("                    'tsai'\n");
    printf("                    'use'\n");
    printf("          -d N    delta (int, optional, default = 0)\n");
    printf("          -k N    K par (k/2) (int, optional, default = 1)\n");
    printf("          -m N    max iteration (int, optional, default = 10)\n");
    printf("          -s N    shift (int, optional, default = -32)\n");
    printf("          -w      weight colors (bool, optional)\n");
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
    int delta = 0;
    int knum = 2;
    int iters = 10;
    int shift = -32;
    bool weight = false;
    bool fhelp = false;
    int threshold;
    char *namefilter;
    namefilter="bimod";
    while ((opt = getopt(argc, argv, ":f:d:k:m:s:wh")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case 'd':
                delta = atof(optarg);
                break;
            case 'k':
                knum = atof(optarg);
                break;
            case 'm':
                iters = atof(optarg);
                break;
            case 's':
                shift = atof(optarg);
                break;
            case 'w':
                weight = true;
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

    ImthresholdFilterTGlobalTitle();

    if(optind + 2 > argc || fhelp)
    {
        ImthresholdFilterTGlobalUsage();
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

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (strcmp(namefilter, "bht") == 0)
            {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTBHT(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "dither") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTDither(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "entropy") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTEnt(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "eqbright") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTEqBright(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "grad") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTGrad(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "janni") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTJanni(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "kmeans") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTKMeans(p_im, d_im, height, width, knum, iters);
            } else if (strcmp(namefilter, "otsu") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTOtsu(p_im, d_im, height, width);
            } else if (strcmp(namefilter, "rot") == 0) {
                printf("Filter= %s\n", namefilter);
                threshold = IMTFilterTRot(p_im, d_im, height, width, weight);
            } else if (strcmp(namefilter, "tsai") == 0) {
                threshold = IMTFilterTTsai(p_im, d_im, height, width, shift);
                printf("Filter= %s\n", namefilter);
            } else if (strcmp(namefilter, "use") == 0) {
                printf("Filter= %s\n", namefilter);
                printf("Delta= %d\n", delta);
                threshold = IMTFilterThreshold(p_im, d_im, height, width, delta);
            } else {
                printf("Filter= bimod\n");
                printf("Delta= %d\n", delta);
                threshold = IMTFilterTBiMod(p_im, d_im, height, width, delta);
            }
            printf("Threshold= %d\n", (threshold + 1) / 3);
            for (y = 0; y < height; y++){free(p_im[y]);}
            free(p_im);
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
