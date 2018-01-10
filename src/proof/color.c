/**************************************************************/
/* This software was made by Oskar L”nnberg                   */
/*                                                            */
/* Copyright (c) by Oskar L”nnberg                            */
/*                                                            */
/* Permission to use, copy, modify, distribute, and sell this */
/* software and its documentation for any purpose is not      */
/* granted without permission from the copyright owner.       */
/**************************************************************/


/*
	color.c	- Module for color operations
*/



#include <math.h>
#include <ctype.h>
#include "c_types.h"
#include "color.h"
#include "fileio.h"
#include "buffer.h"
#include "defs.h"



/**************************************************************/

/* structure and type definitions */

typedef struct {double r, g, b;}   ColorRGB;
typedef struct {double x, y, z;}   ColorXYZ;

typedef struct {
		int MaxWL;
		int MinWL;
		int size;
		} ColorStruct;

#define DEFAULT_COLOR_VALUE 0.5

#define RED    0
#define GREEN  1
#define BLUE   2
#define WHITE  3

/*
	define the max and min scaling values for equal intensity
    clipping. These are defined to be slightly inside of the
    0 to 1 range to avoid numerical problems that occur when
	colors are on or very close to the clipping boundary.
*/
         
#define MAX_CLIP	0.9999  /* 1 - MAX_CLIP > 1 res step */
#define MIN_CLIP	0.0001  /* less than 1 res step */


/* Global variables for this file */

static ColorStruct color={	MAX_WAWE_LENGTH,
							MIN_WAWE_LENGTH,
							MAX_WAWE_LENGTH-MIN_WAWE_LENGTH+1};

static Logical init=FALSE;

static double  		XYZtoRGB[3][3];
static double  		RGBtoXYZ[3][3];
static ColorType 	X_tristim = NULL;
static ColorType 	Y_tristim = NULL;
static ColorType 	Z_tristim = NULL;
static ColorType 	tmp_curve = NULL;
static double  		XYZscale = 1.0;
static ColorXYZ 	RGBprimary[4];

/*
	This is the NTSC primaries with D6500 white point for use as
	the default initialization as given in sect 5.1.1 Color
	Correction for Display.
*/

static ColorXYZ RGB_NTSC[4] = {{0.670, 0.330, 0.000},   /* red */
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


/* Internal functions */

static ColorXYZ SpectToXYZ(ColorType);
static ColorRGB SpectToRGB(ColorType);
static Logical  MultSpect(ColorType,ColorType,ColorType);
static double   SpectArea(ColorType);
static Logical  CSpaceToXYZ (ColorXYZ *,double [3][3]);
static Logical  TInverse (double [3][3],double [3][3]);
static ColorRGB ClipRGB(ColorRGB in_rgb);


/**************************************************************/



/**************************************************************

	Logical ColorInit(void)

	Builds the transformation from a set of primaries to the CIEXYZ
	color space. This is the basis for the generation of the color
	transformations in the color routine set. Returns FALSE if there
	is a singularity.

*/

Logical ColorInit(void)
{
	int clr, ct;
	double len;

	if (init==TRUE)
		return FALSE;
	init = TRUE;

	/*
		load primaries and build transformations, use the
		defaults is rgb==NULL
	*/

	for (clr=RED; clr<=WHITE; clr++)
		{
		RGBprimary[clr] = RGB_NTSC[clr];
		RGBprimary[clr].z=1.0-RGBprimary[clr].x-RGBprimary[clr].y;
		}
	if (CSpaceToXYZ(RGBprimary, RGBtoXYZ)==FALSE)
		goto error;
	if (TInverse(RGBtoXYZ, XYZtoRGB)==FALSE)
		goto error;

	/*
		build the XYZ sampling curves - allocate space for the
		curves and interpolate the CIEXYZ table into continuous
		curves at 1 nm increments.
	*/

	{
		int ii, nm=color.MinWL;
		double *x, *y, *z;
		double x_cur, y_cur, z_cur;
		double x_inc, y_inc, z_inc;

		if((tmp_curve=ColorVectorInit())==NULL)
			goto error;
		if ((x = X_tristim = ColorVectorInit()) == NULL)
			goto error;
		if ((y = Y_tristim = ColorVectorInit()) == NULL)
			goto error;
		if ((z = Z_tristim = ColorVectorInit()) == NULL)
			goto error;
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
				if (nm > color.MaxWL)
					goto XYZ_done;
				if (nm >= color.MinWL)
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
		if(nm <= color.MaxWL)
			{
			*x++ = x_cur;
			*y++ = y_cur;
			*z++ = z_cur;
			nm++;
			}
		for ( ;nm<=color.MaxWL; nm++)
			*x++ = *y++ = *z++ = 0.0;
		}

XYZ_done:

	/*
		determine the scaling factor to be used in sampling spectral
		curves to bring Y os a sampled identity curve to 1.
	*/

	XYZscale = 1.0 / SpectArea(Y_tristim);
	return TRUE;

error:
	ColorExit();
	return FALSE;
}



/**************************************************************

	void ColorExit(void)

	Exits using color module and frees allocated memories

*/

void ColorExit(void)
{
	if (init==FALSE)
		return;
	ColorVectorExit(X_tristim);
	ColorVectorExit(Y_tristim);
	ColorVectorExit(Z_tristim);
	ColorVectorExit(tmp_curve);
	X_tristim = Y_tristim = Z_tristim = tmp_curve = NULL;
	init = FALSE;
	return;
}



/**************************************************************

	ColorType ColorVectorInit(void)

	Allocates memory and initializes a color spectrum vector

*/

ColorType ColorVectorInit(void)
{
	ColorType buf;
	int i;

	buf=NULL;
	buf=MemoryAllocate(BasicColorType,color.size);
	if(buf==NULL)
		return NULL;
	for(i=0;i<=color.size;i++)
		buf[i]=DEFAULT_COLOR_VALUE;
	return buf;

}



/**************************************************************

	void ColorVectorExit(ColorType buf)

	Frees the memory used by color vector given in argument

*/

void ColorVectorExit(ColorType buf)
{
	if(buf==NULL)
		return;
	MemoryFree(buf);
	return;
}



/**************************************************************

	Logical ColorReadVector(cBuffer buf,ColorType vector,
							String name,int bSize)

	This function reads color vector from file and
	interpolates it.

*/

Logical ColorReadVector(cBuffer buf,ColorType vector,
						String name,int bSize)
{
	int 	index,i=0;
	int 	CurL,PrevL,WaweL;
	double 	WaweV,PrevV=0.0;
	double 	k=0.0;

	if(init==FALSE)
		return FALSE;
	FileIOReadLine(name,buf,bSize);
	CurL=PrevL=color.MinWL;
	vector[0]=PrevV;
	while (isdigit(buf[0]))
		{
		index=0;
		if((index=BufferReadInt(buf,&WaweL,index))<=0)
			return FALSE;
		if((index=BufferReadDouble(buf,&WaweV,index))<=0)
			return FALSE;


		/* Interpolation of values */

		if(WaweL > PrevL && WaweL <= color.MaxWL)
			{
			CurL++;
			i++;
			if(CurL<WaweL)
				{
				k=(double)(WaweV-PrevV)/((double)(WaweL-PrevL));
				while(CurL<WaweL)
					{
					vector[i]=k*(CurL-PrevL)+vector[i-1];
					PrevL=CurL;
					CurL++;
					i++;
					}
				}
			PrevL=CurL;
			PrevV=WaweV;
			vector[i]=WaweV;
			}
		if(WaweL==PrevL && PrevL==color.MinWL)
			vector[0]=PrevV=WaweV;

		FileIOReadLine(name,buf,bSize);
		}
	if(CurL<color.MaxWL)
		for(;i<color.size;i++)
			vector[i]=0.0;
	return TRUE;

}



/**************************************************************

	int ColorGetSize(void)

	Returns the size of color vector.

*/

int ColorGetSize(void)
{
	return color.size;
}



/*****************************************************************

	RGBType *ColorGetRGB (ColorType spectral)

	Returns the sample values if successful, and NULL
	if not successfull.The curve is first sampled to XYZ
	then transformed to RGB.

*/

RGBType *ColorGetRGB (ColorType spectral)
{
	ColorRGB tmp;
	static RGBType rgb;

	if(spectral==NULL || init==FALSE)
		return NULL;
	tmp=SpectToRGB(spectral);
    tmp=ClipRGB(tmp);
	rgb.r=(int)(tmp.r*255);
	rgb.g=(int)(tmp.g*255);
	rgb.b=(int)(tmp.b*255);
	return &rgb;
}




/**************************************************************
	Internal functions for this file
***************************************************************/



/*****************************************************************

	Logical MultSpect(ColorType c1,ColorType c2,ColorType c3)

    Multiply spectral curve 'c1' by spectral curve 'c2' to get
    spectral curve 'c3'. Returns TRUE if successful, FALSE if
    the color routines are not initialized.

*/
         
Logical MultSpect(ColorType c1,ColorType c2,ColorType c3)
{
	int i;

	for(i=color.size; --i>=0; )
		*c3++ = *c1++ * *c2++;
    return TRUE;     
}



/*****************************************************************

	ColorXYZ SpectToXYZ(ColorType spectral)

    Returns the sample values in tristimulus coordinates.
         
    Multiplies the spectral curve by each of the sampling curves
    then integrates the resulting curves. The XYZ values are then
   normalized such that an identity material has Y=1.

*/
         
ColorXYZ SpectToXYZ(ColorType spectral)
{
	ColorXYZ xyz;

	(void)MultSpect(spectral, X_tristim, tmp_curve);
	xyz.x = XYZscale * SpectArea(tmp_curve);
	(void)MultSpect(spectral, Y_tristim, tmp_curve);
	xyz.y = XYZscale * SpectArea(tmp_curve);
    (void)MultSpect(spectral, Z_tristim, tmp_curve);
	xyz.z = XYZscale * SpectArea(tmp_curve);
    return xyz;
}



/*****************************************************************

	double SpectArea (ColorType c1)

    Returns the area under the spectral curve 'c1'. 

*/
         
double SpectArea (ColorType c1)
{
	int ct;
	double area = 0.0;

	for (ct=color.size; --ct>=0; )
		area += *c1++;
	return area;
}



/*****************************************************************
         
	ColorRGB SpectToRGB (ColorType spectral)

	Returns the sample values in RGB. The curve is first sampled
	to XYZ then transformed to RGB

*/
         
ColorRGB SpectToRGB (ColorType spectral)
{
	ColorXYZ xyz;
    ColorRGB rgb;

	xyz = SpectToXYZ (spectral);
	rgb.r = (XYZtoRGB[0][0] * xyz.x)+(XYZtoRGB[0][1] * xyz.y)
	                                +(XYZtoRGB[0][2] * xyz.z);
	rgb.g = (XYZtoRGB[1][0] * xyz.x)+(XYZtoRGB[1][1] * xyz.y)
									+(XYZtoRGB[1][2] * xyz.z);
	rgb.b = (XYZtoRGB[2][0] * xyz.x)+(XYZtoRGB[2][1] * xyz.y)
									+(XYZtoRGB[2][2] * xyz.z);
	return rgb;
}



/*****************************************************************
         
	static Logical CSpaceToXYZ (ColorXYZ *cspace,double **t_mat)
         
	Builds the transformation from a set of primaries to the CIEXYZ
    color space. This is the basis for the generation of the color
	transformations in the color routine set. Returns FALSE if there
	is a singularity.

*/

static Logical CSpaceToXYZ (ColorXYZ *cspace,double t_mat[3][3])
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

	static Logical TInverse (double **mat,double **inv_mat)

	Inverts mat using Gaussian elimination. Returns TRUE if
	successful and FALSE if there is a singularity.

*/

static Logical TInverse (double mat[3][3],double inv_mat[3][3])
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

	static ColorRGB ClipRGB(ColorRGB in_rgb)

    Returns the clipped color - clipping is by desaturating the
    color by shifting it towards the neutral axis in a plane
    perpendicular to the neutral axis. The neutral axis is taken
    as the vector from the monitor black to the monitor white.

*/
         
static ColorRGB ClipRGB(ColorRGB in_rgb)
{
	ColorRGB diff, out_rgb;
    double  axis_rgb, diff_mult, tmp_mult;
         
	/*
		check to see if clipping is required
    */
         
	if ((in_rgb.r < 0.0) || (in_rgb.r > 1.0) ||
		   (in_rgb.g < 0.0) || (in_rgb.g > 1.0) ||
		   (in_rgb.b < 0.0) || (in_rgb.b > 1.0))
		{
	/*
		clipping is required, determine the distance from
        the origin to the equal intensity plane containing
        the color. The distance is normalized the origin
        at color (0,0,0) and a distance of 1 at (1,1,1)
	*/
         
    	axis_rgb = (in_rgb.r + in_rgb.g + in_rgb.b) * 0.333333;

	/*
		check for the intensity plane of the color being
        outside the displayable range -- if it is, set
        color to either black or white.
    */
         
		if (axis_rgb <= MIN_CLIP)

		/*
			this is not a visible color -- it should not
        	have been computed
    	*/
         
    		out_rgb.r = out_rgb.g = out_rgb.b = 0.0;
    	else if (axis_rgb >= MAX_CLIP)

		/*
			This is way beyond white in intensity, set it
			to the white point.
        */
			 out_rgb.r = out_rgb.g = out_rgb.b = 1.0;
		else
			{

		/*
			the intensity plane is within the displayable
         	range. Compute the vector from the neutral
         	axis to the color on it's intensity plane.
         	The intersection of the neutral axis and the
         	intensity plane is at r=g=b=axis_rgb.
        */
         
         	diff.r = in_rgb.r - axis_rgb;
         	diff.g = in_rgb.g - axis_rgb;
         	diff.b = in_rgb.b - axis_rgb;

		/*
			determine the relative length of the vector
         	to the edge of the displayable color gamut.
        */
         
         	diff_mult = 1.0;
			if (in_rgb.r > 1.0)
				{
				if((tmp_mult=(MAX_CLIP-axis_rgb)/diff.r)<diff_mult)
					diff_mult = tmp_mult;
				}
			else if (in_rgb.r < 0.0)
				{
				if((tmp_mult=(MIN_CLIP-axis_rgb)/diff.r)<diff_mult)
					diff_mult = tmp_mult;
				}
         
			if (in_rgb.g > 1.0)
				{
				if((tmp_mult=(MAX_CLIP-axis_rgb)/diff.g)<diff_mult)
					diff_mult = tmp_mult;
                }
			else if (in_rgb.g < 0.0)
				{
				if ((tmp_mult=(MIN_CLIP-axis_rgb)/diff.g)<diff_mult)
					diff_mult = tmp_mult;
		 		}
			if (in_rgb.b > 1.0)
				{
				if((tmp_mult=(MAX_CLIP-axis_rgb)/diff.b)<diff_mult)
					diff_mult = tmp_mult;
		 		}
			else if (in_rgb.b < 0.0)
				{
				if((tmp_mult=(MIN_CLIP-axis_rgb)/diff.b)<diff_mult)
					diff_mult = tmp_mult;
				}
         
		/*
		 	determine the location of the color at the
            edge of the displayable color gamut.
        */
         
         	out_rgb.r = axis_rgb + (diff.r * diff_mult);
         	out_rgb.g = axis_rgb + (diff.g * diff_mult);
         	out_rgb.b = axis_rgb + (diff.b * diff_mult);
            }
		return out_rgb;
        }        
	else
		return in_rgb;
}
         




