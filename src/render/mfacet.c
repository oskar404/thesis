/********************************************************************/
/*                      mfacet.c                                    */
/********************************************************************/
         
/*
	MODULE PURPOSE:
    	This module contains micro facet distribution routines and
        various support routines. One of support functions includes
        computing the coefficients corresponding to beta for each
        function. Beta is the angle between H and N where the
        function is equal to half the value at N = H. Beta is in
        radians.
         
    MODULE CONTENTS:
		MFacetPhongInit     - compute Ns given beta
		MFacetPhong 		- Phong cosine power function
		MFacetBlinnInit     - compute Ns given beta
		MFacetBlinn         - Blinn cosine power function
		MFacetGaussianInit 	- compute C1 given beta
		MFacetGaussian  	- Gaussian distribution
		MFacetReitzInit		- compute C2 given beta
		MFacetReitz         - Trowbridge-Reitz
		MFacetCookInit      - compute m given beta
		MFacetCook          - Cook-model
		MFacetG	 			- compute apparent roughness
		MFacetTau   		- compute correlation distance
		MFacetSigma 		- compute ring roughness
		MFacetM	 			- compute slope
		MFacetCoherent  	- coherent roughness attenuation
                     		(per Beckmann)
		MFacetIncoherent   	- incoherent microfacet attenuation
                      		(per Beckmann and Bahar)
		MFacetVxy   		- Vxy used in MFacetIncoherent()
         
	ASSUMPTIONS:
        The following are defined in math.h
        	HUGE    largest floating point value
         	M_PI    pi
         	M_PI_2  pi/2
         	M_PI_4  pi/4
         	M_SQRT2 root of 2        
		The direction vectors N, E, and L are assumed to be oriented
        to the same side of the surface.
*/         



#include <stdio.h>
#include <math.h>
#include "vector.h"
#include "mfacet.h"



#ifndef HUGE
#define HUGE 999999999999999999 
#endif /* HUGE */


#ifndef M_PI
#define M_PI 3.141592654
#endif /* M_PI */


#ifndef M_PI_2
#define M_PI_2 1.570796327
#endif /* M_PI_2 */


#ifndef M_PI_4
#define M_PI_4 0.7853981634
#endif /* M_PI_4 */


#ifndef M_SQRT2
#define M_SQRT2 1.772453851
#endif /* M_SQRT2 */



/****************************************************************
         
         double MFacetPhongInit(double beta)
		 beta (in) - angle between N and H where function = .5

		 Returns Ns specular exponent for given beta
		 for the Phong (1975) specular function.

*/
         
double MFacetPhongInit(double beta)
{
	if (beta <= 0.0)
		return HUGE;
	if (beta >= M_PI_4)
		return 0.0;
	return -(log(2.0)/log(cos(2.0*beta)));
}
         

         
/*****************************************************************

	double MFacetPhong (VECTOR *N,VECTOR *L,VECTOR *E,double Ns)
	N, L, E (in) 	- N, L, E vectors
    Ns (in) 		- specular exponent
         
    Returns the value of the Phong (1975) specular function given
    surface normal, Incident light direction, view direction, and
    the specular exponent. 

*/
         
double MFacetPhong (VECTOR *N,VECTOR *L,VECTOR *E,double Ns)
{
	double      Re_dot_L;

	if ((Re_dot_L=VectorDot(VectorReflected(E,N),L))<0.0)
		return 0.0;
    return pow(Re_dot_L,Ns);       
}



/*****************************************************************

    double MFacetBlinnInit(double beta)
	beta (in) - angle between N and H where function = .5

	Returns Ns specular exponent given beta for the Cosine power
	distribution function presented by Blinn (1977).

*/
         
double MFacetBlinnInit(double beta)
{
	if (beta <= 0.0)
		return HUGE;
	if (beta >= M_PI_2)
		return 0.0;
    return -(log(2.0)/log(cos(beta)));
}         



/*****************************************************************

	double MFacetBlinn (VECTOR *N,VECTOR *L,VECTOR *E,double Ns)
	N, L, E (in) 	- N, L, E vectors
	Ns (in) 		- specular exponent
         
    Returns the cosine power distribution function presented by
    Blinn (1977) given surface normal, incident light direction,
    view direction, and the specular exponent. 

*/
         
double MFacetBlinn (VECTOR *N,VECTOR *L,VECTOR *E,double Ns)
{
	double N_dot_H;

	if ((N_dot_H=VectorDot(N,VectorH(E,L))) < 0.0)
		return 0.0;
    return pow (N_dot_H, Ns);     
}



/*****************************************************************

	double MFacetGaussianInit(double beta)
	beta (in) - angle between N and H where function = .5

    Returns C1 given beta for the Gaussian distribution presented
    by Blinn (1977). 

*/
         
double MFacetGaussianInit(double beta)
{
	if(beta <= 0.0)
		return HUGE;
    return sqrt(log(2.0))/beta;
}



/*****************************************************************

	double MFacetGaussian (VECTOR *N,VECTOR *L,VECTOR *E,double C1)
	N, L, E (in) 	- N, L, E vectors
	C1 (in) 		- shape coefficient
         
	Returns the Gaussian distribution function presented by
    Blinn (1977) given surface normal, incident light direction,
	view direction, and the specular exponent. 

*/
         
double MFacetGaussian (VECTOR *N,VECTOR *L,VECTOR *E,double C1)
{
	double      tmp;
	double      N_dot_H;

	if ((N_dot_H = VectorDot (N, VectorH(E,L))) < 0.0)
		return 0.0;
	tmp = acos(N_dot_H)*C1;
	return exp(-(tmp*tmp));
}



/*****************************************************************

	double MFacetReitzInit(double beta)
	beta (in) - angle between N and H where function = .5

    Returns C2 squared given beta for the Trowbridge-Reitz
	distribution function presented by Blinn (1977). C2 is
	squared for computational efficiency later.

*/

double MFacetReitzInit(double beta)
{
	double      cos_beta;

	if (beta <= 0.0)
		return 0.0;
    cos_beta = cos(beta);
    return ((cos_beta*cos_beta)-1.0)/((cos_beta*cos_beta)-M_SQRT2);        
}



/*****************************************************************

	double MFacetReitz (VECTOR *N,VECTOR *L,VECTOR *E,double C2_2)
	N, L, E (in) 	- N, L, E vectors
	C2_2 (in)		- C2 squared
         
	Returns the Trowbridge-Reitz distribution function presented
    by Blinn (1977) given surface normal, incident light
	direction, view direction, and the specular exponent. 

*/

double MFacetReitz (VECTOR *N,VECTOR *L,VECTOR *E,double C2_2)
{
	double      tmp;
	double      N_dot_H;

	if ((N_dot_H = VectorDot(N,VectorH(E,L))) < 0.0)
		return 0.0;
    tmp = C2_2/((N_dot_H*N_dot_H*(C2_2-1.0))+1.0);
    return tmp * tmp; 
}



/*****************************************************************

	double MFacetCookInit(double beta)
	beta (in) - angle between N and H where function = .5

	Returns m squared corresponding to beta.
	m is squared for computational efficiency
	in later use.
	
*/

double MFacetCookInit(double beta)
{
	double      tan_beta;

	if (beta <= 0.0)
		return 0.0;
	if (beta >= M_PI_2)
		return HUGE;
    tan_beta = tan(beta);
    return -(tan_beta*tan_beta)/log(pow(cos(beta),4.0)/2.0);
}         



/*****************************************************************

	double MFacetCook (VECTOR *N,VECTOR *L,VECTOR *E,double m2)
	N, L, E (in) 	- N, L, E vectors
	m2 (in)			- m squared
         
	Returns the microfacet distribution propability (cook 1982)
    derived from (Beckmann 1963). 

*/
        
double MFacetCook (VECTOR *N,VECTOR *L,VECTOR *E,double m2)
{
	double      tmp;
	double      N_dot_H;

	if((N_dot_H=VectorDot(N,VectorH(E,L)))<0.0)
		return 0.0;
    tmp = -(1.0-(N_dot_H*N_dot_H))/(N_dot_H*N_dot_H*m2);
    return exp(tmp)/(4.0*M_PI*m2*pow(N_dot_H,4.0));
}         



/*****************************************************************

	double MFacetG(VECTOR *N,VECTOR *L,VECTOR *E,double sigma,double lambda)
	N, L, E (in) 	- N, L, E vectors
    sigma (in) 		- RMS roughness, nm
    lambda (in) 	- wavelength, nm
         
	Returns the apparent roughness, g, as given by Beckmann (1963).
    
*/
         
double MFacetG(VECTOR *N,VECTOR *L,VECTOR *E,double sigma,double lambda)
{
	double      tmp;

    tmp = VectorDot(N,L)+VectorDot(N,E);
    return (4.0*M_PI*M_PI*sigma*sigma*tmp*tmp)/(lambda*lambda);
}



/*****************************************************************

	double MFacetTau(double sigma,double m)
	sigma (in) 	- rms roughness (nm)
    m (in) 		- rms slope
         
    Returns tau (correlation distance) in nm. The relationship of
    roughness, slope, and correlation distance given by
    Beckmann (1963) is assumed.

*/
         
double MFacetTau(double sigma,double m)
{
	return (2.0 * sigma) / m;
}



/*****************************************************************

	double MFacetSigma (double m,double tau)
	m (in) 		- rms slope
    tau (in) 	- correlation distance (nm)

    Returns sigma (rms roughness). The relationship of roughness,
    slope, and correlation distance given by Beckmann (1963) is
    assumed.

*/
         
double MFacetSigma (double m,double tau)
{
	return (m * tau)/ 2.0;
}



/*****************************************************************

	double MFacetM (double sigma,double tau)
	sigma (in) 	- rms roughness (nm)
    tau (in) 	- correlation distance (nm)

    Returns m (rms slope). The relationship of roughness, slope,
    and correlation distance given by Beckmann (1963) is assumed.

*/
         
double MFacetM (double sigma,double tau)
{
	return (2.0 * sigma) / tau;        
}



/*****************************************************************

	double MFacetCoherent(double g)
	g (in) apparent roughness
         
    Returns the coherent roughness attenuation given
    by Beckmann (1963).

*/
         
double MFacetCoherent(double g)
{
	return exp(-g);
}



/*****************************************************************

	double MFacetIncoherent (VECTOR *N,VECTOR *L,VECTOR *E,
					double m,double g,double lambda,double tau)
	N,L,E (in) N, L, E vectors
    m (in) m (slope)
    g (in) apparent roughness
    lambda (in) wavelength (rms)
    tau (in) correlation distance (rms)
         
    Returns the micro facet distribution function given by
    Beckmann (1963) as interpreted by Koestner (1986).

*/
         
double MFacetIncoherent (VECTOR *N,VECTOR *L,VECTOR *E,
					double m,double g,double lambda,double tau)
{
	VECTOR     *H;
    double     D1, D2;
    double     denom;
    double     N_dot_H, N_dot_H2;
         
	if (g <= 0.0)
		return 0.0;
	if ((H = VectorH(E,L)) == NULL)
		return 0.0;
	if ((N_dot_H = VectorDot(N,H))<=0.0)
		return 0.0;
    N_dot_H2 = N_dot_H * N_dot_H;
    denom = 4.0*M_PI*m*m*N_dot_H2*N_dot_H2;
         
    /* if g < 8.0, evaluate the series expansion,
    terminating when a term falls below 0.00001. For small g
    the evaluation terminates quickly. For large g the series
    converges slowly. If g is near 8.0 the first terms of the
    series will be less that the termination criteria. This
    requires an additional termination criteria that the values
    of the terms are decreasing at the time the termination
	criteria is reached.
	*/

	if(g<8.0)
		{
		double inc,ct,ct_factorial,g_pow;
		double last_inc,Vxz_t_over_4;

        Vxz_t_over_4 = (tau*tau*MFacetVxz(N,L,E,lambda))/4.0;
        D1 = 0.0;
        ct = 1.0;
        ct_factorial = 1.0;
        g_pow = g * g;
        inc = 0.0;
		do
        	{
         	last_inc = inc;
         	inc =(g_pow/(ct*ct_factorial))*exp(-(g+Vxz_t_over_4/ct));
         	D1 += inc;
         	ct += 1.0;
         	ct_factorial *= ct;
         	g_pow *= g;
			} while (((inc/denom)>0.00001)||(inc>last_inc));
         D1 /= denom;
         
	/* if g < 5.0, only the series expansion is used. If
    5.0 < g < 8.0, then we are in a transition range
    for interpolation between the series expansion and
	the convergence expression.
	*/

		if(g<5.0)
			return D1;
    	}
         
    /* if g > 5.0, evaluate the converger.ce expression
	(this routine would have returned earlier if G < 5.0).
    */
         
    {
    	double tmp;

    	tmp = (N_dot_H2-1.0)/(N_dot_H2*m*m);
    	D2 = exp(tmp)/denom;
         
    /* if g > 8.0, only the convergence expression is used,
    otherwise we are in a transition zone between the
    convergent expression and series expansiGn.
	*/

		if (g > 8.0)
			return D2;
	}
	      
    /* linear interpolation between Dl and D2 for
	5.0 < g < 8.0
    */
         
	return (((8.0-g)*D1)+((g-5.0)*D2))/3.0;
}



/*****************************************************************

	double MFacetVxz (VECTOR *N,VECTOR *L,VECTOR *E,double lambda)
	N, L, E (in) N, L, E vectors
    lambda      (in) wavelength (nm)

	Returns the value of Vxz.

*/         

double MFacetVxz (VECTOR *N,VECTOR *L,VECTOR *E,double lambda)
{
	double N_dot_L, N_dot_E, cos_phi;
	VECTOR sin_i, sin_r;
    double len_2i, len_2r; 
	     
	/* compute the sine squared of the incident angle
    */
         
    N_dot_L = VectorDot(N,L);
    len_2i = 1.0 - (N_dot_L*N_dot_L);
         
    /* compute the sine squared of the reflected angle
    */
         
    N_dot_E = VectorDot (N,E);
    len_2r = 1.0 - (N_dot_E * N_dot_E);

    /* if the sine of either the incident or the
    reflected angle is zero, then the middle
	term is zero.
    */
         
	if ((len_2i > 0.0) && (len_2r > 0.0))
		{
        /* the two sine vectors are of length equal to the sine of
        the angles. The dot product is the cosine times the
        lengths of the vectors (sines of the angles).
        */
         
        sin_i.i = L->i - (N_dot_L * N->i);
        sin_i.j = L->j - (N_dot_L * N->j);
        sin_i.k = L->k - (N_dot_L * N->k);
        sin_r.i = E->i - (N_dot_E * N->i);
        sin_r.j = E->j - (N_dot_E * N->j);
        sin_r.k = E->k - (N_dot_E * N->k);
		cos_phi = VectorDot(&sin_i,&sin_r);
        return (4.0*M_PI*M_PI*(len_2i+(2.0*cos_phi)+len_2r))/(lambda*lambda);        
        }
	else
		{ 
    	return (4.0*M_PI*M_PI*(len_2i + len_2r))/(lambda*lambda);
		}
}


         