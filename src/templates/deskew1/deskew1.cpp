// ==========================================================
// The PageTools Deskew algorithm adapted for the FreeImage 3.92
//
// This deskew algorithm is based on the fast Radon transform algorithm
//
// PageTools Project: http://pagetools.sourceforge.net/
//
// ==========================================================
#include <math.h>
#include <memory.h>
#include <stdio.h>

#include <FreeImage.h>

extern FIBITMAP *DLL_CALLCONV BSL_RotateClassic(FIBITMAP *dib, double angle);

// ----------------------------------------------------------

/**
FreeImage error handler
@param fif Format / Plugin responsible for the error 
@param message Error message
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** "); 
	printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	printf(message);
	printf(" ***\n");
}

// ----------------------------------------------------------

/** Generic image loader

  @param lpszPathName Pointer to the full file name
  @param flag Optional load flag constant
  @return Returns the loaded dib if successful, returns NULL otherwise
*/

FIBITMAP* GenericLoader(const char* lpszPathName, int flag)
{	
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	
	fif = FreeImage_GetFileType(lpszPathName, 0);
	
	if(fif == FIF_UNKNOWN)
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
		// ok, let's load the file
		FIBITMAP *dib = FreeImage_Load(fif, lpszPathName, flag);
		
		// unless a bad file format, we are done !
		return dib;
	}
	
	return NULL;
}

// ----------------------------------------------------------

unsigned int PageTools_Next_Pow2(unsigned int n)
{
    unsigned int retval=1;
    while(retval<n){
        retval<<=1;
    }
    return retval;
}

// ----------------------------------------------------------

void PageTools_Radon(FIBITMAP *dib, int sign, unsigned int sharpness[])
{
    unsigned char* bitcount=new unsigned char[256];
	
	unsigned short int *p1, *p2; // Stored columnwise
    
	unsigned int w2=PageTools_Next_Pow2(FreeImage_GetPitch(dib));
	printf("w2 = %d\n", w2);

	// FreeImage_GetPitch - Returns the width of the bitmap in bytes, rounded
	// to the next 32-bit boundary, also known as pitch or stride or scan
	
	unsigned int w=FreeImage_GetPitch(dib);
	printf("w = %d\n", w);
	
	unsigned int h=FreeImage_GetHeight(dib);
	printf("h = %d\n", h);

	// FreeImage_GetHeight - Returns the height of the bitmap in pixel units.
	
    unsigned int s=h*w2;
	printf("s = %d\n", s);
    p1=new unsigned short int[s];
    p2=new unsigned short int[s];
    // Fill in the first table
    memset(p1, 0, sizeof(unsigned short int)*s);
    unsigned int ir, ic;

	unsigned int i, j, cnt;
	
    for(i=0; i<256; i++)
	{
		j=i, cnt=0;
        do cnt+=j&1; while(j>>=1);
        bitcount[i]=cnt;		
	}
	
    for(ir=0; ir<h; ir++)
	{        
		unsigned char const *scl= FreeImage_GetScanLine(dib, ir);
		
		// DLL_API BYTE *DLL_CALLCONV FreeImage_GetScanLine(FIBITMAP *dib, int);
		//
		// Returns a pointer to the start of the given scanline in the bitmap’s data-bits.
		// It is up to you to interpret these bytes correctly, according to the results of
		// FreeImage_GetBPP and FreeImage_GetImageType

		unsigned char val;
		
        for(ic=0; ic<w; ic++)
		{
            if(sign>0)
			{
				val = scl[w-1-ic];

                p1[h*ic+ir]=bitcount[val];
				
            }
			else
			{
				val = scl[ic];				

                p1[h*ic+ir]=bitcount[val];
            }
        }
    }    
	
    // Iterate
    unsigned short int *x1=p1;
    unsigned short int *x2=p2;
    unsigned int step=1;
    for(;;){
        for(i=0; i<w2; i+=2*step){
            for(j=0; j<step; j++){
                // Columns-sources:
                unsigned short int *s1=x1+h*(i+j);
                unsigned short int *s2=x1+h*(i+j+step);
                
                // Columns-targets:
                unsigned short int *t1=x2+h*(i+2*j);
                unsigned short int *t2=x2+h*(i+2*j+1);
                unsigned int m;
                for(m=0; m<h; m++){
                    t1[m]=s1[m];
                    t2[m]=s1[m];
                    if(m+j<h){
                        t1[m]+=s2[m+j];
                    }
                    if(m+j+1<h){
                        t2[m]+=s2[m+j+1];
                    }
                }
            }
        }
        // Swap the tables:
        unsigned short int *aux=x1;
        x1=x2;
        x2=aux;
        // Increase the step:
        step*=2;
        if(step>=w2) break;
    }
    // Now, compute the sum of squared finite differences:
    for(ic=0; ic<w2; ic++){
        unsigned int acc=0;
        unsigned short int *col=x1+h*ic;
        for(ir=0; ir+1<h; ir++){
            int diff=(int)(col[ir])-(int)(col[ir+1]);
            acc+=diff*diff;
        }
        sharpness[w2-1+sign*ic]=acc;
        
    }
    delete[] p1;
    delete[] p2;
    delete[] bitcount;    
}

// ----------------------------------------------------------

double PageTools_FindSkew(FIBITMAP *dib)
{
	unsigned int w2=PageTools_Next_Pow2(FreeImage_GetPitch(dib));
	printf("w2 = %d\n", w2);
	
	// FreeImage_GetPitch - Returns the width of the bitmap in bytes, rounded
	// to the next 32-bit boundary, also known as pitch or stride or scan
	
    unsigned int ssize=2*w2-1; // Size of sharpness table
	printf("ssize = %d\n", ssize);
    unsigned int *sharpness=new unsigned int[ssize];
    PageTools_Radon(dib, 1, sharpness);
    PageTools_Radon(dib, -1, sharpness);
    unsigned int i, imax=0;
    unsigned int vmax=0;
    double sum=0.;
    double fskew;
    for(i=0; i<ssize; i++)
	{
        unsigned int s=sharpness[i];

        if(s>vmax)
		{
            imax=i;
            vmax=s;
        }
        sum+=s;
    }
	printf("imax = %d\n", imax);
	printf("vmax = %d\n", vmax);
	printf("sum = %f\n", sum);
	
	unsigned int h=FreeImage_GetHeight(dib); 
	printf("h = %d\n", h);
	
	// FreeImage_GetHeight - Returns the height of the bitmap in pixel units.
	
    if(vmax<=3*sum/h){  // Heuristics !!!
		fskew = 0;
		printf("fskew = %f\n", fskew);
        return 0;
    }
    int iskew= (int)imax-int(w2)+1;
	printf("iskew = %d\n", iskew);
    delete[] sharpness;
    fskew = atan((double)iskew/(8*w2))*FreeImage_GetBPP(dib);
	printf("fskew = %f\n", fskew);
    return fskew;
}

// ----------------------------------------------------------

int main(int argc, char *argv[]) {
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif // FREEIMAGE_LIB
	
	// initialize your own FreeImage error handler
	printf("ImThreshold.\n");
	printf("BookScanLib Project: http://djvu-soft.narod.ru/\n\n");
	printf("The PageTools Deskew algorithm adapted for the FreeImage\n");
	printf("This deskew algorithm is based on the fast Radon transform algorithm\n");
	printf("PageTools Project: http://pagetools.sourceforge.net/\n\n");
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
	
   	if(argc != 3) {
		printf("Usage : imthreshold-deskew <input file name> <output file name>\n");
		return 1;
	}
	
	FIBITMAP *dib = GenericLoader(argv[1], 0);
	
	if (dib)
	{		
		// bitmap successfully loaded!
		
		double angle = PageTools_FindSkew(dib);
		
        // Convert radians to degrees and change the sign:
		
		angle = -57.295779513082320876798154814105*angle;
		
		printf("Skew angle = %f\n", angle);
		
		// Rotate the monochrome image by the found skew angle
		
		FIBITMAP *rotated;		

		rotated = BSL_RotateClassic(dib, -angle);
		
		// save the deskewed bitmap
		const char *output_filename = argv[2];
		
		// first, check the output format from the file name or file extension
		FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(output_filename);
		
		if(out_fif != FIF_UNKNOWN)
		{
			// then save the file
			FreeImage_Save(out_fif, rotated, output_filename, 0);
		}
		
		// free the loaded FIBITMAP
		FreeImage_Unload(dib);
		
		// free the loaded FIBITMAP
		FreeImage_Unload(rotated);		
	}
	
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
	
	return 0;
}
