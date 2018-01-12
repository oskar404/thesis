
/*****************************************************************/
/*                        clr.c                                  */
/*****************************************************************/

/*

    MODULE PURPOSE:
        This module contains routines for color
        transformations, color space manipulations,
        and spectral sampling.

    MODULE CONTENTS:
        ClrInit          - initialized the color routines
        ClrReadMtl       - read a material file
        ClrAddSpect      - add two spectral curves
        ClrMultSpect     - multiply two spectral curves
        ClrScaleSpect    - scale a spectral curve
        ClrAreaSpect     - get the area under a curve
        ClrSpectToXYZ    - sample a curve to xyz
        ClrSpectToRGB    - sample a curve to rgb
        ClrGetCIEXYZ     - get the CIEXYZ matching functions
        ClrGetRGB        - returns the color space primaries
        ClrGetMinWL      - returns the max wavelength
        ClrGetMaxWL      - returns the min wavelength
        ClrGetXYZ_RGB    - returns the xyz to rgb matrix
        ClrGetRGB_XYZ    - returns the rgb to xyz matrix
        ClrGetRGB_YIQ    - returns the rgb to yiq matrix
        ClrGetYIQ_RGB    - returns the yiq to rgb matrix
        ClrRGBtoAuxRGB   - returns the matrix from the
                           current color space to an
                           auxiliary color space
        ClrXYZ_LAB       - transforms from xyz to Lab
        ClrXYZ_LUV       - transforms from xyz to Luv
        ClrTConcat       - concatenate color transforms
        ClrTInverse      - return an inverse transform
        ClrExit          - finish with the color routines

    NOTES:
        > Spectral curve manipulations use the lower and upper
          bounds established at init. Curves are at 1 nm
          increments and are arrays of doubles, (max - min + 1)
          elements long.
        > Matrix scaling is so that an RGB of 1,1,1 transforms
          to an XYZ with a Y value of 1.0
        > The XYZ and RGB sampling from spectral curves are
          scaled so that that a mirror reflector (1.0 for all
          wavelengths) has a Y value of 1.0.
        > Wavelength bounds of 380nm to 780nm (the visible
          spectrum) are appropriate for most applications.

*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "clr.h"



#define RED    0
#define GREEN  1
#define BLUE   2

#define WHITE  3
#define LOAD_MAT(a,b)     {\
                        int ii,jj; \
                        for (ii=0; ii<=2; ii++) \
                            for (jj=0; jj<=2; jj++) \
                                a[ii][jj] = b[ii][jj]; \
                        }



static int     init = FALSE;
static int     minWL = 380;
static int     maxWL = 780;
static double  XYZ_RGBmat[3][3];
static double  RGB_XYZmat[3][3];
static double  *X_tristim = NULL;
static double  *Y_tristim = NULL;
static double  *Z_tristim = NULL;
static double  *work_curve = NULL;
static double  XYZscale = 1.0;
static CLR_XYZ RGBprimary[4];
static int     ClrCSpaceToXYZ(CLR_XYZ *,double **);
static CLR_XYZ white;
static CLR_LUV refLUV;



/*
    This is the RGB to YIQ matrix and YIQ to RGB matrix as
    given in Sect 5.2 NTSC and RGB Video
*/

static double  RGB_YIQmat[3][3] =  {{0.299, 0.587, 0.144},
                                    {0.596, -0.275, -0.321},
                                    {0.212, -0.528, 0.311}};

static double  YIQ_RGBmat[3][3] =  {{1.0000, 0.9557, 0.6199},
                                    {1.0000, -0.2716, -0.6469},
                                    {1.0000, -1.1082, 1.7051}};



/*
    This is the NTSC primaries with D6500 white point for use as
    the default initialization as given in sect 5.1.1 Color
    Correction for Display.
*/

static CLR_XYZ RGB_NTSC[4] = {{0.670, 0.330, 0.000},   /* red */
                              {0.210, 0.710, 0.080},   /* green */
                              {0.140, 0.080, 0.780},   /* blue */
                              {0.313, 0.329, 0.358}};  /* white */



/*
    This is the data for the CIEXYZ curves take from Judd and
    Wyszecki (1975}, table 2.6, these are for the 1931 standard
    observer with a 2-degree visual field.
*/

static double       CIEXYZ[81][4] = {
         {380, 0.0014, 0.0000, 0.0065}, {385, 0.0022, 0.0001, 0.0105},
         {390, 0.0042, 0.0001, 0.0201}, {395, 0.0076, 0.0002, 0.0362},
         {400, 0.0143, 0.0004, 0.0679}, {405, 0.0232, 0.0006, 0.1102},
         {410, 0.0435, 0.0012, 0.2074}, {415, 0.0776, 0.0022, 0.3713},
         {420, 0.1344, 0.0040, 0.6456}, {425, 0.2148, 0.0073, 1.0391},
         {430, 0.2839, 0.0116, 1.3856}, {435, 0.3285, 0.0168, 1.6230},
         {440, 0.3483, 0.0230, 1.7471}, {445, 0.3481, 0.0298, 1.7826},
         {450, 0.3362, 0.0380, 1.7721}, {455, 0.3187, 0.0480, 1.7441},
         {460, 0.2908, 0.0600, 1.6692}, {465, 0.2511, 0.0739, 1.5281},
         {470, 0.1954, 0.0910, 1.2876}, {475, 0.1421, 0.1126, 1.0419},
         {480, 0.0956, 0.1390, 0.8310}, {485, 0.0580, 0.1693, 0.6162},
         {490, 0.0320, 0.2080, 0.4652}, {495, 0.0147, 0.2586, 0.3533},
         {500, 0.0049, 0.3230, 0.2720}, {505, 0.0024, 0.4073, 0.2123},
         {510, 0.0093, 0.5030, 0.1582}, {515, 0.0291, 0.6082, 0.1117},
         {520, 0.0633, 0.7100, 0.0782}, {525, 0.1096, 0.7932, 0.0573},
         {530, 0.1655, 0.8620, 0.0422}, {535, 0.2257, 0.9149, 0.0298},
         {540, 0.2904, 0.9540, 0.0203}, {545, 0.3597, 0.9803, 0.0134},
         {550, 0.4334, 0.9950, 0.0087}, {555, 0.5121, 1.0000, 0.0057},
         {560, 0.5945, 0.9950, 0.0039}, {565, 0.6784, 0.9786, 0.0027},
         {570, 0.7621, 0.9520, 0.0021}, {575, 0.8425, 0.9154, 0.0018},
         {580, 0.9163, 0.8700, 0.0017}, {585, 0.9786, 0.8163, 0.0014},
         {590, 1.0263, 0.7570, 0.0011}, {595, 1.0567, 0.6949, 0.0010},
         {600, 1.0622, 0.6310, 0.0008}, {605, 1.0456, 0.5668, 0.0006},
         {610, 1.0026, 0.5030, 0.0003}, {615, 0.9384, 0.4412, 0.0002},
         {620, 0.8544, 0.3810, 0.0002}, {625, 0.7514, 0.3210, 0.0001},
         {630, 0.6424, 0.2650, 0.0000}, {635, 0.5419, 0.2170, 0.0000},
         {640, 0.4479, 0.1750, 0.0000}, {645, 0.3608, 0.1382, 0.0000},
         {650, 0.2835, 0.1070, 0.0000}, {655, 0.2187, 0.0816, 0.0000},
         {660, 0.1649, 0.0610, 0.0000}, {665, 0.1212, 0.0446, 0.0000},
         {670, 0.0874, 0.0320, 0.0000}, {675, 0.0636, 0.0232, 0.0000},
         {680, 0.0468, 0.0170, 0.0000}, {685, 0.0329, 0.0119, 0.0000},
         {690, 0.0227, 0.0082, 0.0000}, {695, 0.0158, 0.0057, 0.0000},
         {700, 0.0114, 0.0041, 0.0000}, {705, 0.0081, 0.0029, 0.0000},
         {710, 0.0058, 0.0021, 0.0000}, {715, 0.0041, 0.0015, 0.0000},
         {720, 0.0029, 0.0010, 0.0000}, {725, 0.0020, 0.0007, 0.0000},
         {730, 0.0014, 0.0005, 0.0000}, {735, 0.0010, 0.0004, 0.0000},
         {740, 0.0007, 0.0002, 0.0000}, {745, 0.0005, 0.0002, 0.0000},
         {750, 0.0003, 0.0001, 0.0000}, {755, 0.0002, 0.0001, 0.0000},
         {760, 0.0002, 0.0001, 0.0000}, {765, 0.0001, 0.0000, 0.0000},
         {770, 0.0001, 0.0000, 0.0000}, {775, 0.0001, 0.0000, 0.0000},
         {780, 0.0000, 0.0000, 0.0000} };



/*****************************************************************

    int ClrInit (int min,int max,CLR_XYZ rgb[4]}
        min, max (in) wavelength bounds (nm)
        rgb (in) the primaries. If NULL, then the NTSC primaries with
                 a D6500 white point are used.

    Initializes the color routines for a spectral range from
    'min' nm to 'max' nm and an RGB color space given by 'rgb'
    Returns TRUE if successful and FALSE on error.

*/

int ClrInit (int min,int max,CLR_XYZ rgb[4])
{
    int color, ct;
    double len;

    if (init)
        return FALSE;
    init = TRUE;
    minWL = min;
    maxWL = max;

    /*
        load primaries and build transformations, use the
        defaults is rgb==NULL
    */

    for (color=RED; color<=WHITE; color++)
        {
        if (rgb == NULL)
            RGBprimary[color] = RGB_NTSC[color];
        else
            RGBprimary[color] = rgb[color];
        RGBprimary[color].z=1.0-RGBprimary[color].x-RGBprimary[color].y;
        }
    if (!ClrCSpaceToXYZ(RGBprimary, RGB_XYZmat))
        goto error;
    if (!ClrTInverse(RGB_XYZmat, XYZ_RGBmat))
        goto error;

    /*
        build reference info for L*a*b*, L*u*v* transforms
    */

    white.x =(100.0*RGBprimary[WHITE].x)/RGBprimary[WHITE].y;
    white.y = 100.0;
    white.z =(100.0*RGBprimary[WHITE].z)/RGBprimary[WHITE].y;
    refLUV.u=(4.0*white.x)/(white.x+(15.0*white.y)+(3.0*white.z));
    refLUV.v=(9.0*white.y)/(white.x+(15.0*white.y)+(3.0*white.z));

    /*
        build the XYZ sampling curves - allocate space for the
        curves and interpolate the CIEXYZ table into continuous
        curves at 1 nm increments.
    */

    {
        int ii, nm;
        double *x, *y, *z;
        double x_cur, y_cur, z_cur;
        double x_inc, y_inc, z_inc;

        if((work_curve=(double *)malloc((unsigned)
                    (sizeof(double) * (maxWL-minWL+1))))==NULL)
            goto error;
        if ((x = X_tristim=(double *)malloc((unsigned)
                    (sizeof(double) * (maxWL-minWL+1))))== NULL)
            goto error;
        if ((y = Y_tristim = (double *)malloc((unsigned)
                    (sizeof(double) * (maxWL-minWL+1))))== NULL)
            goto error;
        if ((z = Z_tristim = (double *)malloc((unsigned)
                    (sizeof(double) * (maxWL-minWL+1))))== NULL)
            goto error;
        for (nm=minWL; nm<380; nm++)
            *x++ = *y++ = *z++ = 0.0;
        for (ct=0; ct<80; ct++)
            {
            x_cur = CIEXYZ[ct][1];
            y_cur = CIEXYZ[ct][2];
            z_cur = CIEXYZ[ct][3];
            x_inc = (CIEXYZ[ct+1][1] - x_cur) / 5.0;
            y_inc = (CIEXYZ[ct+1][2] - y_cur) / 5.0;
            z_inc = (CIEXYZ[ct+1][3] - z_cur) / 5.0;
            for (ii=0; ii<5; ii++, nm++)
                {
                if (nm > maxWL)
                    goto XYZ_done;
                if (nm >= minWL)
                    {
                    *x++ = x_cur;
                    *y++ = y_cur;
                    *z++ = z_cur;
                    }
                x_cur += x_inc;
                y_cur += y_inc;
                z_cur += z_inc;
                }
            }
        if(nm <= maxWL)
            {
            *x++ = x_cur;
            *y++ = y_cur;
            *z++ = z_cur;
            nm++;
            }
        for ( ;nm<=maxWL; nm++)
            *x++ = *y++ = *z++ = 0.0;
        }

XYZ_done:

    /*
        determine the scaling factor to be used in sampling spectral
        curves to bring Y os a sampled identity curve to 1.
    */

    XYZscale = 1.0 / ClrAreaSpect(Y_tristim);
    return TRUE;

error:

    ClrExit();
    return FALSE;
}



/*****************************************************************

    int ClrReadMtl(char *file,int *conduct,double *n,double *k,
                   double *curve)
        file (in) material file name
        conduct (out) conductor - set to FALSE if not
                      specified in the material file
        n,k (out) n and k, - set to 0.0 if not
                  specified in the material file
        curve (out) curve array

    Returns TRUE if the material was read, FALSE if the material
    could not be found or an error was detected in the material
    file. NULL pointers can be given as arguments for 'conduct',
    'n', 'k', and/or 'curve' if the properties are not of interest.

    The material file must be in the format given in Sect III.7.1
    Material Data.

*/

int ClrReadMtl(char *file,int *conduct,double *n,double *k,double *curve)
{
    FILE *fp, *fopen();
    char      in_str[200];
    int       flag = 1;
    int       curWL, lastWL, indWL;
    double    cur_val, last_val, inc_val;

    if (!init)
        return FALSE;
    if ((fp=fopen(file,"r")) == NULL)
        return FALSE;
    if (n != NULL)
        *n = 0.0;
    if (k != NULL)
        *k = 0.0;
    if (conduct != NULL)
        *conduct = FALSE;
    if (curve == NULL)
        flag = -1;
    indWL = minWL;
    while (fgets(in_str,200,fp))
        {
        switch (in_str[0])
            {
            case '#':       /* comment */
                break;
            case 'k':       /* absorption coefficient */
                if (k != NULL)
                    (void)sscanf(in_str, "k %lf", k);
                break;
             case 'n':       /* index of refraction */
                if (n != NULL)
                    (void)sscanf (in_str, "n %lf", n);
                break;
             case 'c':       /* conductor */
                if (conduct != NULL)
                    *conduct = TRUE;
                break;
             case 'd':       /* dielectric */
                if (conduct != NULL)
                    *conduct = FALSE;
                break;
             default:        /* spectral data */
                if ((flag!=-1)&&
                        (sscanf(in_str,"%d %lf",&curWL,&cur_val)==2))
                    {
                    if (flag == 1)
                        {
                        flag = 0;
                        while (indWL <= curWL)
                            {
                            *curve++ = cur_val;
                            if (++indWL > maxWL)
                                {
                                flag = -1;
                                break;
                                }
                             }
                        }
                    else
                        {
                        if (curWL < lastWL)
                            {
                            (void)fclose(fp);
                            return FALSE;
                            }

                        inc_val = (cur_val-last_val)/(curWL-lastWL);
                        while (lastWL < indWL)
                            {
                            last_val += inc_val;
                            lastWL++;
                            }
                        while (indWL <= curWL)
                            {
                            *curve++ = last_val;
                            last_val += inc_val;
                            if (++indWL > maxWL)
                                {
                                flag = -1;
                                break;
                                }
                            }

                        }
                    lastWL = curWL;
                    last_val = cur_val;
                    }
            }

        }
    while (indWL <= maxWL)
        {
        *curve++ = last_val;
        indWL++;
        }
    (void)fclose(fp);
    return TRUE;
}



/*****************************************************************

    int ClrAddSpect (double *c1,double *c2,double *c3)
        cl, c2 (in)     - input curves
        c3   (out)         - output curve

    Add spectral curve 'c1' to spectral curve 'c2' to get spectral
    curve 'c3'. Returns TRUE if successful, FALSE if the CLR
    routines are not initialized.

*/

int ClrAddSpect (double *c1,double *c2,double *c3)
{
    int ct;

    if (!init)
        return FALSE;
    for (ct=maxWL-minWL+1; --ct>=0; )
        *c3++ = *c1++ + *c2++;
    return TRUE;
}



/*****************************************************************

    int ClrMultSpect (double *c1,double *c2,double *c3)
        cl, c2 (in) - input curves
        c3 (out)     - output curve

    Multiply spectral curve 'c1' by spectral curve 'c2' to get
    spectral curve 'c3'. Returns TRUE if successful, FALSE if
    the CLR routines are not initialized.

*/

int ClrMultSpect (double *c1,double *c2,double *c3)
{
    int ct;

    if (!init)
        return FALSE;
    for (ct=maxWL-minWL+1; --ct>=0; )
        *c3++ = *c1++ * *c2++;
    return TRUE;
}



/*****************************************************************

    int ClrScaleSpect (double *c1, double scale,double *c3)
        cl         (in) - input curve
        scale       (in) - scale
        c3         (out) - output curve

    Multiply spectral curve 'c1' by 'scale' to get spectral curve
    'c3'. Returns TRUE if successful, FALSE if the CLR routines
    are not initialized.

*/

int ClrScaleSpect (double *c1, double scale,double *c3)
{
    int ct;

    if (!init)
        return FALSE;
    for (ct=maxWL-minWL+1; --ct>=0; )
        *c3++ = *c1++ * scale;
    return TRUE;
}



/*****************************************************************

    double ClrAreaSpect (double *c1)
        c1 (in) - input curve

    Returns the are under the spectral curve 'c1'. Returns O if
    the CLR routines are not initialized.

*/

double ClrAreaSpect (double *c1)
{
    int ct;
    double area = 0.0;

    if (!init)
        return FALSE;
    for (ct=maxWL-minWL+1; --ct>=0; )
        area += *c1++;
    return area;
}



/*****************************************************************

    CLR_XYZ ClrSpectToXYZ(double *spectral)
        spectral (in) - the spectral curve

    Returns the sample values if successful, and a sample value
    of (0,0,0) if the CLR routines are not initialized.

    Multiplies the spectral curve by each of the sampling curves
    then integrates the resulting curves. The XYZ values are then

    normalized such that an identity material has Y=1.

*/

CLR_XYZ ClrSpectToXYZ(double *spectral)
{
    CLR_XYZ xyz;

    if (!init)
        {
        xyz.x = xyz.y = xyz.z = 0.0;
        return xyz;
        }
    (void)ClrMultSpect(spectral, X_tristim, work_curve);
    xyz.x = XYZscale * ClrAreaSpect(work_curve);
    (void)ClrMultSpect(spectral, Y_tristim, work_curve);
    xyz.y = XYZscale * ClrAreaSpect(work_curve);
    (void)ClrMultSpect(spectral, Z_tristim, work_curve);
    xyz.z = XYZscale * ClrAreaSpect(work_curve);
    return xyz;
}



/*****************************************************************

    CLR_RGB ClrSpectToRGB (double *spectral)
        spectral (in) - the spectral curve

    Returns the sample values if successful, and a sample value of
    (0,0,0) if the CLR routines are not initialized.

    The curve is first sampled to XYZ then transformed to RGB

*/

CLR_RGB ClrSpectToRGB (double *spectral)
{
    CLR_XYZ xyz;
    CLR_RGB rgb;

    if (!init)
        {
        rgb.r = rgb.g = rgb.b = 0.0;
        return rgb;
        }
    xyz = ClrSpectToXYZ (spectral);
    rgb.r = (XYZ_RGBmat[0][0] * xyz.x)+(XYZ_RGBmat[0][1] * xyz.y)
                                      +(XYZ_RGBmat[0][2] * xyz.z);
    rgb.g = (XYZ_RGBmat[1][0] * xyz.x)+(XYZ_RGBmat[1][1] * xyz.y)
                                      +(XYZ_RGBmat[1][2] * xyz.z);
    rgb.b = (XYZ_RGBmat[2][0] * xyz.x)+(XYZ_RGBmat[2][1] * xyz.y)
                                      +(XYZ_RGBmat[2][2] * xyz.z);
    return rgb;
}



/*****************************************************************

    int ClrGetCIEXYZ (double *X,double *Y,double *Z)
        X,Y,Z (mod) arrays to hold the curves

    Copies the XYZ sampling curves into the user supplied buffers.
    Returns TRUE is successful and FALSE if the CLR routines are
    not initialized.

*/

int ClrGetCIEXYZ (double *X,double *Y,double *Z)
{
    int ct;
    double *x,*y,*z;

    if (!init)
        return FALSE;
    x = X_tristim;
    y = Y_tristim;
    z = Z_tristim;
    for (ct=maxWL-minWL+1; --ct>=0; )
        {
        *X++ = *x++;
        *Y++ = *y++;
        *Z++ = *z++;
        }
    return TRUE;
}



/*****************************************************************

    int ClrGetRGB (CLR_XYZ rgb[4])
        rgb[] (mod) the primaries

    Fills 'rgb' with the primaries the CLR routines are initialized
    to. Returns TRUE if successful and FALSE if the CLR routines
    are not initialized.

*/

int ClrGetRGB (CLR_XYZ rgb[4])
{
    int ct;

    if (!init)
        return FALSE;
    for (ct=0; ct<=3; ct++)
        rgb[ct] = RGBprimary[ct];
    return TRUE;
}



/*****************************************************************

    int ClrGetMinWL ()

    Returns the minimum wavelength bound for which the CLR routines
    are initialized, returns -1 if the CLR routines are not
    initialized.

*/

int ClrGetMinWL (void)
{
    if (!init)
        return -1;
    return minWL;
}



/*****************************************************************

    int ClrGetMaxWL ()

    Returns the minimum wavelength bound for which the CLR routines
    are initialized, returns -1 if the CLR routines are not
    initialized.

*/

int ClrGetMaxWL (void)
{
    if (!init)
        return -1;
    return maxWL;
}



/*****************************************************************

    int ClrGetXYZ_RGB (double mat[3][3])
        mat (mod) - matrix to be loaded

    Copies the CIEXYZ to RGB transformation into the user supplied
    matrix. Returns TRUE if successful and FALSE if the CLR
    routines are not initialized.

*/

int ClrGetXYZ_RGB (double **mat)
{
    if (!init)
        return FALSE;
    LOAD_MAT(mat, XYZ_RGBmat);
    return TRUE;
}



/*****************************************************************

    int ClrGetRGB_XYZ (double mat[3][3])
        mat (mod) - matrix to be loaded

    Copies the RGB to CIEXYZ transformation into the user supplied
    matrix. Returns TRUE if successful and FALSE if the CLR
    routines are not initialized.

*/

int ClrGetRGB_XYZ (double **mat)
{
    if (!init)
        return FALSE;
    LOAD_MAT(mat, RGB_XYZmat);
    return TRUE;
}



/*****************************************************************

    int ClrGetYIQ_RGB (double mat[3][3])
        mat  (mod) - matrix to be loaded

    Copies the YIQ to RGB transformation into the user supplied
    matrix. Returns TRUE if successful and FALSE if the CLR
    routines are not initialized.

*/

int ClrGetYIQ_RGB (double **mat)
{
    if (!init)
        return FALSE;
    LOAD_MAT(mat, YIQ_RGBmat);
    return TRUE;
}

/*****************************************************************

    int ClrGetRGB_YIQ (double mat[3][3])
        mat (mod) - matrix to be loaded

    Copies the RGB to YIQ transformation into the user supplied
    matrix. Returns TRUE if successful and FALSE if the CLR
    routines are not initialized.

*/

int ClrGetRGB_YIQ (double **mat)
{
    if (!init)
        return FALSE;
    LOAD_MAT(mat, RGB_YIQmat);
    return TRUE;
}



/*****************************************************************

    int ClrRGBtoAuxRGB (double to[3][3],double from[3][3],
                        CLR_XYZ rgb_aux[4])
        to (mod) - matrix to aux rgb
        from (mod) - matrix from aux rgb
        rgb_aux (in) - the color space definition,
                       3 primaries and white

    Creates the transformations between RGB color space the CLR
    routines are initialized for and another RGB color space as
    specified in 'aux_rgb'. The 'to' and 'from' matrices are
    filled with the resulting transformations. TRUE is returned
    if successful, FALSE is returned if the CLR_ routines are not
    initialized or if there is a singularity detected when the
    transformation is being generated.

    The technique used is described, Colorimetry and
    the RGB Monitor.

*/

int ClrRGBtoAuxRGB (double **to,double **from,
                        CLR_XYZ *rgb_aux)
{
    CLR_XYZ rgb_tmp[4];
    double rgb_xyz_aux[3][3], xyz_rgb_aux[3][3];
    int color;

    if (!init)
        return FALSE;

    /*
        normalize the chromaticities of the auxiliary primaries
        and white point.
    */

    for(color=RED; color<=WHITE; color++)
        {
        rgb_tmp[color] = rgb_aux[color];
        rgb_tmp[color].z = 1.0 - rgb_aux[color].x - rgb_aux[color].y;
        }

    /*
         get the transform between XYZ and the auxiliary RGB
    */

    if (!ClrCSpaceToXYZ(rgb_tmp, rgb_xyz_aux))
        return FALSE;

    if (!ClrTInverse(rgb_xyz_aux,xyz_rgb_aux))
        return FALSE;

    /*
        concatenate with the transforms for the RGB color space
        that the CLR_ routine set is initialized to
    */

    (void)ClrTConcat(xyz_rgb_aux, RGB_XYZmat, to);
    (void)ClrTConcat(XYZ_RGBmat, rgb_xyz_aux, from);
    return TRUE;
}



/*****************************************************************

    CLR_LAB ClrXYZtoLAB (CLR_XYZ xyz)
        xyz (in) - xyz color (0.01<=Y<=1)

    Returns L*a*b* given XYZ. Returns (0,0,0) if the CLR routines
    are not initialized. This transformation , Alternate Color
    Representations, is taken from Judd and Wyszecki (1975) pg.320.
    The transformation is scaled for (.01 < Y < 1) for consistency
    within the CLR routine set.

*/

CLR_LAB ClrXYZtoLAB (CLR_XYZ xyz)
{
    CLR_LAB lab;

    if (!init)
        {
        lab.l = lab.a = lab.b = 0.0;
        return lab;
        }
    xyz.x *= 100.0;
    xyz.y *= 100.0;
    xyz.z *= 100.0;
    lab.l = 25.0 * pow(((100.0*xyz.y)/white.y),.33333) - 16.0;
    lab.a = 500.0 * (pow(xyz.x/white.x,.33333)
                  - pow(xyz.y/white.y,.33333));
    lab.b = 200.0 * (pow(xyz.y/white.y,.33333)
                  - pow(xyz.z/white.z,.33333));
    return lab;
}

/*****************************************************************

    CLR_LUV ClrXYZtoLUV (CLR_XYZ xyz)
        xyz (in) - xyz color (0.01<=Y<=1)

    Returns L*u*v* given XYZ. Returns (0,0,0) if the CLR routines
    are not initialized. This transformation is taken from Judd
    and Wyszecki (1975) pg.328. The transformation is scaled for
    (.01 < Y < 1) for consistency within the CLR routine set.

*/

CLR_LUV ClrXYZtoLUV (CLR_XYZ xyz)
{
    CLR_LUV luv;
    double      u, v;

    if (!init)
        {
        luv.l = luv.u = luv.v = 0.0;
        return luv;
        }
    xyz.x *= 100.0;
    xyz.y *= 100.0;
    xyz.z *= 100.0;
    luv.l = 25.0 * pow(((100.0*xyz.y)/white.y),.33333) - 16.0;
    u = (4.0 * xyz.x) / (xyz.x + (15.0 * xyz.y) + (3.0 * xyz.z));
    v = (9.0 * xyz.y) / (xyz.x + (15.0 * xyz.y) + (3.0 * xyz.z));
    luv.u = 13.0 * luv.l * (u - refLUV.u);
    luv.v = 13.0 * luv.l * (v - refLUV.v);
    return luv;
}



/*****************************************************************

    int ClrTConcat (double m1[3][3],double m2[3][3],double m3[3][3])
        m1 (in) - matrix
        m2 (in) - matrix to concat
        m3 (in) - concatenated matrix

    Concatenate m1 to m2 resulting in m3 . In use, suppose you
    have an XYZ to RGB and an RGB to YIQ matrix. Concatenate the
    RGB to YIQ matrix to the XYZ to RGB matrix to get an XYZ to
    YIQ matrix. Returns TRUE.

*/

int ClrTConcat (double **m1,double **m2,double **m3)
{
    double t1[3][3], t2[3][3];
    LOAD_MAT(t1,m1);
    LOAD_MAT(t2,m2);
    {
        int ii,jj;

        for (ii=0; ii<=2; ii++)
            for (jj=0; jj<=2; jj++)
                m3[ii][jj] = (t1[ii][0] * t2[0][jj])
                            +(t1[ii][1] * t2[1][jj])
                            +(t1[ii][2] * t2[2][jj]);
    }
    return TRUE;
}



/*****************************************************************

    int ClrTInverse (double mat[3][3],double inv_mat[3][3])
        mat (in) - matrix to be inverted
        inv_mat (mod) - inverted matrix

    Inverts mat using Gaussian elimination. Returns TRUE if
    successful and FALSE if there is a singularity.

*/

int ClrTInverse (double **mat,double **inv_mat)
{
    int ii, jj, kk;
    double tmp_mat[3][3], tmp_d;

    for (ii=0; ii<=2; ii++)
        for (jj=0; jj<=2; jj++)
            {
            tmp_mat[ii][jj] = mat[ii][jj];
             inv_mat[ii][jj] = (ii==jj ? 1.0 : 0.0);
            }
    for (ii=0; ii<=2; ii++)
        {
        for (jj=ii+1, kk=ii; jj<=2; jj++)
            if (fabs(tmp_mat[jj][ii]) > fabs(tmp_mat[kk][ii]))
                 kk = jj;

    /* check for singularity */

        if (tmp_mat[kk][ii] == 0.0)
            return FALSE;

    /* pivot - switch rows kk and ii */

        if (kk != ii)
            for (jj=0; jj<=2; jj++)
                {
                 tmp_d = tmp_mat[ii][jj];
                tmp_mat[ii][jj] = tmp_mat[kk][jj];
                 tmp_mat[kk][jj] = tmp_d;
                 tmp_d = inv_mat[ii][jj];
                 inv_mat[ii][jj] = inv_mat[kk][jj];
                inv_mat[kk][jj] = tmp_d;
                }

    /* normalize the row - make the diagonal 1 */

        for (tmp_d = 1.0 / tmp_mat[ii][ii], jj=0; jj<=2; jj++)
            {
             tmp_mat[ii][jj] *= tmp_d;
             inv_mat[ii][jj] *= tmp_d;
            }

    /* zero the non-diagonal terms in this column */

        for (jj=0; jj<=2; jj++)
            if (jj != ii)
                for (tmp_d = -tmp_mat[jj][ii], kk=0; kk<=2; kk++)
                    {
                     tmp_mat[jj][kk] += tmp_mat[ii][kk] * tmp_d;
                     inv_mat[jj][kk] += inv_mat[ii][kk] * tmp_d;
                    }
        }
    return TRUE;
}



/*****************************************************************

    int ClrCSpaceToXYZ (CLR_XYZ cspace[4],double t_mat[3][3])
        cspace (in) - the color space definition,
                      3 primaries and white
        t_mat (mod) - the color transformation

    Builds the transformation from a set of primaries to the CIEXYZ
    color space. This is the basis for the generation of the color
    transformations in the CLR routine set. Returns FALSE if there
    is a singularity.

*/

static int ClrCSpaceToXYZ (CLR_XYZ *cspace,double **t_mat)
{
    int ii, jj, kk, tmp_i, ind[3];
    double mult, white[3], scale[3];

    /* normalize the white point to Y=1 */

    if (cspace[WHITE].y <= 0.0)
        return FALSE;
    white[0] = cspace[WHITE].x / cspace[WHITE].y;
    white[1] = 1.0;
    white[2] = cspace[WHITE].z / cspace[WHITE].y;
    for (ii=0; ii<=2; ii++)
        {
        t_mat[0][ii] = cspace[ii].x;
        t_mat[1][ii] = cspace[ii].y;
        t_mat[2][ii] = cspace[ii].z;
        ind[ii] = ii;
        }

    /* gaussian elimination with partial pivoting */

    for (ii=0; ii<2; ii++)
        {
        for (jj=ii+1; jj<=2; jj++)
            if (fabs(t_mat[ind[jj]][ii])>fabs(t_mat[ind[ii]][ii]))
                {
                tmp_i=ind[jj];
                ind[jj]=ind[ii];
                ind[ii]=tmp_i;
                }
        if (t_mat[ind[ii]][ii] == 0.0)
            return FALSE;
        for (jj=ii+1; jj<=2; jj++)
            {
            mult = t_mat[ind[jj]][ii] / t_mat[ind[ii]][ii];
            for (kk=ii+1; kk<=2; kk++)
                t_mat[ind[jj]][kk] -= t_mat[ind[ii]][kk] * mult;
            white[ind[jj]] -= white[ind[ii]] * mult;
            }
        }
    if (t_mat[ind[2]][2] == 0.0)
        return FALSE;

    /* back substitution to solve for scale */

    scale[ind[2]] = white[ind[2]] / t_mat[ind[2]][2];
    scale[ind[1]] = (white[ind[1]] - (t_mat[ind[1]][2]
                  * scale[ind[2]])) / t_mat[ind[1]][1];
    scale[ind[0]] = (white[ind[0]] - (t_mat[ind[0]][1] * scale[ind[1]])
                  - (t_mat[ind[0]][2]*scale[ind[2]]))/t_mat[ind[0]][0];

    /* build matrix */

    for (ii=0; ii<=2; ii++)
        {
        t_mat[0][ii] = cspace[ii].x * scale[ii];
        t_mat[1][ii] = cspace[ii].y * scale[ii];
        t_mat[2][ii] = cspace[ii].z * scale[ii];
        }
    return TRUE;
}



/*****************************************************************

    int ClrExit()

    Completes use of the CLR routine set and frees any
    - allocated space

*/

int ClrExit(void)
{
    if (!init)
        return TRUE;
    (void)ClrExitSamples();
    if (X_tristim != NULL)
        free((char *)X_tristim);
    if (Y_tristim != NULL)
        free((char *)Y_tristim);
    if (Z_tristim != NULL)
        free((char *)Z_tristim);
    if (work_curve != NULL)
        free((char *)work_curve);
    X_tristim = Y_tristim = Z_tristim = work_curve = NULL;
    init = FALSE;
    return TRUE;
}
