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
    printf("                    'bimod'\n");
    printf("                    'blur'\n");
    printf("                    'bwc'\n");
    printf("                    'cluster'\n");
    printf("                    'denoise'\n");
    printf("                    'greynorm'\n");
    printf("                    'greyworld'\n");
    printf("                    'illumc'\n");
    printf("                    'invert'\n");
    printf("                    'kmeans'\n");
    printf("                    'levelmean'\n");
    printf("                    'levelsigma'\n");
    printf("                    'levelsize'\n");
    printf("                    'mirror'\n");
    printf("                    'monocolor'\n");
    printf("                    'mclose'\n");
    printf("                    'mdilate'\n");
    printf("                    'merose'\n");
    printf("                    'mopen'\n");
    printf("                    'none' (default)\n");
    printf("                    'peron'\n");
    printf("                    'posterize'\n");
    printf("                    'quant'\n");
    printf("                    'retinex'\n");
    printf("                    'rs'\n");
    printf("                    'selgauss'\n");
    printf("                    'shrink'\n");
    printf("                    'unripple'\n");
    printf("                    'unsharp'\n");
    printf("                    'whitefill'\n");
    printf("                    'wiener'\n");
    printf("          -4      RGB to RYB4 (bool, optional, default = false)\n");
    printf("          -a N.N  amount (float, optional, default = 0.5)\n");
    printf("          -c N.N  contour factor (float, optional, default = -1[auto])\n");
    printf("          -d N    max delta (int, optional, default = 50)\n");
    printf("          -i N    num divide (int, optional, default = 1)\n");
    printf("          -k N    K num (int, optional, default = 2)\n");
    printf("          -l N    lower bound (int, optional, default = 32)\n");
    printf("          -n N.N  noise size (float, optional, default = -1.0[auto])\n");
    printf("          -p N    posterize divide factor (int, optional, default = 16)\n");
    printf("          -r N.N  radius (float, optional, default = 3.0)\n");
    printf("          -s N.N  sigma (float, optional, default = 4.0)\n");
    printf("          -t N.N  threshold (float, optional, default = 0.0)\n");
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
    unsigned width, height;
    float amount = 0.5;
    float contour = -1.0;
    int maxdelta = 50;
    unsigned ndiv = 1;
    int knum = 2;
    int lower_bound = 32;
    float noise = -1.0;
    int posterdiv = 16;
    float radius = 5.0;
    float sigma = 4.0;
    float threshold = 0.0;
    int upper_bound = 223;
    bool fryb4 = false;
    bool fhelp = false;
    char *namefilter;
    namefilter="none";
    while ((opt = getopt(argc, argv, ":f:4a:c:d:i:k:l:n:p:r:s:t:u:h")) != -1)
    {
        switch(opt)
        {
            case 'f':
                namefilter = optarg;
                break;
            case '4':
                fryb4 = true;
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
            width = FreeImage_GetWidth(dib);
            height = FreeImage_GetHeight(dib);
            IMTpixel** p_im = IMTalloc(height, width);
            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);
            if (fryb4)
            {
                printf("ColorSpace= RYB4\n");
                IMTFilterRGBtoRYB4(p_im, height, width, 1);
            }

            if (strcmp(namefilter, "adsmooth") == 0)
            {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);

                IMTFilterAdSmooth(p_im, d_im, height, width, radiusint);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "bimod") == 0) {
                printf("Filter= %s\n", namefilter);
                int thres = 0;
                IMTpixel** d_im = IMTalloc(height, width);

                posterdiv = (posterdiv < 2) ? 2 : posterdiv;
                printf("Count= %d\n", posterdiv);

                thres = IMTFilterTBiModP(p_im, d_im, height, width, posterdiv);
                thres /= 3;
                printf("Threshold= %d\n", thres);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
             } else if (strcmp(namefilter, "bwc") == 0) {
                printf("Filter= %s\n", namefilter);
                unsigned radiusint;
                float kwb = 0.5;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = unsigned(radius + 0.5);
                printf("Radius= %d\n", radiusint);

                kwb = IMTFilterClusterBWC(p_im, d_im, height, width, radiusint);
                printf("W/b= %f\n", kwb);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "blur") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);

                IMTFilterGaussBlur(p_im, d_im, height, width, radius);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
           } else if (strcmp(namefilter, "cluster") == 0) {
                printf("Filter= %s\n", namefilter);
                int imdm;
                IMTpixel** d_im = IMTalloc(height, width);

                if (knum < 2) {knum = 2;}
                printf("K= %d\n", knum);

                imdm = IMTFilterClusterBiModC (p_im, d_im, height, width, knum);
                printf("Dmax= %d\n", imdm);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "denoise") == 0) {
                printf("Filter= %s\n", namefilter);
                float kdenoises;
                unsigned radiusint;

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                printf("Radius= %d\n", radiusint);
                printf("KUse= %f\n", threshold);

                kdenoises = IMTFilterDeNoiseDiff (p_im, height, width, radiusint, threshold);
                printf("KDeNoise= %f\n", kdenoises);
            } else if (strcmp(namefilter, "greyworld") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel dim;

                dim = IMTFilterGreyWorld(p_im, height, width);
                printf("Normalize= %d,%d,%d\n", dim.c[0], dim.c[1], dim.c[2]);
            } else if (strcmp(namefilter, "greynorm") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel dim;

                dim = IMTFilterGreyNorm(p_im, height, width);
                printf("Normalize= %d,%d,%d: %d\n", dim.c[0], dim.c[1], dim.c[2], dim.s);
            } else if (strcmp(namefilter, "illumc") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel domin;
                IMTpixel** b_im = IMTalloc(height, width);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);
                printf("Threshold= %f\n", threshold);

                IMTFilterGaussBlur(p_im, b_im, height, width, radius);
                domin = IMTFilterIllumCorr(p_im, b_im, d_im, height, width);
                printf("Dominante= %d,%d,%d\n", domin.c[0], domin.c[1], domin.c[2]);
                IMTfree(b_im, height);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "invert") == 0) {
                printf("Filter= %s\n", namefilter);

                IMTFilterInvert(p_im, height, width);
            } else if (strcmp(namefilter, "kmeans") == 0) {
                printf("Filter= %s\n", namefilter);

                if (knum < 2) {knum = 2;}
                printf("K= %d\n", knum);

                threshold = IMTFilterKMeans(p_im, height, width, knum, threshold);
                printf("Iters= %d\n", (int)threshold);
            } else if (strcmp(namefilter, "levelmean") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("Threshold= %f\n", threshold);
                printf("Lower= %d\n", lower_bound);
                printf("Upper= %d\n", upper_bound);

                contour = IMTFilterLevelMean(p_im, d_im, height, width, radiusint, contour, threshold, lower_bound, upper_bound);
                printf("Contour= %f\n", contour);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "levelsigma") == 0) {
                printf("Filter= %s\n", namefilter);
                float ks = 0;
                IMTpixel** d_im = IMTalloc(height, width);

                printf("Average= %f\n", amount);
                printf("Part= %f\n", threshold);

                ks = IMTFilterLevelSigma(p_im, d_im, height, width, amount, threshold);
                printf("Multy= %f\n", ks);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "levelsize") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                float ks = 0;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("Delta= %f\n", threshold);

                ks = IMTFilterLevelSize(p_im, d_im, height, width, radiusint, threshold);
                printf("Distance= %f\n", ks);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "mirror") == 0) {
                printf("Filter= %s\n", namefilter);

                IMTFilterMirrorMean(p_im, height, width);
                dst_dib = FreeImage_Allocate(width, height, 24);
                ImthresholdSetData(dst_dib, p_im);
            } else if (strcmp(namefilter, "monocolor") == 0) {
                printf("Filter= %s\n", namefilter);

                IMTFilterMonoColor(p_im, height, width);
            } else if (strcmp(namefilter, "mclose") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                printf("Radius= %f\n", radius);

                IMTFilterMorph(p_im, d_im, height, width, radius, true);
                IMTFilterMorph(d_im, p_im, height, width, radius, false);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "mdilate") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                printf("Radius= %f\n", radius);

                IMTFilterMorph(p_im, d_im, height, width, radius, true);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "merose") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                printf("Radius= %f\n", radius);

                IMTFilterMorph(p_im, d_im, height, width, radius, false);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "mopen") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                printf("Radius= %f\n", radius);

                IMTFilterMorph(p_im, d_im, height, width, radius, false);
                IMTFilterMorph(d_im, p_im, height, width, radius, true);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "peron") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                printf("Radius= %f\n", radius);
                if (radiusint == 0) {radiusint = 1;}
                if (noise <= 0)
                {
                    noise = IMTFilterNoiseVariance (p_im, height, width, radiusint);
                    noise /= 2;
                }
                printf("Noise= %f\n", noise);
                IMTFilterPeron(p_im, d_im, height, width, radius, noise);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "posterize") == 0) {
                printf("Filter= %s\n", namefilter);
                float imsh = 0;

                if (posterdiv < 1) {posterdiv = 1;}
                printf("Posterize= %d\n", posterdiv);
                imsh = IMTFilterPosterize(p_im, height, width, posterdiv);
                printf("Delta= %f\n", imsh);
            } else if (strcmp(namefilter, "quant") == 0) {
                printf("Filter= %s\n", namefilter);
                float imsh = 0;

                if (posterdiv < 1) {posterdiv = 1;}
                printf("Quant= %d\n", posterdiv);
                imsh = IMTFilterQuant(p_im, height, width, posterdiv);
                printf("Delta= %f\n", imsh);
            } else if (strcmp(namefilter, "retinex") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                int kimi = 0;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("Sigma= %f\n", sigma);
                kimi = IMTFilterRetinex(p_im, d_im, height, width, radiusint, sigma);
                printf("K= %d\n", kimi);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "rs") == 0) {
                printf("Filter= %s\n", namefilter);
                float imd = 0;

                imd = IMTFilterRS(p_im, height, width);
                printf("Stdev= %f\n", imd);
            } else if (strcmp(namefilter, "selgauss") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);
                printf("MaxDelta= %d\n", maxdelta);

                IMTFilterSelGauss(p_im, d_im, height, width, radius, maxdelta);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "shrink") == 0) {
                printf("Filter= %s\n", namefilter);
                float imsh = 0;

                if (threshold <= 0) {threshold = 4;}
                printf("Threshold= %f\n", threshold);

                imsh = IMTFilterShrink(p_im, height, width, threshold);
                printf("Shrink= %f\n", imsh);
            } else if (strcmp(namefilter, "unripple") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);
                printf("MaxDelta= %d\n", maxdelta);

                IMTFilterUnRipple(p_im, d_im, height, width, radiusint, maxdelta);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else if (strcmp(namefilter, "unsharp") == 0) {
                printf("Filter= %s\n", namefilter);
                IMTpixel** b_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                if (radius == 0) {radius = 1.0;}
                printf("Radius= %f\n", radius);
                printf("Amount= %f\n", amount);
                printf("Threshold= %f\n", threshold);

                IMTFilterGaussBlur(p_im, b_im, height, width, radius);
                IMTFilterUnsharpMask(p_im, b_im, height, width, amount, threshold);
                IMTfree(b_im, height);
            } else if (strcmp(namefilter, "whitefill") == 0) {
                printf("Filter= %s\n", namefilter);
                int niter;

                niter = IMTFilterWhiteFill(p_im, height, width);
                printf("Iteration= %d\n", niter);
            } else if (strcmp(namefilter, "wiener") == 0) {
                printf("Filter= %s\n", namefilter);
                int radiusint;
                IMTpixel** d_im = IMTalloc(height, width);

                if (radius < 0) {radius = -radius;}
                radiusint = int(radius + 0.5);
                if (radiusint == 0) {radiusint = 1;}
                printf("Radius= %d\n", radiusint);

                if (noise < 0)
                {
                    noise = IMTFilterNoiseVariance (p_im, height, width, radiusint);
                }
                printf("Noise= %f\n", noise);
                IMTFilterWiener(p_im, d_im, height, width, radiusint, noise);
                IMTFilterCopy (d_im, p_im, height, width);
                IMTfree(d_im, height);
            } else {
                printf("Filter= none\n");
            }
            if (fryb4)
            {
                printf("ColorSpace= RGB\n");
                IMTFilterRGBtoRYB4(p_im, height, width, -1);
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

