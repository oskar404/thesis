/*****************************************************************/
/*                 illum.c                                       */
/*****************************************************************/

/*
    MODULE PURPOSE:
        This module contains the source code for various
        illumination models. Incremental Shading, Empirical
        Models, and Ray Tracing, Transitional Models.

    MODULE CONTENTS:
        IllumInit       - initialize the illumination models
        IllumBouknight  - Bouknignt (1970) illumination model
        IllumPhong      - Phong (1975) illumination model
        IllumBlinn      - Blinn (1976) illumination model
        IllumWhitted    - Whitted (1980) illumination model
        IllumHall       - Hall (1583) illumination model
        IllumExit       - finish with the illumination models

    NOTES:
        The illumination model is called once a surface has
        point on a surface has been identified and the list
        of illuminating light sources has been generated.

        There exists a routine that the illumination models can
        call to get a color from any direction. specifically
        this is used for inquiring about the reflected or
        transmitted directions in the ray tracing models.
        This routine is passed the view vector for which the
        color is required.

        A common calling convention is used for ease in
        interchanging the illumination model routines. Each
        routine is passed the location and orientation of the
        surface, a view vector, an interface material, a
        description of the lighting, and an array to receive
        the computed color.

        The orientation of the surface is given by the surface
        normal which is ALWAYS directed to the 'outside' or
        reflected side of the material.

        The view vector is specified by start position and
        direction vector. During visibility computations the
        view vector is typically directed towards the surface.
        The direction cosines MUST be negated prior to calling
        the illumination model for consistency with the vector
        conventions used.

        See 'illum.h' for material details.

        The light vector is a list of pointers to ILLUM_LGT
        structures terminated by a NULL pointer. The first
        entry is taken as the ambient illumination. Only
        light that is incident from the air side of a material
        can provide illumination.

        These models assume that the material structure is
        correctly loaded and that the surface is facing the
        viewer (N.E > 0) for illumination models that do not
        consider transparency.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "illum.h"
#include "fresnel.h"



static int      num_samples = 0;
static int      (*get_color)() = NULL;
static double   *Fr_buffer = NULL;
static double   *Ft_buffer = NULL;



/*****************************************************************

    int IllumInit(int num_clr_samples,int (*get_clr_routine)())
        num_clr_samples (in) - number of color samples
        get_clr_routine (in) - routine to call to get
                               the color from some direction

    Initializes the illumination model routine set, returns TRUE
    if successful and FALSE upon failure.

*/

int IllumInit(int num_clr_samples,int (*get_clr_routine)())
{
    if(((num_samples=num_clr_samples)<=0) ||
            ((Fr_buffer=(double *)malloc((unsigned)(num_samples *
                                       sizeof(double))))==NULL)||
            ((Ft_buffer=(double *)malloc((unsigned)(num_samples *
                                       sizeof(double))))==NULL))
        {
        (void)IllumExit();
        return FALSE;
        }
    get_color = get_clr_routine;
    return TRUE;
}



/*****************************************************************

    double *IllumBouknight(LINE *surface,LINE *E,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
        surface (in)    - surface for color computation
        E (in)          - view vector
        matl (in)       - material properties
        lgts (in)       - illuminating sources
        color (mod)     - array to receive the color

    Evaluates the color using the Bouknight (1970) illumination
    model. Returns 'color' upon success and NULL upon fairly.

*/

double *IllumBouknight(LINE *surface,LINE *E,ILLUM_MATL *matl,
                       ILLUM_LGT **lgts,double *color)
{
    int         ct;
    double      N_dot_L;
    LINE        L;

    /*  load the ambient illumination */

    for (ct=0; ct<num_samples; ct++)
        {
        color[ct] = matl->Ka_scale * matl->Ka_spectral[ct] *
                    (*lgts)->I_scale * (*lgts)->I_spectral[ct];
        }
    lgts++;

    /*
        load the diffuse component of the illumination. Loop
        through the lights and compute (N.L). If it is positive
        then the surface is illuminated.
    */

    while (*lgts != NULL)
        {
        if ((VectorLine(&(surface->start),&((*lgts)->center),&L) > 0)
               && ((N_dot_L=VectorDot(&(surface->dir),&(L.dir))) > 0))
            {

    /*
        The surface is illuminated by this light. Loop through
        the color samples and sum the diffuse contribution for
        this light to the the color.
    */

            for(ct=0; ct<num_samples; ct++)
                {
                color[ct]+=matl->Kd_scale*matl->Kd_spectral[ct]*N_dot_L*
                           (*lgts)->I_scale * (*lgts)->I_spectral[ct];
                }
            }
            lgts++;
        }
    return color;
}



/*****************************************************************

    double *IllumPhong(LINE *surface,LINE *E,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
        surface (in)    - surface for color computation
        E (in)          - view vector
        matl (in)       - material properties
        lgts (in)       - illuminating sources
        color (mod)     - array to receive the color

    Evaluates the color using the Phong (1975) Illumination
    model. Returns 'color' upon success and NULL upon failure.

    The actual Phong model results when the micro facet distribution
    MFacetPhong() is used, and matl-spectral is the identity
    material.

    Using the micro facet distribution MFacetBlinn() gives Blinn's
    interpretation of the Phong model.

*/

double *IllumPhong(LINE *surface,LINE *E,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
{
    int         ct;
    double      N_dot_L, D;
    LINE        L;

    /* load the ambient illumination */

    for (ct=0; ct<num_samples; ct++)
        {
        color[ct]=matl->Ka_scale*matl->Ka_spectral[ct] *
                  (*lgts)->I_scale*(*lgts)->I_spectral[ct];
        }
    lgts++;

    /*
        load the diffuse and specular illumination components.
        Loop through the lights and compute (N.L). If it is
        positive then the surface is illuminated.
    */

    while (*lgts != NULL)
        {
        if((VectorLine(&(surface->start),&((*lgts)->center),&L) > 0)
                && ((N_dot_L=VectorDot(&(surface->dir),&(L.dir))) > 0))
            {

    /*
        The surface is illuminated. Compute the micro facet
        distribution function.
    */

            D=(*(matl->D))(&(surface->dir),&(L.dir),&(E->dir),
                           matl->D_coeff);

    /*
        Loop through the color samples and sum the diffuse
        and specular contribution for this light to the the
        color.
    */

            for (ct=0; ct<num_samples; ct++)
                {
                color[ct] +=((N_dot_L*matl->Kd_scale *
                             matl->Kd_spectral[ct])+(D*matl->Ks_scale *
                             matl->Ks_spectral[ct])) * (*lgts)->I_scale *
                             (*lgts)->I_spectral[ct];
                }
            }
            lgts++;
        }
    return color;
}



/*****************************************************************

    double *IllumBlinn(LINE *surface,LINE *E,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
        surface (in)    - surface for color computation
        E (in)          - view vector
        matl (in)       - material properties
        lgts (in)       - illuminating sources
        color (mod)     - array to receive the color

    Evaluates the color using the Blinn (1977) illumination
    model. Returns 'color' upon success and NULL upon failure.
    The micro facet distribution functions MFacetBlinn(),
    MFacetGaussian(), and MFacetReitz() are the three functions
    presented by Blinn. The geometric attenuation function
    GeomTorrance() is the attenuation function used by Blinn.
    If matl->G is NULL then the geometric attenuation is omitted.
    The Fresnel reflectance is approximated using the Cook (1983)
    technique.

*/

double *IllumBlinn(LINE *surface,LINE *E,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
{
    int   ct;
    double N_dot_L, N_dot_E, D, G, *Fr;
    LINE  L;
    VECTOR *T, *H;

    /* load the ambient illumination */

    for (ct=0; ct<num_samples; ct++)
        {
        color[ct] = matl->Ka_scale * matl->Ka_spectral[ct] *
                    (*lgts)->I_scale * (*lgts)->I_spectral[ct];
        }
    lgts++;

    /*
        load the diffuse and specular illumination components.
        Loop through the lights and compute (N.L). If it is
        positive then the surface is iilluminated.
    */

    while (*lgts != NULL)
        {
        if((VectorLine(&(surface->start),&((*lgts)->center),&L) > 0)
              && ((N_dot_L=VectorDot(&(surface->dir),&(L.dir))) > 0))
            {

    /*
        The surface is illuminated. Compute the microfacet
        distribution, geometric attenuation, Fresnel
        reflectance and (N.E) for the specular function.
    */

            D = (*(matl->D))(&(surface->dir), &(L.dir), &(E->dir),
                               matl->D_coeff);
            if (matl->G==NULL)
                G = 1.0;
            else
                G = (*(matl->G))(&(surface->dir), &(L.dir),
                    &(E->dir), matl->G_coeff);
            H = VectorH(&(L.dir), &(E->dir));
            if (matl->conductor)
                {
                Fr = FresnelApproxFr(H, &(L.dir), matl->Ro, matl->nt,
                        matl->k, num_samples, matl->Ks_spectral, Fr_buffer);
                }
            else
                {
                T = VectorRefracted(&(E->dir),H, matl->nr, matl->nt);
                Fr = FresnelApproxFrFt(H, &(L.dir), T,
                              matl->Ro, matl->nr, matl->nt, num_samples,
                              matl->Ks_spectral, Fr_buffer, Ft_buffer);
                }

    /*
        Loop through the color samples and sum the diffuse
        and specular contribution for this light to the the
        color. Note the threshold on N_dot_E to prevent
        divide by zero at grazing.
    */

            if((N_dot_E=VectorDot(&(surface->dir),&(E->dir))) > 0.0001)
                {
                for (ct=0; ct<num_samples; ct++)
                    {
                    color[ct]+=((N_dot_L * matl->Kd_scale
                                 * matl->Kd_spectral[ct])
                                 + ((D * G * matl->Ks_scale * Fr[ct])
                                 / N_dot_E)) * (*lgts)->I_scale
                                 * (*lgts)->I_spectral[ct];
                    }
                }
            }
            lgts++;
        }
    return color;
}



/*****************************************************************

    double *IllumWhitted(LINE *surface,LINE *V,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
        surface (in)    - surface for color computation
        V (in)          - view vector
        matl (in)       - material properties
        lgts (in)       - illuminating sources
        color (mod)     - array to receive the color

    Evaluates the color using the Whitted (1980) illumination
    model. Returns 'color' upon success and NULL upon failure.

    The actual Whitted model results when the microfacet.
    distribution MFacetBlinn() is used, and when both matl->spectral
    and matl->Kt_spectral are the identity material.

    The matl->Kt_scale and matl->Kt_spectral are required for this
    illumination model only.

*/

double *IllumWhitted(LINE *surface,LINE *V,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
{
    int     inside = FALSE;
    int     ct;
    double  N_dot_L, D;
    LINE    L;

    /*
        figure out whether we are on the reflected or transmitted
        side of the material interface (outside or inside). If
        we are inside a material, then there is no illumination
        from lights and such - skip directly to reflection and
        refraction contribution.
    */

    if((VectorDot(&(surface->dir),&(V->dir))<0)||(matl->r_is_air))
        {
        for (ct=0; ct<num_samples; ct++)
            color[ct] = 0.0;
        inside = TRUE;
        goto rfl_rfr;
        }

    /*
        If we are at interface between materials, neither of which
        is air, then skip directly to reflection and refraction
    */

    if (!matl->r_is_air)
        goto rfl_rfr;

    /* load the ambient illumination */

    for(ct=0; ct<num_samples; ct++)
        {
        color[ct] = matl->Ka_scale * matl->Ka_spectral[ct] *
                    (*lgts)->I_scale * (*lgts)->I_spectral[ct];
        }
    lgts++;

    /*
        load the diffuse and specular illumination components.
        Loop through the lights and compute (N.L). If it is
        positive then the surface is illuminated.
    */

    while (*lgts != NULL)
        {
        if ((VectorLine(&(surface->start),&((*lgts)->center),&L)>0)
               && ((N_dot_L=VectorDot(&(surface->dir),&(L.dir))) > 0))
            {
    /*
        The surface is illuminated. Compute the micro facet
        distribution function.
    */

            D=(*(matl->D))(&(surface->dir),&(L.dir),&(V->dir),
                                matl->D_coeff);

    /*
        Loop through the color samples and sum the diffuse
        and specular contribution for this light to the the
        color.
    */

            for(ct=0;ct<num_samples;ct++)
                {
                color[ct] +=((N_dot_L * matl->Kd_scale
                             * matl->Kd_spectral[ct])
                             + (D * matl->Ks_scale
                             * matl->Ks_spectral[ct]))
                             * (*lgts)->I_scale
                             * (*lgts)->I_spectral[ct];
                }
            }
            lgts++;
        }

    /*
        compute the contribution from the reflection and
        refraction directions. Get a buffer to hold the
        computed colors, then process the reflected direction
        and the refracted direction.
    */

rfl_rfr:

    if(get_color != NULL)
        {
        double      *Ir_or_It;
        LINE        V_new;
        VECTOR      *T;

        if((Ir_or_It = (double *)malloc((unsigned)(num_samples *
                        sizeof(double)))) == NULL)
            return color;

    /*
        get the reflected vector then ask for the color from
        the reflected direction. If there is a color, then
        sum it with the current color
    */

        V_new.start = surface->start;
        V_new.dir = *(VectorReflected(&(V->dir),&(surface->dir)));
        if ((*get_color)(&V_new, Ir_or_It)!= NULL)
            for (ct=0; ct<num_samples; ct++)
                color[ct]+=Ir_or_It[ct]*matl->Ks_scale*matl->Ks_spectral[ct];

    /*
        if the material is transparent then get the refracted
        vector and ask for the color from the refracted
        direction. If there is a color, then sum it with
        the current color
    */

        if (matl->transparent)
            {
            V_new.start = surface->start;
            if (inside)
                T = VectorRefracted(&(V->dir),&(surface->dir),
                                    matl->nt, matl->nr);
            else
                T = VectorRefracted(&(V->dir),&(surface->dir),
                                    matl->nr, matl->nt);
            if (T != NULL)
                {
                V_new.dir = *T;
                if ((*get_color)(&V_new, Ir_or_It) != NULL)
                    for (ct=0; ct<num_samples; ct++)
                        color[ct]+=Ir_or_It[ct] * matl->Kt_scale
                                   * matl->Kt_spectral[ct];
                }
            }
        (void)free((char *)Ir_or_It);
        }
    return color;
}



/*****************************************************************

    double *IllumHall(LINE *surface,LINE *V,ILLUM_MATL *matl,
                           ILLUM_LGT **lgts,double *color)
        surface (in)    - surface for color computation
        V (in)          - view vector
        matl (in)       - material properties
        lgts (in)       - illuminating sources
        color (mod)     - array to receive the color

    Evaluates the color using the Hall (1983) illumination
    model. Returns 'color' upon success and NULL upon failure.

    The actual Hall model results when the micro facet
    distribution MFacetBlinn() is used, and matl->D = NULL,

    The transmittance is computed from the reflectance, so
    matl->Kt_scale and matl->Kt_spectral are not used in the model.

*/

double *IllumHall(LINE *surface,LINE *V,ILLUM_MATL *matl,
                  ILLUM_LGT **lgts,double *color)
{
    int      inside = FALSE;
    int      ct;
    double   N_dot_L, D, G, *Fr;
    LINE     L;
    VECTOR   *T, *H, *Ht, pseudo_V;

    /*
        figure out whether we are on the reflected or transmitted
        side of the material interface (Outside or inside). If
        we are inside a material, then there may be Illumination
        from outside.
    */

    if (VectorDot(&(surface->dir),&(V->dir)) < 0)
        {
        for (ct=0; ct<num_samples; ct++)
            color[ct] = 0.0;
        inside = TRUE;
        }

    /*
        If we are at interface between materials, neither of which
        is air, then skip directly to reflection and retraction
    */

    if (!matl->r_is_air)
        goto rfl_rfr;

    /*
        load the ambient illumination if we are not inside
        inside the material.
    */

    if (!inside)
        {
        for (ct=0; ct<num_samples; ct++)
            {
            color[ct] = matl->Ka_scale * matl->Ka_spectral[ct] *
                        (*lgts)->I_scale * (*lgts)->I_spectral[ct];
            }
        }
    lgts++;

    /*
        load the diffuse and specular illumination components.
        Loop through the lights and compute (N.L). If it is
        positivethen the surface is illuminated. If it is
        negative, then there may be transmitted illumination.
    */

    while (*lgts != NULL)
        {
        if ((VectorLine(&(surface->start),&((*lgts)->center),&L)>0)
                &&((N_dot_L=VectorDot(&(surface->dir),&(L.dir)))>0)
                && !inside)
            {

    /*
        The surface is illuminated. Compute the microfacet
        distribution, geometric attenuation, Fresnel
        reflectance and (N.V) for the specular function.
    */

            D = (*(matl->D))(&(surface->dir), &(L.dir), &(V->dir),
                          matl->D_coeff);
            if (matl->G == NULL)
                G = 1.0;
            else
                G = (*(matl->G))(&(surface->dir),&(L.dir),
                             &(V->dir), matl->G_coeff);
            H = VectorH(&(L.dir), &(V->dir));
            if (matl->conductor)
                {
                Fr = FresnelApproxFr(H, &(L.dir), matl->Ro, matl->nt,
                         matl->k, num_samples, matl->Ks_spectral, Fr_buffer);
                }
            else
                {
                T = VectorRefracted(&(L.dir), H, matl->nr, matl->nt);
                Fr = FresnelApproxFrFt(H, &(L.dir), T,
                         matl->Ro, matl->nr, matl->nt, num_samples,
                         matl->Ks_spectral, Fr_buffer, Ft_buffer);
                }

    /*
        Loop through the color samples and sum the diffuse
        and specular contribution for this light to the color.
    */

            for (ct=0; ct<num_samples; ct++)
                {
                color[ct] += ((N_dot_L * matl->Kd_scale
                           * matl->Kd_spectral[ct])
                           + (D * G * matl->Ks_scale * Fr[ct]))
                           * (*lgts)->I_scale * (*lgts)->I_spectral[ct];
                }
            }
        else if ((N_dot_L > 0) && inside)
            {
    /*
        We are inside and the light is outside. Compute
        the transmitted contribution from the light
    */

            if ((Ht = VectorHt(&(L.dir),&(V->dir),matl->nr,
                                        matl->nt)) != NULL)
                {

    /*
        The micro facet distribution functions could
        only be equated when cast in terms of the primary
        vectors L, V, and N. A pseudo_V vector is required
        so that any of the distribution functions can be
        applied. Ht is the vector bisector between of the
        angle between L and pseudo_V, thus pseudo V can be
        computed by reflecting L about Ht. Refer to the
        text for details.
    */

                pseudo_V = *(VectorReflected(&(L.dir), Ht));
                D = (*(matl->D))(&(surface->dir), &(L.dir),
                                 &pseudo_V,matl->D_coeff);
                Fr = FresnelApproxFrFt(Ht, &(L.dir), &(V->dir),
                              matl->Ro, matl->nr, matl->nt, num_samples,
                              matl->Ks_spectral, Fr_buffer, Ft_buffer);
                if (matl->G == NULL)
                    G = 1.0;
                else
                    {

    /*
        To include the geometric attenuation, the view
        vector direction must be reversed so that it
        is to the same side of the surface as the
        normal, see text for details.
    */

                    pseudo_V.i = -V->dir.i;
                    pseudo_V.j = -V->dir.j;
                    pseudo_V.k = -V->dir.k;
                    G = (*(matl->G))(&(surface->dir), &(L.dir),
                                        &pseudo_V, matl->G_coeff);
                    }

                for (ct=0; ct<num_samples; ct++)
                    {
                    color[ct] += (D*G*matl->Ks_scale*Ft_buffer[ct])
                              *(*lgts)->I_scale*(*lgts)->I_spectral[ct];
                    }
                }
            }
        lgts++;
        }

    /*
        compute the contribution from the recflection and
        refraction directions. Get a buffer to hold the
        comted colors, then process the reflected directicn
        and the refracted direction.
    */

rfl_rfr:
    if (get_color != NULL)
        {
        double *Ir_or_It;
        LINE    V_new;
        VECTOR  pseudo_N;

        if((Ir_or_It = (double *)malloc((unsigned)(num_samples *
                        sizeof(double)))) == NULL)
            return color;

    /*
        Determine the Fresnel reflectance and transmittance.
        If we are inside the material, then a pseudo normal
        is required that faces to the same side of the
        interface as the view vector.
    */

        if (matl->conductor)
            {
            Fr = FresnelApproxFr(&(surface->dir), &(V->dir), matl->Ro,
                        matl->nt, matl->k, num_samples, matl->Ks_spectral,
                        Fr_buffer);
            }
         else if (inside)
            {
            T = VectorRefracted(&(V->dir),&(surface->dir),
                                matl->nt, matl->nr);
            pseudo_N.i = -surface->dir.i;
            pseudo_N.j = -surface->dir.j;
            pseudo_N.k = -surface->dir.k;
            Fr = FresnelApproxFrFt(&pseudo_N, &(V->dir), T,
                            matl->Ro, matl->nt, matl->nr, num_samples,
                            matl->Ks_spectral,Fr_buffer,Ft_buffer);
            }
         else
            {
            T = VectorRefracted(&(V->dir),&(surface->dir),
                                     matl->nr, matl->nt);
            Fr = FresnelApproxFrFt(&(surface->dir), &(V->dir),
                            T, matl->Ro, matl->nr, matl->nt, num_samples,
                            matl->Ks_spectral, Fr_buffer, Ft_buffer);
            }

    /*
        get the reflected vector then ask for the color from
        the reflected direction. If there is a color, then
        sum it with the current color
    */

        V_new.start = surface->start;
        V_new.dir = *(VectorReflected(&(V->dir),&(surface->dir)));
        if ((*get_color)(&V_new, Ir_or_It) != NULL)
            {
            for (ct=0; ct<num_samples; ct++)
                color[ct] +=Ir_or_It[ct]*matl->Ks_scale*Fr[ct];
            }

    /*
        if the material is transparent then get the refracted
        vector and ask for the color from the refracted
        direction. If there is a color, then sum it with
        the current color.
    */

        if(matl->transparent && (T != NULL))
            {
            V_new.start = surface->start;
            V_new.dir = *T;
            if ((*get_color)(&V_new, Ir_or_It) != NULL)
                for (ct=0; ct<num_samples; ct++)
                    color[ct]+=Ir_or_It[ct]*matl->Kt_scale*Ft_buffer[ct];
            }
        (void)free((char *)Ir_or_It);
        }

    /*
        If we are inside a material that has a filter attenuation
        then apply the attenuation to the color.
    */

    if(((!inside) && ((Fr = matl->Ar_spectral) != NULL)) ||
              ((inside) && ((Fr = matl->At_spectral) != NULL)))
        {
        double      dist;

        dist = sqrt ( ((surface->start.x - V->start.x) *
                      (surface->start.x - V->start.x)) +
                      ((surface->start.y - V->start.y) *
                      (surface->start.y - V->start.y)) +
                      ((surface->start.z - V->start.z) *
                      (surface->start.z - V->start.z)) );
        for (ct=0; ct<num_samples; ct++)
            color[ct] *= pow(Fr[ct],dist);
        }
    return color;
}



/*****************************************************************

    int IllumExit(void)

    Finishes use of the illumination models routines.

*/

int IllumExit(void)
{
    if(Fr_buffer != NULL)
            {
            (void)free((char *)Fr_buffer);
            Fr_buffer = NULL;
            }
    if (Ft_buffer != NULL)
        {
        (void)free((char *)Ft_buffer);
        Ft_buffer = NULL;
        }
    num_samples = 0;
    get_color = NULL;
    return TRUE;
}