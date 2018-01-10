/*****************************************************************/
/*                            clr.h                              */
/*****************************************************************/

/*         
  	iclude file for the color utilities
*/
         
#ifndef __CLR_H__
#define __CLR_H__
         


#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */
         

#define CLR_SAMPLE_MEYER 0
#define CLR_SAMPLE_HALL  1
         


/* 
	common color constructs
*/
         
typedef struct {double r, g, b;}   CLR_RGB;
typedef struct {double x, y, z;}   CLR_XYZ;
typedef struct {double l, a, b;}   CLR_LAB;
typedef struct {double l, u, v;}   CLR_LUV;
         


/*
	color routine declarations
*/
         
int  ClrInit(int,int,CLR_XYZ []);
int  ClrExit(void);
int  ClrReadMtl(char *,int *,double *,double *,double *);
int  ClrAddSpect(double *,double *,double *);
int  ClrMultSpect(double *,double *,double *);
int  ClrScaleSpect(double *,double,double *);
double    ClrAreaSpect(double *);
CLR_XYZ   ClrSpectToXYZ(double *);
CLR_RGB   ClrSpectToRGB(double *);
int  ClrGetRGB(CLR_XYZ *);
int  ClrGetMinWL(void);
int  ClrGetMaxWL(void);
int  ClrGetXYZ_RGB(double **);
int  ClrGetRGB_XYZ(double **);
int  ClrGetYIQ_RGB(double **);
int  ClrGetRGB_YIQ(double **);
int  ClrRGBtoAuxRGB(double **,double **,CLR_XYZ *);
CLR_LAB   ClrXYZtoLAB(CLR_XYZ);
CLR_LUV   ClrXYZtoLUV(CLR_XYZ);
int  ClrTConcat(double **,double **,double **);
int  ClrTInverse(double **,double **);

CLR_RGB ClrClampRGB(CLR_RGB);
CLR_RGB ClrScaleRGB(CLR_RGB);
CLR_RGB ClrClipRGB(CLR_RGB);

int  ClrInitSamples(int,int,int *);
int  ClrNumSamples(void);
int  ClrSpectToSample(double *,double *);
int  ClrGetSampleRGB(double *);
int  ClrGetSampleXYZ(double *);
int  ClrReconstruct(double *,double *);
int  ClrExitSamples(void);



#endif /* __CLR_H__ */

