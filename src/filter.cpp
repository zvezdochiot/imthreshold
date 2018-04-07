//  Zlib license
//
// Filter image.
//
//  Copyright (C) 2017:
//  zvezdochiot <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Filter image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterUsage()
{
    printf("Usage : imthreshold-filter [options] <input_image> <output_image>\n\n");
    printf("options:\n");
    printf("          -f str  name filter:\n");
    printf("                    'adsmooth'\n");
    printf("                    'blur'\n");
    printf("                    'bwc'\n");
    printf("                    'cluster'\n");
    printf("                    'greynorm'\n");
    printf("                    'greyworld'\n");
    printf("                    'illumc'\n");
    printf("                    'invert'\n");
    printf("                    'kmeans'\n");
    printf("                    'levelmean'\n");
    printf("                    'levelsigma'\n");
    printf("                    'mirror'\n");
    printf("                    'monocolor'\n");
    printf("                    'none' (default)\n");
    printf("                    'peron'\n");
    printf("                    'posterize'\n");
    printf("                    'retinex'\n");
    printf("                    'rs'\n");
    printf("                    'selgauss'\n");
    printf("                    'shrink'\n");
    printf("                    'unripple'\n");
    printf("                    'unsharp'\n");
    printf("                    'whitefill'\n");
    printf("                    'wiener'\n");
    printf("          -a N.N  amount (double, optional, default = 0.5)\n");
    printf("          -c N.N  contour factor (double, optional, default = -1[auto])\n");
    printf("          -d N    max delta (int, optional, default = 50)\n");
    printf("          -i N    num divide (int, optional, default = 1)\n");
    printf("          -k N    K num (int, optional, default = 2)\n");
    printf("          -l N    lower bound (int, optional, default = 32)\n");
    printf("          -n N.N  noise size (double, optional, default = -1.0[auto])\n");
    printf("          -p N    posterize divide factor (int, optional, default = 16)\n");
    printf("          -r N.N  radius (double, optional, default = 3.0)\n");
    printf("          -s N.N  sigma (double, optional, default = 4.0)\n");
    printf("          -t N.N  threshold (double, optional, default = 0.0)\n");
    printf("          -u N    upper bound (int, optional, default = 223)\n");
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
    double amount = 0.5;
    double contour = -1.0;
    int maxdelta = 50;
    unsigned ndiv = 1;
    int knum = 2;
    int lower_bound = 32;
    double noise = -1.0;
    int posterdiv = 16;
    double radius = 5.0;
    double sigma = 4.0;
    double threshold = 0.0;
    int upper_bound = 223;
    bool fhelp = false;
    char *namefilter;
    namefilter="none";
    while ((opt = getopt(argc, argv, ":f:a:c:d:i:k:l:n:p:r:s:t:u:h")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case 'a':
                amount = atof(optarg);
                break;
            case 'c':
                contour = atof(optarg);
                break;
            case 'd':
                maxdelta = atof(optarg);
                break;
            case 'i':
                ndiv = atof(optarg);
                break;
            case 'k':
                knum = atof(optarg);
                break;
            case 'l':
                lower_bound = atof(optarg);
                break;
            case 'n':
                noise = atof(optarg);
                break;
            case 'p':
                posterdiv = atof(optarg);
                break;
            case 'r':
                radius = atof(optarg);
                break;
            case 's':
                sigma = atof(optarg);
                break;
            case 't':
                threshold = atof(optarg);
                break;
            case 'u':
                upper_bound = atof(optarg);
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

    ImthresholdFilterTitle();

    if (optind + 2 > argc || fhelp)
    {
        ImthresholdFilterUsage();
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

            if (strcmp(namefilter, "adsmooth") == 0)
            {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterAdSmooth(p_im, d_im, height, width, radiusint);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "bwc") == 0) {
                printf("Filter= %s\n", namefilter);
                unsigned radiusint;
                double kwb = 0.5;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = unsigned(radius + 0.5);
                printf("Radius= %d\n", radiusint);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                kwb = IMTFilterClusterBWC(p_im, d_im, height, width, radiusint);
                printf("W/b= %f\n", kwb);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "blur") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** b_im;
                b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterGaussBlur(p_im, b_im, height, width, radius);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, b_im);
                for (y = 0; y < height; y++){free(b_im[y]);}
                free(b_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
           } else if (strcmp(namefilter, "cluster") == 0) {
                printf("Filter= %s\n", namefilter);
                int imdm;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (knum < 2) {knum = 2;}
                printf("K= %d\n", knum);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                imdm = IMTFilterClusterBiModC (p_im, d_im, height, width, knum);
                printf("Dmax= %d\n", imdm);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "greyworld") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel dim;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                dim = IMTFilterGreyWorld(p_im, height, width);
                printf("Normalize= %d,%d,%d\n", dim.c[0], dim.c[1], dim.c[2]);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "greynorm") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel dim;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                dim = IMTFilterGreyNorm(p_im, height, width);
                printf("Normalize= %d,%d,%d: %d\n", dim.c[0], dim.c[1], dim.c[2], dim.s);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "illumc") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel domin;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** b_im;
                b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);
                printf("Threshold= %f\n", threshold);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterGaussBlur(p_im, b_im, height, width, radius);
                domin = IMTFilterIllumCorr(p_im, b_im, d_im, height, width);
                printf("Dominante= %d,%d,%d\n", domin.c[0], domin.c[1], domin.c[2]);
                for (y = 0; y < height; y++){free(b_im[y]);}
                free(b_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "invert") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterInvert(p_im, height, width);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "kmeans") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (knum < 2) {knum = 2;}
                printf("K= %d\n", knum);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                threshold = IMTFilterKMeans(p_im, height, width, knum, threshold);
                printf("Iters= %d\n", (int)threshold);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "levelmean") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("Threshold= %f\n", threshold);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                contour = IMTFilterLevelMean(p_im, d_im, height, width, radiusint, contour, threshold, lower_bound, upper_bound);
                printf("Contour= %f\n", contour);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "levelsigma") == 0) {
                printf("Filter= %s\n", namefilter);
                double ks = 0;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                printf("Average= %f\n", amount);
                printf("Part= %f\n", threshold);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                ks = IMTFilterLevelSigma(p_im, d_im, height, width, amount, threshold);
                printf("Multy= %f\n", ks);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "mirror") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterMirrorMean(p_im, height, width);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "monocolor") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterMonoColor(p_im, height, width);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "peron") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                printf("Radius= %f\n", radius);
                if (radiusint == 0) {radiusint = 1;}
                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                if (noise <= 0)
                {
                    noise = IMTFilterNoiseVariance (p_im, height, width, radiusint);
                    noise /= 2;
                }
                printf("Noise= %f\n", noise);
                IMTFilterPeron(p_im, d_im, height, width, radius, noise);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "posterize") == 0) {
                printf("Filter= %s\n", namefilter);
                double imsh = 0;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (posterdiv < 1) {posterdiv = 1;}
                printf("Posterize= %d\n", posterdiv);
                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                imsh = IMTFilterPosterize(p_im, height, width, posterdiv);
                printf("Delta= %f\n", imsh);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "retinex") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                int kimi = 0;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("Sigma= %f\n", sigma);
                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                kimi = IMTFilterRetinex(p_im, d_im, height, width, radiusint, sigma);
                printf("K= %d\n", kimi);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "rs") == 0) {
                printf("Filter= %s\n", namefilter);
                double imd = 0;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                imd = IMTFilterRS(p_im, height, width);
                printf("Stdev= %f\n", imd);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "selgauss") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);
                printf("MaxDelta= %d\n", maxdelta);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterSelGauss(p_im, d_im, height, width, radius, maxdelta);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "shrink") == 0) {
                printf("Filter= %s\n", namefilter);
                double imsh = 0;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (threshold <= 0) {threshold = 4;}
                printf("Threshold= %f\n", threshold);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                imsh = IMTFilterShrink(p_im, height, width, threshold);
                printf("Shrink= %f\n", imsh);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "unripple") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("MaxDelta= %d\n", maxdelta);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterUnRipple(p_im, d_im, height, width, radiusint, maxdelta);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else if (strcmp(namefilter, "unsharp") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** b_im;
                b_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {b_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);
                printf("Amount= %f\n", amount);
                printf("Threshold= %f\n", threshold);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                IMTFilterGaussBlur(p_im, b_im, height, width, radius);
                dst_dib = FreeImage_Allocate(width, height, 24);
                IMTFilterUnsharpMask(p_im, b_im, d_im, height, width, amount, threshold);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
                for (y = 0; y < height; y++){free(b_im[y]);}
                free(b_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "whitefill") == 0) {
                printf("Filter= %s\n", namefilter);
                int niter;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                niter = IMTFilterWhiteFill(p_im, height, width);
                printf("Iteration= %d\n", niter);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            } else if (strcmp(namefilter, "wiener") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}
                IMTpixel** d_im;
                d_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {d_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                if (noise < 0)
                {
                    noise = IMTFilterNoiseVariance (p_im, height, width, radiusint);
                }
                printf("Noise= %f\n", noise);
                IMTFilterWiener(p_im, d_im, height, width, radiusint, noise);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, d_im);
                for (y = 0; y < height; y++){free(d_im[y]);}
                free(d_im);
            } else {
                printf("Filter= none\n");
                IMTpixel** p_im;
                p_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                ImthresholdGetData(dib, p_im);
                FreeImage_Unload(dib);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
                for (y = 0; y < height; y++){free(p_im[y]);}
                free(p_im);
            }

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

