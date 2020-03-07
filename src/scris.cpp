//    Zlib license
//
// Resize image using bicubic Reverse Interpolate Scale (CRIS)
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSCRISTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Resize image using bicubic Reverse Interpolate Scale (CRIS).\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterSCRISUsage()
{
    printf("Usage : imthreshold-scris [options] <input_file> <output_file>\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'bicubic'\n");
    printf("                    'bicont' (default)\n");
    printf("          -p N.N  part prefilter (float, optional, default = 0.5)\n");
    printf("          -r N.N  ratio (float, optional, default = 1.0)\n");
    printf("          -w N    new width (int, optional, default = [auto])\n");
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
    float ratio = 1.0;
    int newh = 0;
    int neww = 0;
    float ims = 0.0;
    float ppart = 0.5;
    bool fhelp = false;
    char *namefilter = "bicont";
    while ((opt = getopt(argc, argv, ":f:p:r:w:z:h")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case 'p':
                ppart = atof(optarg);
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

    ImthresholdFilterSCRISTitle();

    if(optind + 2 > argc || fhelp || (ratio <= 0 && (newh <= 0 &&  neww <= 0)))
    {
        ImthresholdFilterSCRISUsage();
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

            IMTpixel** p_im = IMTalloc(height, width);
            IMTpixel** c_im = IMTalloc(height, width);
            IMTpixel** d_im = IMTalloc(new_height, new_width);

            printf("Width= %d\n", new_width);
            printf("Height= %d\n", new_height);

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (strcmp(namefilter, "bicubic") == 0)
            {
                printf("Filter= %s\n", namefilter);
                IMTFilterSBicub(p_im, d_im, height, width, new_height, new_width);
                IMTFilterSBicub(d_im, c_im, new_height, new_width, height, width);
                printf("Prefilter= %f\n", ppart);
                ims = IMTFilterMirrorPart(p_im, c_im, height, width, ppart);
                printf("RIS= %f\n", ims);
                IMTFilterSBicub(c_im, d_im, height, width, new_height, new_width);
            } else {
                printf("Filter= bicont\n");
                IMTFilterSBicont(p_im, d_im, height, width, new_height, new_width);
                IMTFilterSBicont(d_im, c_im, new_height, new_width, height, width);
                printf("Prefilter= %f\n", ppart);
                ims = IMTFilterMirrorPart(p_im, c_im, height, width, ppart);
                printf("RIS= %f\n", ims);
                IMTFilterSBicont(c_im, d_im, height, width, new_height, new_width);
            }
            IMTfree(p_im, height);
            IMTfree(c_im, height);
            dst_dib = FreeImage_Allocate(new_width, new_height, 24);
            ImthresholdSetData(dst_dib, d_im);
            IMTfree(d_im, new_height);

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
