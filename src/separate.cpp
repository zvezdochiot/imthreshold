//    Zlib license
//
// BGFG separate based Multi-scale separate.
//
//    Copyright (C) 2017:
//    zvezdochiot    <zvezdochiot@user.sourceforge.net>

#include <unistd.h>
#include <FreeImage.h>
#include "imthresholdfreeimage.h"

////////////////////////////////////////////////////////////////////////////////

void ImthresholdFilterSeparateTitle()
{
    printf("ImThreshold.\n");
    printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
    printf("Separate image.\n");
    printf("Homepage: https://sourceforge.net/projects/imthreshold/.\n\n");
}

void ImthresholdFilterSeparateUsage()
{
    printf("Usage : imthreshold-separate [options] <input_file> <input_mask_file>(BW) <output_fg_file> <output_bg_file>\n\n");
    printf("options:\n");
    printf("          -m str  name metod:\n");
    printf("                    'delta'\n");
    printf("                    'inpaint'\n");
    printf("                    'mscale'\n");
    printf("                    'simple (default)'\n");
    printf("          -c N    clean blur radius (int, optional, default = 0)\n");
    printf("          -b N    base block size (int, optional, default = 3)\n");
    printf("          -f N    foreground divide (int, optional, default = 2)\n");
    printf("          -k N.N  coefficient delta threshold (double, optional, default = 2.0)\n");
    printf("          -l N    level (int, optional, default = 0[auto])\n");
    printf("          -r      rewrite mask (bool, optional, default = false)\n");
    printf("          -o N.N  overlay (double, optional, default = 0.5)\n");
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
    double doverlay = 0.5;
    double kdelta = 2.0;
    int bgs = 3;
    int fgs = 2;
    int level = 0;
    int fclean = 0;
    bool frewrite = false;
    bool fhelp = false;
    char *namefilter;
    namefilter="simple";
    while ((opt = getopt(argc, argv, ":b:c:f:k:l:m:o:rh")) != -1)
    {
        switch(opt)
        {
            case 'm':
                namefilter = optarg;
                break;
            case 'b':
                bgs = atof(optarg);
                break;
            case 'f':
                fgs = atof(optarg);
                break;
            case 'k':
                kdelta = atof(optarg);
                break;
            case 'l':
                level = atof(optarg);
                break;
            case 'o':
                doverlay = atof(optarg);
                break;
            case 'c':
                fclean = atof(optarg);
                break;
            case 'r':
                frewrite = true;
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

    ImthresholdFilterSeparateTitle ();

    if(optind + 4 > argc || fhelp)
    {
        ImthresholdFilterSeparateUsage ();
        return 0;
    }
    const char *src_filename = argv[optind];
    const char *mask_filename = argv[optind + 1];
    const char *fg_filename = argv[optind + 2];
    const char *bg_filename = argv[optind + 3];

    if (bgs < 1) {bgs = 1;}
    if (fgs < 1) {fgs = 1;}
    if (level < 0) {level = 0;}
    FreeImage_SetOutputMessage (FreeImageErrorHandler);

    printf ("Input= %s\n", src_filename);
    FIBITMAP *dib = ImthresholdGenericLoader (src_filename, 0);
    if (dib)
    {
        if (FreeImage_GetImageType(dib) == FIT_BITMAP)
        {
            unsigned width = FreeImage_GetWidth (dib);
            unsigned height = FreeImage_GetHeight (dib);
            unsigned y;

            IMTpixel** p_im;
            p_im = (IMTpixel**)malloc (height * sizeof(IMTpixel*));
            for (y = 0; y < height; y++) {p_im[y] = (IMTpixel*)malloc (width * sizeof(IMTpixel));}

            ImthresholdGetData(dib, p_im);
            FreeImage_Unload(dib);

            printf("Input= %s\n", mask_filename);
            FIBITMAP *dib = ImthresholdGenericLoader(mask_filename, 0);
            if (dib)
            {
                if (FreeImage_GetImageType(dib) == FIT_BITMAP)
                {
                    if (FreeImage_GetBPP(dib) == 1)
                    {
                        unsigned widthm = FreeImage_GetWidth (dib);
                        unsigned heightm = FreeImage_GetHeight (dib);

                        if ((height == heightm) && (width == widthm))
                        {
                            FIBITMAP* fg_dib;
                            FIBITMAP* bg_dib;

                            BYTE** m_im;
                            m_im = (BYTE**)malloc(height * sizeof(BYTE*));
                            for (y = 0; y < height; y++) {m_im[y] = (BYTE*)malloc(width * sizeof(BYTE));}

                            ImthresholdGetDataBW(dib, m_im);
                            if (strcmp(namefilter, "delta") == 0)
                            {
                                printf("Filter= %s\n", namefilter);

                                IMTpixel** g_im;
                                g_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                                for (y = 0; y < height; y++) {g_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                                printf("Kdelta= %f\n", kdelta);

                                IMTFilterSeparateDelta (p_im, m_im, g_im, height, width, 1, kdelta);
                                fg_dib = FreeImage_Allocate (width, height, 24);
                                ImthresholdSetData (fg_dib, g_im);
                                IMTFilterSeparateDelta (p_im, m_im, g_im, height, width, -1, kdelta);
                                bg_dib = FreeImage_Allocate (width, height, 24);
                                ImthresholdSetData (bg_dib, g_im);
                                for (y = 0; y < height; y++){free(g_im[y]);}
                                free(g_im);
                            }
                            else if (strcmp(namefilter, "inpaint") == 0)
                            {
                                printf("Filter= %s\n", namefilter);

                                IMTpixel** g_im;
                                g_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                                for (y = 0; y < height; y++) {g_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                                IMTFilterInpaint (p_im, m_im, g_im, height, width, 0);
                                fg_dib = FreeImage_Allocate (width, height, 24);
                                ImthresholdSetData (fg_dib, g_im);
                                IMTFilterInpaint (p_im, m_im, g_im, height, width, 255);
                                bg_dib = FreeImage_Allocate(width, height, 24);
                                ImthresholdSetData (bg_dib, g_im);
                                for (y = 0; y < height; y++){free(g_im[y]);}
                                free(g_im);
                            }
                            else if (strcmp(namefilter, "mscale") == 0)
                            {
                                printf("Filter= %s\n", namefilter);

                                unsigned widthbg = (width + bgs - 1) / bgs;
                                unsigned heightbg = (height + bgs - 1) / bgs;
                                unsigned widthfg = (widthbg + fgs - 1) / fgs;
                                unsigned heightfg = (heightbg + fgs - 1) / fgs;

                                printf("Level= %d\n", level);
                                printf("Overlay= %f\n", doverlay);

                                IMTpixel** fg_im;
                                fg_im = (IMTpixel**)malloc(heightfg * sizeof(IMTpixel*));
                                for (y = 0; y < heightfg; y++) {fg_im[y] = (IMTpixel*)malloc(widthfg * sizeof(IMTpixel));}
                                IMTpixel** bg_im;
                                bg_im = (IMTpixel**)malloc(heightbg * sizeof(IMTpixel*));
                                for (y = 0; y < heightbg; y++) {bg_im[y] = (IMTpixel*)malloc(widthbg * sizeof(IMTpixel));}
                                BYTE** fgm_im;
                                fgm_im = (BYTE**)malloc(heightfg * sizeof(BYTE*));
                                for (y = 0; y < heightfg; y++) {fgm_im[y] = (BYTE*)malloc(widthfg * sizeof(BYTE));}
                                BYTE** bgm_im;
                                bgm_im = (BYTE**)malloc(heightbg * sizeof(BYTE*));
                                for (y = 0; y < heightbg; y++) {bgm_im[y] = (BYTE*)malloc(widthbg * sizeof(BYTE));}

                                IMTFilterSeparateBGFGL (p_im, m_im, fg_im, bg_im, height, width, bgs, fgs, level, doverlay);
                                IMTReduceBW (m_im, fgm_im, height, width, heightfg, widthfg, bgs * fgs, 0, 255);
                                IMTReduceBW (m_im, bgm_im, height, width, heightbg, widthbg, bgs, 255, 0);

                                if (fclean > 0)
                                {
                                    printf("Blur= %d\n", fclean);
                                    IMTBlurMask(fg_im, fgm_im, heightfg, widthfg, fclean);
                                    IMTBlurMask(bg_im, bgm_im, heightbg, widthbg, fclean);
                                }
                                for (y = 0; y < heightfg; y++){free(fgm_im[y]);}
                                free(fgm_im);
                                for (y = 0; y < heightbg; y++){free(bgm_im[y]);}
                                free(bgm_im);
                                fg_dib = FreeImage_Allocate (widthfg, heightfg, 24);
                                ImthresholdSetData (fg_dib, fg_im);
                                for (y = 0; y < heightfg; y++){free(fg_im[y]);}
                                free(fg_im);
                                bg_dib = FreeImage_Allocate (widthbg, heightbg, 24);
                                ImthresholdSetData (bg_dib, bg_im);
                                for (y = 0; y < heightbg; y++){free(bg_im[y]);}
                                free(bg_im);
                            } else {
                                printf("Filter= simple\n");

                                IMTpixel** g_im;
                                g_im = (IMTpixel**)malloc(height * sizeof(IMTpixel*));
                                for (y = 0; y < height; y++) {g_im[y] = (IMTpixel*)malloc(width * sizeof(IMTpixel));}

                                IMTFilterSeparate (p_im, m_im, g_im, height, width, 0);
                                fg_dib = FreeImage_Allocate (width, height, 24);
                                ImthresholdSetData (fg_dib, g_im);
                                IMTFilterSeparate (p_im, m_im, g_im, height, width, 255);
                                bg_dib = FreeImage_Allocate (width, height, 24);
                                ImthresholdSetData (bg_dib, g_im);
                                for (y = 0; y < height; y++){free(g_im[y]);}
                                free(g_im);
                            }
                            if (frewrite)
                            {
                                ImthresholdSetDataBW (dib, m_im);
                                FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename (mask_filename);
                                FreeImage_Save (out_fif, dib, mask_filename, 0);
                                printf("Output= %s\n", mask_filename);                              
                            }
                            FreeImage_Unload(dib);
                            for (y = 0; y < height; y++){free(m_im[y]);}
                            free(m_im);

                            if (fg_dib)
                            {
                                FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename (fg_filename);
                                if(out_fif != FIF_UNKNOWN)
                                {
                                    FreeImage_Save (out_fif, fg_dib, fg_filename, 0);
                                    printf("Output= %s\n", fg_filename);
                                }
                                FreeImage_Unload(fg_dib);
                            }
                            if (bg_dib)
                            {
                                FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename (bg_filename);
                                if(out_fif != FIF_UNKNOWN)
                                {
                                    FreeImage_Save (out_fif, bg_dib, bg_filename, 0);
                                    printf("Output= %s\n", bg_filename);
                                }
                                FreeImage_Unload (bg_dib);
                            }
                        } else {
                            printf("%s\n", "Size mask uncorect.");
                            FreeImage_Unload (dib);
                        }
                    } else {
                        printf("%s\n", "Unsupported color mode.");
                        FreeImage_Unload (dib);
                    }
                } else {
                    printf("%s\n", "Unsupported color mode.");
                    FreeImage_Unload (dib);
                }
            }
            for (y = 0; y < height; y++){free(p_im[y]);}
            free(p_im);

            printf("\n");
        } else {
            printf("%s\n", "Unsupported format type.");
            FreeImage_Unload (dib);
        }
    }

    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise ();
#endif // FREEIMAGE_LIB

    return 0;
}
