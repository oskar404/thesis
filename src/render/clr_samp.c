/*****************************************************************/
/*                       clr_samp.c                              */
/*****************************************************************/

/*         
	MODULE PURPOSE:
    	This module contains the routines for spectral sampling.
        The operations include defining the sampling space,
		sampling spectral curves, and transformations from spectral
        sample space to RGB or XYZ.
         
	MODULE CONTENTS:
		ClrInitSamples 		- initialize spectral sampling
		ClrNumSamples 		- returns the number of samples
		ClrSpectToSample 	- sample a spectral curve
		ClrGetSampleRGB 	- get the sample to RGB matrix
		ClrGetSampleXYZ 	- get the sample to XYZ matrix
		ClrReconstruct 		- reconstruct a spectral curve
		ClrExitSamples     	- finish with spectral sampling
         
	NOTES:
		> The CLR routines must be initiallzed (ClrInit()) before
          using any of the sampling routines.
		> When the CLR_SAMPLE_HALL method is selected, sampling
          uses abutting, non-overlapping, box sample and
		  reconstruction functions are described in Hall (1983).
          This provides continuous sampling between the low bound
          of the first sample and the high bound of the last
          sample. For applications requiring discrete isolated
          samples, user modification of these routines
          is required.
		> When the CLR_SAMPLE_MEYER method is used, 4 samples are
          used as described in Meyer (1988).

*/
         


#include <stdio.h>
#include <stdlib.h>
#include "clr.h"
         


static int  sample_type = -1;
static int  samples = 0;
static int *bounds = NULL;
static double XYZ_to_ACC[3][3] = {{-0.0177,    1.0090, 0.0073},
                                  {-1.5370,    1.0821, 0.3209},
                                  { 0.1946,   -0.2045, 0.5264}};
static double ACC_to_XYZ[3][3];
static double samp_to_ACC[3][4] =
                             {{0.00000, 0.18892, 0.67493,  0.19253},
                              {0.00000, 0.00000, 0.31824, -0.46008},
                              {0.54640, 0.00000, 0.00000,  0.00000}};





/*****************************************************************

	int ClrInitSamples (int method,int num_samples,int *sample_bounds)
		method (in) - sampling method:
                      CLR_SAMPLE_MEYER Meyer (1988)
                      CLR_SAMPLE_HALL Hall (1983)
        num_samples (in) - number of sample functions
        sample_bounds (in) - boundaries of the sampling
                             functions. There must be
                             num_samples+1 bounds arranged in
						     ascending order in this array.
         
	For the CLR_SAMPLE_HALL method the bound wavelength is included
    in the sampling function. For example, using 3 samples with
    bounds at (411, 491, 571, 651), the actual samples are 411-490,
	491-570, and 571-650.
         
    The CLR_SAMPLE_MEYER method uses a prescribed sampling with 4
    samples. The num_samples and sample_bounds arguments are
    ignored.
         
    Returns TRUE if successful, FALSE if sample bounds are not valid
    or sampling is previously initialized to some other value.

*/
         
int ClrInitSamples (int method,int num_samples,int *sample_bounds)
{
	int ct;
         
	if (method == CLR_SAMPLE_MEYER)
		{
        samples = 4;
        sample_type = method;
		ClrTInverse (XYZ_to_ACC, ACC_to_XYZ);
        } 
	else if (method == CLR_SAMPLE_HALL)
		{

	/*
		There are two ways to do this, one is to save a
		complete sampling curve for each sample. The
		other is to do the computation on the fly. Here
		it's done on the fly. Only the bounds are saved.
	*/

		if (num_samples <= 0)
			return FALSE;
		if ((bounds = (int *)malloc((unsigned)(sizeof(int) *
									(num_samples + 1)))) == NULL)
			goto error;
		bounds[0] = sample_bounds[0];
		for (ct=0; ct<num_samples; ct++)
			{
			if (sample_bounds[ct+1] <= sample_bounds[ct])
				goto error;
			bounds[ct+1] = sample_bounds[ct+1];
            }
        samples = num_samples;
        sample_type = method;
        }
	else
		{
        goto error;
        }
    return TRUE;
         
error:

	samples = 0;
	if (bounds != NULL)
		free((char *)bounds);
    return FALSE;        
}
         


/*****************************************************************

	int ClrNumSamples()
         
    Returns the number of samples for which sampling is initialized.   
	Returns 0 if sampling is not initialized.

*/
         
int ClrNumSamples()
{
	return samples;
}


         
/*****************************************************************

	int ClrSpectToSample (double *spectral,double *sample)
		spectral (in) 	- spectral curve to be sampled
		sample (mod) 	- array to receive the sampled values.

	Samples 'spectral' and loads the sample values into 'sample'.
    Returns TRUE if successful and FALSE if CLR or the sampling
    has not been initialized

*/
         
int ClrSpectToSample (double *spectral,double *sample)
{
	int curWL, ct, maxWL, cur_samp;
    double cur_sum;
         
	if (samples <= 0)
		return FALSE;
	if ((curWL = ClrGetMinWL()) < 0)
		return FALSE;
	maxWL = ClrGetMaxWL();
         
	if (sample_type == CLR_SAMPLE_MEYER)
		{

	/*
		sample at 456.4nm, 490.9nm, 557.7nm, and 631.4nm
    */
         
		if ((curWL > 456) || (maxWL < 457))
			*sample++ = 0.0;
		else
			*sample++ = spectral[456-curWL] + (0.4
			          * (spectral[457-curWL] - spectral[456-curWL]));
		if ((curWL > 490) || (maxWL < 491))
			*sample++ = 0.0;
		else *sample++ = spectral[490-curWL] + (0.9
		               *(spectral[491-curWL] - spectral[490-curWL]));
		if ((curWL > 557) || (maxWL < 558))
			*sample++ = 0.0;
		else
			*sample++ = spectral[557-curWL] + (0.7
					  *(spectral[558-curWL] - spectral[557-curWL]));
		if ((curWL > 631) || (maxWL < 632))
			*sample++ = 0.0;
		else
			*sample++ = spectral[631-curWL] + (0.4
					  *(spectral[632-curWL] - spectral[631-curWL]));
        } 
	else
		{ 
		for ( ;curWL<bounds[0]; curWL++, spectral++) ;
		for (cur_samp=1,cur_sum=0.0,ct=0;curWL<=maxWL;curWL++)
			{
			while (curWL>=bounds[cur_samp])
				{
				if (ct == 0)
					*sample++ = 0.0;
				else
					*sample++ = cur_sum / ct;
				if (++cur_samp > samples)
					return TRUE;
         		cur_sum = 0.0;
         		ct = 0;
				}
		 	cur_sum += *spectral++;
         	ct++;
            }
        *sample = cur_sum / ct;
        } 
    return TRUE;
}


     
/*****************************************************************

	int ClrGetSampleRGB (double *matrix)
		matrix   (mod) - matrix to be filled.
         
    Returns the matrix for conversion from the sampled space to the
	RGB the CLR routines have been initialized for. The matrix
    is a 3 x num samples matrix.

*/         

int ClrGetSampleRGB (double *matrix)
{
	double    *xyz = NULL;
    double    xyz_rgb[3][3];
    int       ct;

    /* get the XYZ matrix, then transform it into RGB */
         
    if ((xyz = (double *)malloc((unsigned) (3 *
		                         sizeof(double)*samples)))==NULL)
		goto error;
	if(!ClrGetSampleXYZ(xyz))
		goto error;
	if (!ClrGetXYZ_RGB(xyz_rgb))
		goto error;
	for (ct=0; ct<samples; ct++, matrix++, xyz++)
		{
        matrix[0] = (xyz_rgb[0][0] * xyz[0]) +
                    (xyz_rgb[0][1] * xyz[samples]) +
                    (xyz_rgb[0][2] * xyz[2*samples]);
        matrix[samples] = (xyz_rgb [1][0] * xyz[0]) +
                          (xyz_rgb[1][1] * xyz[samples]) +
         				  (xyz_rgb[1][2] * xyz[2*samples]);
        matrix[2*samples] = (xyz_rgb[2][0] * xyz[0]) +
         				    (xyz_rgb[2][1] * xyz[samples]) +
         				    (xyz_rgb[2][2] * xyz[2*samples]);
        } 
	free((char *)xyz);
    return TRUE;
         
error:

	if (xyz != NULL)
		free((char *)xyz);
    return FALSE;        
}
         


/*****************************************************************
         
	int ClrGetSampleXYZ (double *matrix)
		matrix    (mod) - matrix to be filled.
         
	Returns the matrix for conversion from the sampled space to
    CIEXYZ. The matrix is a 3 x num samples matrix.
        
*/

int ClrGetSampleXYZ (double *matrix)
{
	int   minWL, maxWL, curWL, cur_samp;
    double *reconst, *cur_r, fill, *samp_mat;
    CLR_XYZ xyz;
         
	if (samples <= 0)
		return FALSE;
	if (sample_type == CLR_SAMPLE_MEYER)
		{

	/*
		concatenate the sample to ACC matrix with the ACC_to_XYZ
        matrix. The divide by 1.057863 is a normalization so
        than an identity curve has a Y value of 1.0 following
		the conventions used in the CLR_routines.
    */
         
    	samp_mat = samp_to_ACC[0];
		for (cur_samp=0;cur_samp<samples;cur_samp++,matrix++,samp_mat++)
			{
         	matrix[0] = ((ACC_to_XYZ[0][0] * samp_mat[0]) +
         				 (ACC_to_XYZ[0][1] * samp_mat[samples]) +
         				 (ACC_to_XYZ[0][2] * samp_mat[2*samples]))
                          / 1.057863;
         	matrix[samples] = ((ACC_to_XYZ[1][0] * samp_mat [0]) +
         				 (ACC_to_XYZ[1][1] * samp_mat[samples]) +
         				 (ACC_to_XYZ[1][2] * samp_mat[2*samples])) 
                          / 1.057863;
         	matrix[2*samples] = ((ACC_to_XYZ[2][0] * samp_mat[0]) +
         				 (ACC_to_XYZ[2][1] * samp_mat[samples]) +
         				 (ACC_to_XYZ[2][2] * samp_mat[2*samples]))
         				 / 1.057863;
		 	}
        }
	else
		{
		minWL = ClrGetMinWL();
		maxWL = ClrGetMaxWL();

	/*
		allocate space for the reconstruction function
    */
         
		if ((reconst = (double *)malloc((unsigned)(sizeof(double)
							   *(maxWL-minWL+1)))) == NULL)
			goto error;
         
	/*
		Build each reconstruction function and sample it into xyz.
        Load the values into the matrix
	*/
	     
		for(cur_samp=0;cur_samp<samples; cur_samp++, matrix++)
			{
         	cur_r = reconst;        
			if (cur_samp == 0)
				fill = 1.0;
			else
				fill = 0.0;
		 	for (curWL=minWL; (curWL<bounds[cur_samp]) &&
					                        (curWL<maxWL); curWL++)
				*cur_r++ = fill;
		 	for ( ; (curWL<bounds[cur_samp+1]) &&
											(curWL<=maxWL); curWL++)
				*cur_r++ = 1.0;
			if (cur_samp == (samples-1))
				fill = 1.0;
			else
				fill = 0.0;
			for ( ;curWL<=maxWL; curWL++)
				*cur_r++ = fill;
         	xyz = ClrSpectToXYZ(reconst);
         	matrix[0] = xyz.x;
         	matrix[samples] = xyz.y;
         	matrix[2*samples] = xyz.z;
		 	}
        }
	free ((char *)reconst);
    return TRUE;
         
error:

	if (reconst != NULL)
		free((char *)reconst);
    return FALSE;
}


        
/*****************************************************************

	int ClrReconstruct (double *sample,double *spectral)
		sample (in) - the sample values
        spectral (mod) - the reconstructed spectral curve
         
    Reconstructs a spectral curve from the sample values. The
    reconstruction functions are box functions resulting a step
    function as the reconstructed curve.

*/
         
int ClrReconstruct (double *sample,double *spectral)
{
	int curWL, maxWL, cur_sample;

	if (samples <= 0)
		return FALSE;
	if (sample_type == CLR_SAMPLE_MEYER)
		return FALSE;
	else if (sample_type == CLR_SAMPLE_HALL)
    	{
		curWL = ClrGetMinWL();
        maxWL = ClrGetMaxWL();
        cur_sample = 1;
		while (curWL<=maxWL)
			{
         	if (curWL < bounds[cur_sample])
         		*spectral++ = *sample;
         	else if (cur_sample >= samples)
         		*spectral++ = *sample;
			else
				{
         		cur_sample++;
         		sample++;
         		*spectral++ = *sample;
				}
            curWL++;
         	}
        }
    return TRUE;
}
         


/*****************************************************************

	int ClrExitSamples()

	Complete use of spectral sampling, free any allocated space.

*/
         
int ClrExitSamples()
{
 	sample_type = -1;
    samples = 0;
	if (bounds != NULL)
		{
        free((char *)bounds);
        bounds = NULL;
        } 
	return TRUE;
}         
