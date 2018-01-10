/***************************************************************/
/*                     vector.c                                */
/***************************************************************/

/*         
   MODULE PURPOSE:
      This module contains routines that preform common
      vector geometric manipulations for image synthesis.
         
   MODULE CONTENTS:
      VecotrDot   		- vector dot product
	  VectorCross 		- vector cross product
	  VectorNorm  		- vector normalization
	  VectorGetv  		- generate a vector
	  VectorReflected   - compute the reflected vector
      VectorRefracted   - compute the refracted vector
      VectorH     		- compute H vector
      VectorHt     	    - compute the H' vector
*/
         
#include <stdio.h>
#include <math.h>
#include "vector.h"


         
/***************************************************************
         
		 double VectorDot (VECTOR *v0,VECTOR *vl)
		 v0, vl normalised direction vectors
         

         Returns the dot product of two direction vectors. 
		 Note that the dot product is the cosine between the
		 direction vectors because the direction vectors are
		 normalised.
         
*/
        
double VectorDot (VECTOR *v0,VECTOR *v1)
{
	return ((v0->i*v1->i)+(v0->j*v1->j)+(v0->k*v1->k));
}



/***************************************************************

		 VECTOR VectorCross (VECTOR *v0,VECTOR *vl)
		 v0, vl normalised direction vectors
         
         Returns the cross product of two direction vectors.

*/
         
VECTOR *VectorCross (VECTOR *v0,VECTOR *v1)
{
	static VECTOR	v2;

    v2.i = (v0->j * v1->k) - (v0->k * v1->j);
    v2.j = (v0->k * v1->i) - (v0->i * v1->k);
    v2.k = (v0->i * v1->j) - (v0->j * v1->i);
    return &v2;
}


    
/***************************************************************

		 double VectorNorm (VECTOR *v0)
         v0 (mod) vector to be normalised

         Returns the vector length before normalization.
		 Returns zero if the vector could not be normalised.
         Note that the input vector is modified.

*/

double VectorNorm (VECTOR *v0)
{
	double          len;

	if ((len = VectorDot(v0, v0)) <= 0.0)
		return FALSE;
    len = sqrt((double)len);
    v0->i /= len;
    v0->j /= len;
	v0->k /= len;
    return len;
}
         


/***************************************************************

         double VectorLine (POINT *p0,POINT *p1,LINE *v)
		 p0, p1 (in) from, to points
         v (out) generated line
         
         Returns the distance from p0 to p1. Returns 0.0
         on error (p0=p1). The line is expressed in parametric
         form with a start point (p0) and a normalised direction
		 vector.

*/

double VectorLine (POINT *p0,POINT *p1,LINE *v)
{
	v->start = *p0;
    v->dir.i = p1->x - p0->x;
    v->dir.j = p1->y - p0->y;
    v->dir.k = p1->z - p0->z;
    return VectorNorm(&v->dir);
}



/***************************************************************

		 VECTOR *VectorReflected (VECTOR *L,VECTOR *N)
		 L (in) incident vector
		 N (in) surface normal

         Returns the reflected direction vector. The reflected
         direction is computed using the method given by Whitted
         (1980).

*/
         
VECTOR *VectorReflected (VECTOR *L,VECTOR *N)
{
	double 			N_dot_L;
    static VECTOR 	rfl;

    N_dot_L = VectorDot (N,L);
	rfl.i = (2.0 * N_dot_L * N->i) - L->i;
    rfl.j = (2.0 * N_dot_L * N->j) - L->j;
    rfl.k = (2.0 * N_dot_L * N->k) - L->k;
    return &rfl;
}


		          
/***************************************************************

         VECTOR *VectorRefracted (VECTOR *L,VECTOR *N,double ni,double nt)
		 L (in) incident vector
         N (in) surface normal
         ni (in) index of refraction for the
                 material on the front of the
                 interface (same side as N)
         nt (in) index of refraction for the
                 material on the back of the
                 interface (opposite size as N)
         
         Returns the refracted vector, if thereis  complete internal
		 refracted vector otherwise a NULL vector is returned. The
		 vector is computed using the method given by Hall (1983).

*/
         
VECTOR *VectorRefracted (VECTOR *L,VECTOR *N,double ni,double nt)
{
	static VECTOR	T;	/* the refracted vector */
    VECTOR  sin_T;		/* sin vector of the refracted vector */
    VECTOR	cos_L;		/* cos vector of the incident vector */
    double  len_sin_T; 	/* length of sin T squared */
    double  n_mult; 	/* ni over nt */
    double  N_dot_L;
	double  N_dot_T;

	if ((N_dot_L = VectorDot(N,L)) > 0.0)
		n_mult = ni / nt;
	else
		n_mult = nt / ni;
    cos_L.i = N_dot_L * N->i;
    cos_L.j = N_dot_L * N->j;
    cos_L.k = N_dot_L * N->k;
    sin_T.i = (n_mult) * (cos_L.i - L->i);
    sin_T.j = (n_mult) * (cos_L.j - L->j);
    sin_T.k = (n_mult) * (cos_L.k - L->k);
    if ((len_sin_T = VectorDot(&sin_T, &sin_T)) >= 1.0)
         return NULL; /* internal reflection */
    N_dot_T=sqrt(1.0-len_sin_T);
	if(N_dot_L<0.0)
		N_dot_T=-N_dot_T;
	T.i = sin_T.i - (N->i * N_dot_T);
	T.j = sin_T.j - (N->j * N_dot_T);
	T.k = sin_T.k - (N->k * N_dot_T);
	return &T;
}
         


/***************************************************************

		 VECTOR *VectorH (VECTOR *L,VECTOR *E)
         L (in) incident vector
        E (in) reflected vector
         
         Returns H, NULL on error (if L+H = 0).

*/

VECTOR *VectorH (VECTOR *L,VECTOR *E)
{
	static VECTOR	H;

    H.i = L->i + E->i;
    H.j = L->j + E->j;
    H.k = L->k + E->k;
	if (VectorNorm(&H))
		return NULL;
    return &H;
}        


		          
/***************************************************************

        VECTOR *VectorHt(VECTOR *L,VECTOR *T,double ni,double nt)
		L (in) incident vector
        T (in) transmitted hector
        ni (in) incident index
		nt (in) transmitted index
         
        Returns H' oriented to the same side of the surface
        as L computed using the method suggested by
        Hall (1983). Returns NULL on error (if the angle
        between V and L is less than the critical angle).
*/
  
VECTOR *VectorHt(VECTOR *L,VECTOR *T,double ni,double nt)
{
	double      L_dot_T;
    double      divisor;
    static VECTOR     Ht;
         
    L_dot_T = -(VectorDot(L,T));
    /* check for special cases */
    if (ni == nt) {
		if (L_dot_T == 1.0)
			return L;
		else
			return NULL;
		}        
    if (ni < nt) {
		if (L_dot_T < ni/nt)
		 	return NULL;
        divisor = (nt / ni) - 1.0;
		Ht.i = -(((L->i + T->i) / divisor) + T->i);
		Ht.j = -(((L->j + T->j) / divisor) + T->j);
		Ht.k = -(((L->k + T->k) / divisor) + T->k);
        }
	else
		{
		if (L_dot_T < nt/ni)
			return NULL;
        divisor = (ni / nt) - 1.0;
		Ht.i = ((L->i + T->i) / divisor) + L->i;
        Ht.j = ((L->j + T->j) / divisor) + L->j;
        Ht.k = ((L->k + T->k) / divisor) + L->k;
    	}     
	(void)VectorNorm(&Ht);
	return &Ht;
}
         

