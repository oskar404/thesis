/*****************************************************************/
/*                  fresnel.c                                    */
/*****************************************************************/

/*
    MODULE PURPOSE:
        This module contains routines to for fresnel calculations.

    MODULE CONTENTS:
        FresnelDpl()        - reflectance for a dielectric,
                              parallel polarized,
        FresnelDpp()        - reflectance for a dielectric,
                              perpendicular polarized
        FresnelDR()         - reflectance for a dielectric
        FresnelCpl()        - reflectance for a conductor,
                              parallel polarized,
        FresnelCpp()        - reflectance for a conductor,
                              perpendicular polarized
        FresnelCR()         - reflectance for a conductor
        FresnelApproxN()    - approximate the average index of
                              refraction for a material
        FresnelApproxK()    - approximate the average absorption
                              index for a material
    ASSUMPTIONS:
        The following are defined in math.h
            HUGE largest floating pont valour
            M_PI_2 pi/2
            M_PI_4 pi/q

*/

#include <stdio.h>
#include <math.h>
#include "vector.h"
#include "fresnel.h"



#ifndef HUGE
#define HUGE 999999999999999999.0
#endif /* HUGE */



/*****************************************************************

    double FresnelDpl(VECTOR *N,VECTOR *L,VECTOR *T,double ni,double nt)
    N, L, T (in) - N, L, T vectors
    ni (in) - incident index of refraction
    nt (in) - transmitted index of refraction

    Returns the reflectance of a dielectric for light with
    polarization parallel to the plane of incidence (plane of the
    N and L vector). N and L are assumed to be to the same side of
    the surface.

*/

double FresnelDpl(VECTOR *N,VECTOR *L,VECTOR *T,double ni,double nt)
{
    double amplitude, N_dot_L, N_dot_T;

    N_dot_L = VectorDot(N, L);
    N_dot_T = VectorDot(N, T);
    amplitude = ((nt*N_dot_L)+(ni*N_dot_T))/((nt*N_dot_L)-(ni*N_dot_T));
    return amplitude*amplitude;
}



/*****************************************************************

    double FresnelDpp(VECTOR *N,VECTOR *L,VECTOR *T,double ni,double nt)
        N, L, T (in) - N, L, T vectcrs
        ni (in) - incident index of refraction
        nt (in) - transmitted index of refraction

    Returns the reflectance of a dielectric for light with
    polarization perpendicular to the plane of incidence
    (plane of the N and L vector). N and L are assumed to be to
    the same side of the surface.

*/

double FresnelDpp(VECTOR *N,VECTOR *L,VECTOR *T,double ni,double nt)
{
    double amplitude, N_dot_L, N_dot_T;

    N_dot_L = VectorDot(N, L);
    N_dot_T = VectorDot(N, T);
    amplitude=((ni*N_dot_L)+(nt*N_dot_T))/((ni*N_dot_L)-(nt*N_dot_T));
    return amplitude * amplitude;
}



/*****************************************************************

    double FresnelDR(VECTOR *N,VECTOR *L,VECTOR *T,double ni,double nt);
        N, L, T (in) - N, L, T vectcrs
        ni (in) - incident index of refraction
        nt (in) - transmitted index of refraction

    Returns the average reflectance for a dielectric. N and L are
    assumed to be to the same side of the surface.

*/

double FresnelDR(VECTOR *N,VECTOR *L,VECTOR *T,double ni,double nt)
{
    return (FresnelDpl(N,L,T,ni,nt) +FresnelDpp(N,L,T,ni,nt))/2.0;
}



/*****************************************************************

    double FresnelCpl(VECTOR *N,VECTOR *L,double nt,double k)
        N, L (in) - N, L vectors
        nt (in) - index of refraction
        k (in) - absorption coefficient

    Returns the reflectance of a conductor for light with
    polarization parallel to the plane of incidence (plane of the
    N and L vector). N and L are assumed to be to the same side
    of the surface.

*/

double FresnelCpl(VECTOR *N,VECTOR *L,double nt,double k)
{
    double tmp,N_dot_L;

    N_dot_L = VectorDot(N, L);
    tmp = ((nt*nt)+(k*k))*N_dot_L*N_dot_L;
    return (tmp-(2.0*nt*N_dot_L)+1)/(tmp+(2.0*nt*N_dot_L)+1);
}



/*****************************************************************

    double FresnelCpp(VECTOR *N,VECTOR *L,double nt,double k)
        N, L (in) - N, L vectors
        nt (in) - index of refraction
        k (in) - absorption coefficient

    Returns the reflectance of a conductor for light with
    polarization perpendicular to the plane of incidence (plane of
    the N and L vector). N and L are assumed to be to the same side
    of the surface.

*/

double FresnelCpp(VECTOR *N,VECTOR *L,double nt,double k)
{
    double tmp, N_dot_L;

    N_dot_L = VectorDot(N, L);
    tmp = (nt * nt) + (k * k);
    return (tmp-(2.0*nt*N_dot_L)+(N_dot_L*N_dot_L)) /
           (tmp+(2.0*nt*N_dot_L)+(N_dot_L*N_dot_L));
}



/*****************************************************************

    double FresnelCR(VECTOR *N,VECTOR *L,double nt,double k)
        N, L (in) - N, L vectors
        nt (in) - index of refraction
        k (in) - absorption coefficient

    Returns the average reflectance for a conductor. N and L
    are assumed to be to the same side of the surface.

*/

double FresnelCR(VECTOR *N,VECTOR *L,double nt,double k)
{
    return (FresnelCpl(N,L,nt,k)+FresnelCpp(N,L,nt,k))/2.0;
}



/*****************************************************************

    double FresnelApproxN(double *Ro,int n_samp,double *material)
        Ro (out) - average reflectance
        n_samp (in) - number of material samples
        material (in) - material spectral curve

    Returns the approximate n, and loads Ro. This gives the
    correct n for a single "material" reflectance if the material
    is a dielectric, and the correct n assuming k=0 for a conductor.

*/

double FresnelApproxN(double *Ro,int n_samp,double *material)
{
    int ct;

    *Ro=0.0;
    for(ct=n_samp; --ct>=0; )
        *Ro += *material++;
    *Ro /= (double)n_samp;
    return (1.0+sqrt(*Ro))/(1.0-sqrt(*Ro));
}



/*****************************************************************

    double FresnelApproxK(double *Ro,int n_samp,double *material)
        Ro (out) - average reflectance
        n_samp (in) - number of material samples
        material (in) - material spectral curve

    Returns the approximate k, and loads Ro. This assumes n=1.0
    and is applicable to conductors only.

*/

double FresnelApproxK(double *Ro,int n_samp,double *material)
{
    int ct;

    *Ro = 0.0;
    for (ct=n_samp; --ct>=0; )
        *Ro += *material++;
    *Ro/=(double)n_samp;
    return 2.0*sqrt(*Ro/(1.0-*Ro));
}



/*****************************************************************

    double *FresnelApproxFr(VECTOR *N,VECTOR *L,double Ro,double n,
                            double k,int n_samp,double *mtl,double *Fr)
        N, L (in) - N, L vectors
        Ro (in) - average reflectance
        n (in) - measured or aproximate index of refraction
        k (in) - measured or approximate absorption coefficient
       n_samp (in) - number of material samples
        mtl (in) - material spectral curve
        Fr (out) - reflectance

    Loads the vector Fr with the approximate reflectance for each
    color sample point for a conductor. Returns the pointer to Fr.
    The measured values for n and k should be used if they are
    available. Otherwise, assume k=0 and approximate n using
    FresnelApproxN(), or assume nil and approximate k using
    FresnelApproxK().

*/

double *FresnelApproxFr(VECTOR *N,VECTOR *L,double Ro,double n,
                        double k,int n_samp,double *mtl,double *Fr)
{
    double R_theta;  /* average R for N and L */
    double factor, *R_out;

    R_theta = FresnelCR(N,L,n,k);
    factor = (R_theta-Ro)/(1.0-Ro);
    for(R_out=Fr;--n_samp;Fr++,mtl++)
        if((*Fr=*mtl+((1.0-*mtl)*factor))< 0.0)
            *Fr=0.0;
    return R_out;
}



/*****************************************************************

    double *FresnelApproxFrFt(VECTOR *N,VECTOR *L,VECTOR *T,double Ro,double ni,
                              double nt, int n_samp,double *mtl_p,double *Fr,
                              double *Ft)
        N, L, T (in) - N, L, T vectors
        Ro (in) average reflectance
        ni (in) ave n for incident material
        nt (in) ave n for transmitted material
        n_samp (in) number of material samples
        mtl_p (in) pseudo material curve
        Fr (out) reflectance
        Ft (out) transmittance

    Loads the vector Fr with the approximate reflectance and Ft
    with the approximate transmittance for each sample point for
    a conductor. Returns the pointer to Fr. The measured value
    for n should be used if it is available, otherwise use
    FresnelApproxN() to approximate the index of refraction.

*/

double *FresnelApproxFrFt(VECTOR *N,VECTOR *L,VECTOR *T,double Ro,double ni,
                          double nt, int n_samp,double *mtl_p,double *Fr,
                          double *Ft)
{
    double R_theta; /* ave R for N and L*/
    double factor, *R_out;

    if (T == NULL)      /* internal reflection */
        for (R_out=Fr; --n_samp>=0; *Fr++ = 1.0, *Ft++ = 0.0);
    else
        {
         R_theta = FresnelDR(N,L,T,ni,nt);
         factor = (R_theta - Ro) / (1.0 - Ro);
         for(R_out=Fr;--n_samp>=0;mtl_p++)
            {
            if ((*Fr = *mtl_p + ((1.0-*mtl_p)*factor)<0.0))
                *Fr = 0.0;
            *Ft++ = 1.0-*Fr++;
            }
        }
    return R_out;
}