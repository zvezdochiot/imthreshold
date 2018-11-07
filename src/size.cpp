//    Zlib license
//
// Resize image.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSizeTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Resize image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterSizeUsage()
{
    printf("Usage : imthreshold-size [options] <input_image> <output_image>\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'bicubic'\n");
    printf("                    'biline'\n");
    printf("                    'bicont'\n");
    printf("                    'gsample' (default)\n");
    printf("                    'nearest'\n");
    printf("          -r N.N  ratio (double, optional, default = 1.0)\n");
    printf("          -w N    new width (int, optional, default = [auto])\n");
    printf("          -z N    new height (int, optional, default = [auto])\n");
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
    double ratio = 1.0;
    int newh = 0;
    int neww = 0;
    bool fhelp = false;
    char *namefilter;
    namefilter="gsample";
    while ((opt = getopt(argc, argv, ":f:r:w:z:h")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case 'r':
                ratio = atof(optarg);
                break;
            case 'w':
                neww = atof(optarg);
                break;
            case 'z':
                newh = atof(optarg);
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

    ImthresholdFilterSizeTitle();

    if(optind + 2 > argc || fhelp || (ratio <= 0 && (newh <= 0 &&  neww <= 0)))
    {
        ImthresholdFilterSizeUsage();
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
            unsigned new_width, new_height;
            unsigned y;
            if (newh <= 0 && neww <= 0)
            {
                new_width = width * ratio;
                new_height = height * ratio;
            } else if (newh > 0 && neww > 0) {
                new_width = neww;
                new_height = newh;
            } else if (newh > 0) {
                new_width = (width * newh) / height;
                new_height = newh;
            } else {
                new_width = neww;
                new_height = (height * neww) / width;
            }
            IMTpixel** p_im;
            p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
            for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
            IMTpixel** d_im;
            d_im = (IMTpixel**)malloc(new_height * sizeof(IMTpixel*));
            for (y = 0; y < new_height; y++) {d_im[y] = (IMTpixel*)malloc(new_width * sizeof(IMTpixel));}

            printf("Width= %d\n", new_width);
            printf("Height= %d\n", new_height);

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (strcmp(namefilter, "bicubic") == 0)
            {
                printf("Filter= %s\n", namefilter);
                IMTFilterSBicub(p_im, d_im, height, width, new_height, new_width);
            } else if (strcmp(namefilter, "biline") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTFilterSBilin(p_im, d_im, height, width, new_height, new_width);
            } else if (strcmp(namefilter, "bicont") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTFilterSBicont(p_im, d_im, height, width, new_height, new_width);
            } else if (strcmp(namefilter, "nearest") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTFilterSNearest(p_im, d_im, height, width, new_height, new_width);
            } else {
                printf("Filter= gsample\n");
                IMTFilterSGsample(p_im, d_im, height, width, new_height, new_width);
            }
            for (y = 0; y < height; y++){free(p_im[y]);}
            free(p_im);
            dst_dib = FreeImage_Allocate(new_width, new_height, 24);
            ImthresholdSetData(dst_dib, d_im);
            for (y = 0; y < new_height; y++){free(d_im[y]);}
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
