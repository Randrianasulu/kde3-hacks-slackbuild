/*
Little cms - profiler construction set
Copyright (C) 1998-2001 Marti Maria <marti@littlecms.com>

THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.

IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

As a special exception to the GNU General Public License, if you
distribute this file as part of a program that contains a
configuration script generated by Autoconf, you may include it under
the same distribution terms that you use for the rest of that program.
*/

/* Version 1.09a */

#ifndef __cmsprf_H

#include <config.h>
#include LCMS_HEADER

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NON_WINDOWS
#  ifndef stricmp
#     define stricmp strcasecmp
#  endif
#endif

#ifndef max
#define max(a,b) ((a) > (b)?(a):(b))
#endif


/* Matrix operations - arbitrary size ----------------------------------------------------- */

typedef struct {

    int       Cols, Rows;
    double**  Values;

    } MATN,FAR* LPMATN;

// See B.K.O #148930: compile with lcms v.1.17
#if (LCMS_VERSION > 116)
typedef LCMSBOOL BOOL;
#endif

LPMATN      cdecl MATNalloc(int Rows, int Cols);
void        cdecl MATNfree (LPMATN mat);
LPMATN      cdecl MATNmult(LPMATN a1, LPMATN a2);
double      cdecl MATNcross(LPMATN a);
void        cdecl MATNscalar (LPMATN a, double scl, LPMATN b);
LPMATN      cdecl MATNtranspose (LPMATN a);
BOOL        cdecl MATNsolve(LPMATN a, LPMATN b);


/* IT8.7 / CGATS.17-200x handling -------------------------------------------------------- */

#define cmsxIT8_ROWS                    12
#define cmsxIT8_COLS                    22
#define cmsxIT8_GRAYCOLS                24
#define cmsxIT8_NORMAL_PATCHES          (cmsxIT8_ROWS*cmsxIT8_COLS + cmsxIT8_GRAYCOLS)
#define cmsxIT8_CUSTOM_PATCHES          10
#define cmsxIT8_TOTAL_PATCHES           (cmsxIT8_NORMAL_PATCHES + cmsxIT8_CUSTOM_PATCHES)


LCMSHANDLE  cdecl cmsxIT8Alloc(void);
void        cdecl cmsxIT8Free(LCMSHANDLE cmsxIT8);
LCMSHANDLE  cdecl cmsxIT8LoadFromFile(const char* cFileName);
LCMSHANDLE  cdecl cmsxIT8LoadFromMem(void *Ptr, size_t len);
BOOL        cdecl cmsxIT8SaveToFile(LCMSHANDLE cmsxIT8, const char* cFileName);
const char* cdecl cmsxIT8GetSheetType(LCMSHANDLE hIT8);
BOOL        cdecl cmsxIT8SetSheetType(LCMSHANDLE hIT8, const char* Type);
const char* cdecl cmsxIT8GetPatchName(LCMSHANDLE hIT8, int nPatch, char* buffer);
BOOL        cdecl cmsxIT8SetProperty(LCMSHANDLE hcmsxIT8, const char* cProp, const char *Str);
BOOL        cdecl cmsxIT8SetPropertyDbl(LCMSHANDLE hcmsxIT8, const char* cProp, double Val);
const char* cdecl cmsxIT8GetProperty(LCMSHANDLE hcmsxIT8, const char* cProp);
double      cdecl cmsxIT8GetPropertyDbl(LCMSHANDLE hcmsxIT8, const char* cProp);
int         cdecl cmsxIT8EnumProperties(LCMSHANDLE cmsxIT8, char ***PropertyNames);
int         cdecl cmsxIT8EnumDataFormat(LCMSHANDLE cmsxIT8, char ***SampleNames);
BOOL        cdecl cmsxIT8SetDataFormat(LCMSHANDLE cmsxIT8, int n, const char *Sample);

BOOL        cdecl cmsxIT8GetDataSetByPos(LCMSHANDLE IT8, int col, int row, char* Val, int ValBufferLen);

BOOL        cdecl cmsxIT8GetDataSet(LCMSHANDLE cmsxIT8, const char* cPatch,
                        const char* cSample,
                        char* Val, int ValBuffLen);

BOOL        cdecl cmsxIT8GetDataSetDbl(LCMSHANDLE cmsxIT8, const char* cPatch, const char* cSample, double* v);

BOOL        cdecl cmsxIT8SetDataSet(LCMSHANDLE cmsxIT8, const char* cPatch,
                        const char* cSample,
                        char *Val);

BOOL        cdecl cmsxIT8SetDataSetDbl(LCMSHANDLE cmsxIT8, const char* cPatch, const char* cSample, double Val);

const char *cdecl cmsxIT8GenericPatchName(int nPatch, char* buffer);



/* Patch collections (measurement lists) -------------------------------------------------- */

#define PATCH_HAS_Lab         0x00000001
#define PATCH_HAS_XYZ         0x00000002
#define PATCH_HAS_RGB         0x00000004
#define PATCH_HAS_CMY         0x00000008
#define PATCH_HAS_CMYK        0x00000010
#define PATCH_HAS_HEXACRM     0x00000020
#define PATCH_HAS_STD_Lab     0x00010000
#define PATCH_HAS_STD_XYZ     0x00020000
#define PATCH_HAS_XYZ_PROOF   0x00100000  
#define PATCH_HAS_MEAN_DE     0x01000000
#define PATCH_HAS_STD_DE      0x02000000
#define PATCH_HAS_CHISQ       0x04000000


#define MAXPATCHNAMELEN     20
/* A patch in memory */

typedef struct {

                DWORD        dwFlags;   /* Is quite possible to have colorant in only */
                                        /* some patches of sheet, so mark each entry with */
                                        /* the values it has. */

                char Name[MAXPATCHNAMELEN];

                cmsCIELab Lab;          /* The tristimulus values of target */
                cmsCIEXYZ XYZ;

				cmsCIEXYZ XYZProof;		/* The absolute XYZ value returned by profile */
										/* (gamut constrained to device) */

                union {                 /* The possible colorants. Only one space is */
                                        /* allowed...obviously only one set of */
                                        /* device-dependent values per patch does make sense. */
                        double RGB[3];
                        double CMY[3];
                        double CMYK[4];
                        double Hexa[MAXCHANNELS];

                        } Colorant;             

                double dEStd;               /* Standard deviation  */
                double ChiSq;               /* Chi-square parameter (mean of STD of colorants) */
                double dEMean;              /* Mean dE */

           } PATCH, FAR* LPPATCH;



/* A set of patches is simply an array of bools, TRUE if the patch */
/* belong to the set, false otherwise. */

typedef BOOL* SETOFPATCHES;

/* This struct holds whole Patches collection */

typedef struct _measurement {

           int          nPatches;
           LPPATCH      Patches;
           SETOFPATCHES Allowed;

           } MEASUREMENT,FAR *LPMEASUREMENT;


void         cdecl cmsxPCollFreeMeasurements(LPMEASUREMENT m);
SETOFPATCHES cdecl cmsxPCollBuildSet(LPMEASUREMENT m, BOOL lDefault);

BOOL         cdecl cmsxPCollBuildMeasurement(LPMEASUREMENT m, 
                                             const char *ReferenceSheet,
                                             const char *MeasurementSheet,
                                             DWORD dwNeededSamplesType);

int          cdecl cmsxPCollCountSet(LPMEASUREMENT m, SETOFPATCHES Set);
BOOL         cdecl cmsxPCollValidatePatches(LPMEASUREMENT m, DWORD dwFlags);

BOOL         cdecl cmsxPCollLoadFromSheet(LPMEASUREMENT m, LCMSHANDLE hSheet);
BOOL         cdecl cmsxPCollSaveToSheet(LPMEASUREMENT m, LCMSHANDLE it8);

LPPATCH      cdecl cmsxPCollGetPatch(LPMEASUREMENT m, int n);
LPPATCH      cdecl cmsxPCollGetPatchByName(LPMEASUREMENT m, const char* Name, int* lpPos);
LPPATCH      cdecl cmsxPCollGetPatchByPos(LPMEASUREMENT m, int row, int col);
LPPATCH      cdecl cmsxPCollAddPatchRGB(LPMEASUREMENT m, const char *Name,
                                        double r, double g, double b,
                                        LPcmsCIEXYZ XYZ, LPcmsCIELab Lab);

void         cdecl cmsxPCollLinearizePatches(LPMEASUREMENT m, SETOFPATCHES Valids,
                                             LPGAMMATABLE Gamma[3]);

/* Extraction utilities */

/* Collect "need" patches of the specific kind, return the number of collected (that */
/* could be less if set of patches is exhausted) */

void         cdecl cmsxPCollPatchesGS(LPMEASUREMENT m, SETOFPATCHES Result);

int          cdecl cmsxPCollPatchesNearRGB(LPMEASUREMENT m, SETOFPATCHES Valids,
                                            double r, double g, double b, int need, SETOFPATCHES Result);

int          cdecl cmsxPCollPatchesNearNeutral(LPMEASUREMENT m, SETOFPATCHES Valids,
                                            int need, SETOFPATCHES Result);

int          cdecl cmsxPCollPatchesNearPrimary(LPMEASUREMENT m, SETOFPATCHES Valids,
                                           int nChannel, int need, SETOFPATCHES Result);

int          cdecl cmsxPCollPatchesInLabCube(LPMEASUREMENT m, SETOFPATCHES Valids, 
                                           double Lmin, double LMax, double a, double b, SETOFPATCHES Result);

int          cdecl cmsxPCollPatchesInGamutLUT(LPMEASUREMENT m, SETOFPATCHES Valids, 
                                              LPLUT Gamut, SETOFPATCHES Result);

/* Find important values */

LPPATCH		 cdecl cmsxPCollFindWhite(LPMEASUREMENT m, SETOFPATCHES Valids, double* Distance);
LPPATCH		 cdecl cmsxPCollFindBlack(LPMEASUREMENT m, SETOFPATCHES Valids, double* Distance);
LPPATCH		 cdecl cmsxPCollFindPrimary(LPMEASUREMENT m, SETOFPATCHES Valids, int Channel, double* Distance);

/* Multiple linear regression stuff ---------------------------------------- */


/* A measurement of error */

typedef struct {

        double SSE;             /* The error sum of squares */
        double MSE;             /* The error mean sum of squares */
        double SSR;             /* The regression sum of squares */
        double MSR;             /* The regression mean sum of squares */
        double SSTO;            /* Total sum of squares */
        double F;               /* The Fisher-F value (MSR / MSE) */
        double R2;              /* Proportion of variability explained by the regression */
                                /* (root is Pearson correlation coefficient) */

        double R2adj;           /* The adjusted coefficient of multiple determination. */
                                /* R2-adjusted or R2adj. This is calculated as */
                                /* R2adj = 1 - (1-R2)(N-n-1)/(N-1) */
                                /* and used as multiple correlation coefficient */
                                /* (really, it should be square root) */

    } MLRSTATISTICS, FAR* LPMLRSTATISTICS;


int  cdecl cmsxRegressionCreateMatrix(LPMEASUREMENT m, SETOFPATCHES Allowed, int nterms,
                                       int ColorSpace,
                                       LPMATN* lpMat, LPMLRSTATISTICS Stat);

BOOL cdecl cmsxRegressionRGB2Lab(double r, double g, double b,
                                       LPMATN tfm, LPcmsCIELab Lab);

BOOL cdecl cmsxRegressionRGB2XYZ(double r, double g, double b,
                                       LPMATN tfm, LPcmsCIEXYZ XYZ);

BOOL cdecl cmsxRegressionInterpolatorRGB(LPMEASUREMENT m,
                                       int ColorSpace,                            
                                       int    RegressionTerms,
                                       BOOL   lUseLocalPatches,
                                       int    MinPatchesToCollect,
                                       double r, double g, double b,
                                       void* Res);



/* Levenberg-Marquardt ---------------------------------------------------------------------- */

LCMSHANDLE cdecl cmsxLevenbergMarquardtInit(LPSAMPLEDCURVE x, LPSAMPLEDCURVE y, double sig,
								double a[],
								int ma,  
								void (*funcs)(double, double[], double*, double[], int)
								);

double    cdecl cmsxLevenbergMarquardtAlamda(LCMSHANDLE hMRQ);
double    cdecl cmsxLevenbergMarquardtChiSq(LCMSHANDLE hMRQ);
BOOL      cdecl cmsxLevenbergMarquardtIterate(LCMSHANDLE hMRQ);
BOOL      cdecl cmsxLevenbergMarquardtFree(LCMSHANDLE hMRQ);


/* Convex hull geometric routines ------------------------------------------------------------ */

LCMSHANDLE cdecl cmsxHullInit(void);
void	   cdecl cmsxHullDone(LCMSHANDLE hHull);
BOOL	   cdecl cmsxHullAddPoint(LCMSHANDLE hHull, int x, int y, int z);
BOOL	   cdecl cmsxHullComputeHull(LCMSHANDLE hHull);
char	   cdecl cmsxHullCheckpoint(LCMSHANDLE hHull, int x, int y, int z);
BOOL       cdecl cmsxHullDumpVRML(LCMSHANDLE hHull, const char* fname);


/* Linearization ---------------------------------------------------------------------------- */


#define MEDIUM_REFLECTIVE_D50	0	/* Used for scanner targets */
#define MEDIUM_TRANSMISSIVE		1	/* Used for monitors & projectors */
           
void cdecl cmsxComputeLinearizationTables(LPMEASUREMENT m, 
                                    int ColorSpace, 
                                    LPGAMMATABLE Lin[3],
									int nResultingPoints,
									int Medium);                                    
                     
         
void		 cdecl cmsxCompleteLabOfPatches(LPMEASUREMENT m, SETOFPATCHES Valids, int Medium);
LPGAMMATABLE cdecl cmsxEstimateGamma(LPSAMPLEDCURVE X, LPSAMPLEDCURVE Y, int nResultingPoints);
                                            
void cdecl cmsxApplyLinearizationTable(double In[3], LPGAMMATABLE Gamma[3], double Out[3]);
void cdecl cmsxApplyLinearizationGamma(WORD In[3], LPGAMMATABLE Gamma[3], WORD Out[3]);

/* Support routines ---------------------------------------------------------------------- */

double cdecl _cmsxSaturate65535To255(double d);
double cdecl _cmsxSaturate255To65535(double d);
void   cdecl _cmsxClampXYZ100(LPcmsCIEXYZ xyz);

/* Matrix shaper profiler API ------------------------------------------------------------- */


BOOL cdecl cmsxComputeMatrixShaper(const char* ReferenceSheet,                              
                                   const char* MeasurementSheet,
								   int Medium,
                                   LPGAMMATABLE TransferCurves[3],
                                   LPcmsCIEXYZ WhitePoint,
                                   LPcmsCIEXYZ BlackPoint,
                                   LPcmsCIExyYTRIPLE Primaries);


/* Common to all profilers ------------------------------------------------------------------- */

#define MAX_STR 256

typedef int (* cmsxGAUGER)(const char *Label, int nMin, int nMax, int Pos);
typedef int (* cmsxPRINTF)(const char *Frm, ...);

typedef struct {

           /* Files */
           char ReferenceSheet[MAX_PATH];
           char MeasurementSheet[MAX_PATH];
           char OutputProfileFile[MAX_PATH];

           /* Some infos            */
           char Description[MAX_STR];
           char Manufacturer[MAX_STR];
           char Model[MAX_STR];
           char Copyright[MAX_STR];

           /* Callbacks */
           cmsxGAUGER Gauger;
           cmsxPRINTF printf;

           /* EndPoints */
           cmsCIEXYZ WhitePoint;            /* Black point in 0.xxx notation */
           cmsCIEXYZ BlackPoint;            /* Black point in 0.xxx notation */
           cmsCIExyYTRIPLE Primaries;       /* The primaries */
           LPGAMMATABLE Gamma[3];           /* Gamma curves */

		   /* Profile */ 
		   cmsHPROFILE hProfile;            /* handle to profile */

           icProfileClassSignature DeviceClass;
           icColorSpaceSignature   ColorSpace;

           int PCSType;                    /* PT_XYZ or PT_Lab */
           int CLUTPoints;                 /* Final CLUT resolution */
		   int ProfileVerbosityLevel;	   /* 0=minimum, 1=additional, 2=Verbose, 3=Any suitable */

           
		   /* Measurement */	
           MEASUREMENT m;                /* Contains list of available patches */
		   int Medium;


		   /* RGB Gamut hull */
		   LCMSHANDLE hRGBHull;   /* Contains bobbin of valid RGB values */

		   /* CIECAM97s */
		   BOOL lUseCIECAM97s;   /* Use CIECAM97s for chromatic adaptation? */

		   cmsViewingConditions device;     /* Viewing condition of source */
           cmsViewingConditions PCS;        /* Viewing condition of PCS */

           LCMSHANDLE hDevice;              /* CIECAM97s models used for adaptation */
           LCMSHANDLE hPCS;                 /* and viewing conditions */
           

    } PROFILERCOMMONDATA,FAR* LPPROFILERCOMMONDATA;
 

/* Shared routines */

BOOL cdecl cmsxEmbedCharTarget(LPPROFILERCOMMONDATA hdr);
BOOL cdecl cmsxEmbedMatrixShaper(LPPROFILERCOMMONDATA hdr);
BOOL cdecl cmsxEmbedTextualInfo(LPPROFILERCOMMONDATA hdr);

int  cdecl cmsxFindOptimumNumOfTerms(LPPROFILERCOMMONDATA hdr, int nMaxTerms, BOOL* lAllOk);
void cdecl cmsxChromaticAdaptationAndNormalization(LPPROFILERCOMMONDATA hdr, LPcmsCIEXYZ xyz, BOOL lReverse);
void cdecl cmsxInitPCSViewingConditions(LPPROFILERCOMMONDATA hdr);
void cdecl cmsxComputeGamutHull(LPPROFILERCOMMONDATA hdr);
BOOL cdecl cmsxChoosePCS(LPPROFILERCOMMONDATA hdr);

/* Monitor profiler API ------------------------------------------------------------------- */

typedef struct {

           PROFILERCOMMONDATA hdr;
                                                
                     
           LPGAMMATABLE         Prelinearization[3];    /* Canonic gamma */
           LPGAMMATABLE         ReverseTables[3];       /* Reverse (direct) gamma */
           LPGAMMATABLE         PreLab[3];
           LPGAMMATABLE         PreLabRev[3];
         

           MAT3                 PrimariesMatrix;
           MAT3                 PrimariesMatrixRev; 
         

           } MONITORPROFILERDATA,FAR* LPMONITORPROFILERDATA;



BOOL   cdecl cmsxMonitorProfilerInit(LPMONITORPROFILERDATA sys);
BOOL   cdecl cmsxMonitorProfilerDo(LPMONITORPROFILERDATA sys);


/* Scanner profiler API ------------------------------------------------------------------- */


typedef struct {

           PROFILERCOMMONDATA hdr;
                                                                                   
           LPGAMMATABLE Prelinearization[3];        
                             
		   LPMATN HiTerms;      /* Regression matrix of many terms */
		   LPMATN LoTerms;      /* Low order regression matrix used for extrapolation */
		   
		   BOOL   lLocalConvergenceExtrapolation;
			 		                          

           } SCANNERPROFILERDATA,FAR* LPSCANNERPROFILERDATA;




BOOL   cdecl cmsxScannerProfilerInit(LPSCANNERPROFILERDATA sys);                                       
BOOL   cdecl cmsxScannerProfilerDo(LPSCANNERPROFILERDATA sys);

/* ----------------------------------------------------------- end of profilers */


#ifdef __cplusplus
}
#endif

#define __cmsprf_H
#endif
