/*****************************************************************/
/*                           geom.c                              */
/*****************************************************************/

/*         
	MODULE PURPOSE:
    	This module contains geometric attenuation
    	functions
         
    MODULE CONTENTS:
        GeomTorrance      - function by Torrance and Sparrow
        GeomSancer        - function by Sancer
*/

         
#include <math.h>
#include "vector.h"
         
#define ROOT_PI     1.7724538509055159



/*****************************************************************

	double GeomTorrance (VECTOR *N,VECTOR *L,VECTOR *E,double dummy)
	N,L,E (in) - N, L, E vectors
    dummy (in) - just to make the calls identical
       
    Returns geometric attenuation (Torrance and Sparrow
	1967). The direction vectors N, V, and L are assumed to be
	oriented to the same side of the surface.

*/
         
double GeomTorrance (VECTOR *N,VECTOR *L,VECTOR *E,double dummy)
{
	double N_dot_H, E_dot_H;
	double N_dot_E, N_dot_L;
	double g1, g2;
	VECTOR *H;

    H = VectorH(E,L);
	N_dot_H = VectorDot(N,H);
	E_dot_H = VectorDot(E,H);
	N_dot_E = VectorDot(N,E);
	N_dot_L = VectorDot(N,L);

	if((g1=(2.0*N_dot_H*N_dot_E)/E_dot_H)>1.0)
		g1 = 1.0;
	if((g2=(2.0*N_dot_H*N_dot_L)/E_dot_H)<g1)
		return g2;
	else
		return g1;
}


        
double erfc(double); /* Error function of x used by GeomSancer() */



/*****************************************************************

	double GeomSancer (VECTOR *N,VECTOR *L,VECTOR *E,double m2)
	N,L,E (in) - N, L, E vectors
    m2 (in) - m squared
         
	Returns geometric attenuation (Sancer 1969). The direction
	vectors N, V, and E are assumed to be oriented to the same
	side of the surface.
         
    NOTE:
		> m can be related to beta using DCookInit()
        > This implementation assumes slope relates roughness and
          correlation distance as described by Beckmann. This
          explains the missing factor of 2 with m2.

*/
         
double GeomSancer (VECTOR *N,VECTOR *L,VECTOR *E,double m2)
{
	double      N_dot_L, N_dot_E;
    double      c1, c2, root_c1, root_c2, ci, cr;
       
	if (m2 <= 0.0)
		return 1.0;
	if ((N_dot_L = VectorDot(N,L)) <= 0.0)
		return 0.0;
	c1 = (N_dot_L*N_dot_L)/(m2*(1.0-(N_dot_L*N_dot_L)));
    root_c1 = sqrt(c1);
	ci=(exp(-c1)/(2.0*ROOT_PI*root_c1))-(erfc(root_c1)/2.0);
	if ((N_dot_E = VectorDot(N,E)) <= 0.0)
		return 0.0;
	c2 =(N_dot_E*N_dot_E)/(m2*(1.0-(N_dot_E*N_dot_E)));
    root_c2 = sqrt(c2);
    cr=(exp(-c2)/(2.0*ROOT_PI*root_c2))-(erfc(root_c2)/2.0);
    return 1.0/(1.0 + ci + cr);        
}    



/*****************************************************************/

/* This function is not completly implemented */

double erfc(double x) /* Error function of x used by GeomSancer() */
{
	double value;

	value=1.0-x ;
    return value;
}


