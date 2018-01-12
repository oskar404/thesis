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
    Render.c    - Module for rendering a softproof picture
*/



#include <math.h>
#include "c_types.h"
#include "picture.h"
#include "access.h"
#include "message.h"
#include "vector.h"
#include "mfacet.h"
#include "color.h"
#include "paper.h"
#include "light.h"
#include "ink.h"
#include "render.h"



/**************************************************************/

/* structure and type definitions */

#define VIEW_VECTOR { 0.0 , 0.0 , 1.0 }
#define N_AIR 1.0
#define SPECULAR_RATIO 0.1


typedef struct {
        ColorType Ambient;
        ColorType Diffuse;
        ColorType Specular;
        double SpecularPower;
        double Ni;
        double AveRefl;
        } MATERIAL;

typedef struct {
        ColorType Ambient;
        ColorType Specular;
        } LIGHT;

typedef struct {
        ColorType Color;
        ColorType Fresnell;
        int Samples;
        } PICTURE;


/* Global variables for this file */

static MATERIAL mtl={NULL,NULL,NULL,0.0,0.0,0.0};
static MATERIAL *paper=NULL;
static MATERIAL *ink=NULL;
static LIGHT lgt={NULL,NULL};
static PICTURE pic={NULL,NULL,0};

/* Extra global variables for ink handling */
static ColorType specular=NULL;
static ColorType diffuse=NULL;
static ColorType ambient=NULL;


/* Internal functions */

static Logical InitGlobals(void);
static void ExitGlobals(void);
static Logical InitExtraGlobals(void);
static void ExitExtraGlobals(void);

static RGBType *Phong(VECTOR *,VECTOR *,VECTOR *,POINT *);
static RGBType *Blinn(VECTOR *,VECTOR *,VECTOR *,POINT *);

static double GeometricTerm(VECTOR *,VECTOR *,VECTOR *,VECTOR *);
static double FresnelApproxN(ColorType,double *,int);
static double FresnelDR(VECTOR*,VECTOR*,VECTOR*,double,double);
static void   FresnelApproxFr(VECTOR*,VECTOR*,VECTOR*,ColorType,
                              ColorType,double,double,double,int);
static double CalculateSpectralColors(POINT *);



/**************************************************************/



/**************************************************************

    Logical RenderImage(String name,TIFF *tif,double PixelSize,
                        int SzX,int SzY,int model)

    A function for rendering a picture

*/

Logical RenderImage(RenderType picture)
{
    int         i,row;
    buffer_t    buf;
    RGBType    *rgb;
    VECTOR      light,paper,view=VIEW_VECTOR;
    POINT       px,seen_px;

    if((buf=AllocRowBuffer(picture.Tif))==NULL)
        goto error;
    if(InitGlobals()==FALSE)
        goto error;
    if(picture.UseInk==TRUE)
        if(InitExtraGlobals()==FALSE)
            goto error;
    if(picture.ViewDir!=0)
        {
        view.i=sin(picture.ViewDir);
        view.j=0;
        view.k=cos(picture.ViewDir);
        }
    px.x=0.0;
    px.y=0.0;
    px.z=0.0;
    LightVector(&light,&px);

    switch(picture.Model)
        {


        /* uses Phong illumination model */

        case PHONG:

            for(row=0;row<picture.Y;row++)   /* This for loop does the actual making */
                {                      /* of the TIFF test file */
                px.x=0.0;
                for(i=0;i<picture.X;i++)     /* This for loop makes data for a RGB row */
                        {
                        PaperHiddenPixel(&view,&px,&seen_px);
                        PaperGetNormalVector(&paper,&seen_px);
                        if(picture.UseInk==TRUE)
                            mtl.SpecularPower=MFacetPhongInit(
                                              CalculateSpectralColors(&seen_px));
                        else
                            mtl.SpecularPower=MFacetPhongInit(
                                              PaperGetSpecularBeta(&seen_px));
                        rgb=Phong(&paper,&light,&view,&seen_px);
                        PutPixel(buf,RGB,RED,i,(value_t)rgb->r);
                        PutPixel(buf,RGB,GREEN,i,(value_t)rgb->g);
                        PutPixel(buf,RGB,BLUE,i,(value_t)rgb->b);
                        px.x+=picture.PixelSize;
                        }
                if(WriteRowBuffer(picture.Tif,buf,row)==FALSE)  /* Writes buffer to file */
                    MessageError("Error in writing to TIFF file");
                px.y+=picture.PixelSize;
                MessageNumber(picture.Name,row);
                }
            break;


        /* uses Blinn illumination model */

        case BLINN:

            for(row=0;row<picture.Y;row++)   /* This for loop does the actual making */
                {                      /* of the TIFF test file */
                px.x=0.0;
                for(i=0;i<picture.X;i++)     /* This for loop makes data for a RGB row */
                        {
                        PaperHiddenPixel(&view,&px,&seen_px);
                        PaperGetNormalVector(&paper,&seen_px);
                        if(picture.UseInk==TRUE)
                            mtl.SpecularPower=MFacetBlinnInit(
                                              CalculateSpectralColors(&seen_px));
                        else
                            mtl.SpecularPower=MFacetBlinnInit(
                                              PaperGetSpecularBeta(&seen_px));
                        rgb=Blinn(&paper,&light,&view,&seen_px);
                        PutPixel(buf,RGB,RED,i,(value_t)rgb->r);
                        PutPixel(buf,RGB,GREEN,i,(value_t)rgb->g);
                        PutPixel(buf,RGB,BLUE,i,(value_t)rgb->b);
                        px.x+=picture.PixelSize;
                        }
                if(WriteRowBuffer(picture.Tif,buf,row)==FALSE)  /* Writes buffer to file */
                    MessageError("Error in writing to TIFF file");
                px.y+=picture.PixelSize;
                MessageNumber(picture.Name,row);
                }
            break;

        /* Wrong illumination model */

        default:
            goto error;
        }


    ExitGlobals();
    ExitExtraGlobals();
    FreeRowBuffer(buf);

    return TRUE;

error:
    ExitGlobals();
    ExitExtraGlobals();
    FreeRowBuffer(buf);

    return FALSE;
}




/**************************************************************
    Internal functions for this file
***************************************************************/



/*****************************************************************

    static Logical InitGlobals(void)

    Initializes the global variables.

*/

static Logical InitGlobals(void)
{
    int ct;

    /* Init pic */

    if((pic.Color=ColorVectorInit())==NULL)
        return FALSE;
    if((pic.Fresnell=ColorVectorInit())==NULL)
        return FALSE;
    pic.Samples=ColorGetSize();

    /* Init material data mtl */

    if((mtl.Diffuse=PaperGetDiffuse())==NULL)
        return FALSE;
    if((mtl.Specular=PaperGetSpecular())==NULL)
        return FALSE;
    if((mtl.Ambient=PaperGetAmbient())==NULL)
        return FALSE;
    mtl.Ni=FresnelApproxN(mtl.Specular,&mtl.AveRefl,pic.Samples);

    /* Init light data lgt */

    if((lgt.Ambient=LightAmbientColor())==NULL)
        return FALSE;
    if((lgt.Specular=LightSpecColor())==NULL)
        return FALSE;

    return TRUE;
}



/*****************************************************************

    static void ExitGlobals(void)

    Exits the global variables.

*/

static void ExitGlobals(void)
{

    /* Exit pic */

    if(pic.Color!=NULL)
        ColorVectorExit(pic.Color);
    if(pic.Fresnell!=NULL)
        ColorVectorExit(pic.Fresnell);
    pic.Color=NULL;
    pic.Fresnell=NULL;
    pic.Samples=0;

    /* Exit material data mtl */

    mtl.Diffuse=NULL;
    mtl.Specular=NULL;
    mtl.Ambient=NULL;
    mtl.AveRefl=0.0;
    mtl.Ni=0.0;

    /* Exit Light data lgt */

    lgt.Ambient=NULL;
    lgt.Specular=NULL;

    return;
}



/*****************************************************************

    static Logical InitExtraGlobals(void)

    Initializes the global variables.

*/

static Logical InitExtraGlobals(void)
{
    if((specular=ColorVectorInit())==NULL)
        return FALSE;
    if((diffuse=ColorVectorInit())==NULL)
        return FALSE;
    if((ambient=ColorVectorInit())==NULL)
        return FALSE;

    if((ink=MemoryAllocate(MATERIAL,1))==NULL)
        return FALSE;
    if((paper=MemoryAllocate(MATERIAL,1))==NULL)
        return FALSE;

    if((ink->Diffuse=InkGetDiffuse())==NULL)
        return FALSE;
    if((ink->Specular=InkGetSpecular())==NULL)
        return FALSE;
    if((ink->Ambient=InkGetAmbient())==NULL)
        return FALSE;

    paper->Diffuse=mtl.Diffuse;
    paper->Specular=mtl.Specular;
    paper->Ambient=mtl.Ambient;

    mtl.Diffuse=diffuse;
    mtl.Specular=specular;
    mtl.Ambient=ambient;
    return TRUE;
}



/*****************************************************************

    static void ExitExtraGlobals(void)

    Exits the extra global variables.

*/

static void ExitExtraGlobals(void)
{
    if(specular!=NULL)
        {
        ColorVectorExit(specular);
        specular=NULL;
        }
    if(diffuse!=NULL)
        {
        ColorVectorExit(diffuse);
        diffuse=NULL;
        }
    if(ambient!=NULL)
        {
        ColorVectorExit(ambient);
        ambient=NULL;
        }
    if(ink!=NULL)
        {
        MemoryFree(ink);
        ink=NULL;
        }
    if(paper!=NULL)
        {
        MemoryFree(paper);
        paper=NULL;
        }
    return;
}



/*****************************************************************

    static RGBType *Phong(VECTOR *,VECTOR *,VECTOR *,POINT *);

    Evaluates the color using the Phong (1975) Illumination
    model. Returns TRUE upon success.

*/

static RGBType *Phong(VECTOR *Normal,VECTOR *Light,VECTOR *View,POINT *px)
{
    int         ct;
    double      N_dot_L,D;

    if(PaperSelfShadow(Light,px)==TRUE)
        for (ct=0; ct<pic.Samples; ct++)
            pic.Color[ct]=lgt.Ambient[ct]*mtl.Ambient[ct];
    else
        {
        N_dot_L=VectorDot(Normal,Light);
        D=MFacetPhong(Normal,Light,View,mtl.SpecularPower);
        for (ct=0; ct<pic.Samples; ct++)
            pic.Color[ct]=lgt.Ambient[ct]*mtl.Ambient[ct]+
                      lgt.Specular[ct]*
                      (mtl.Diffuse[ct]*N_dot_L+mtl.Specular[ct]*D);
        }
    return ColorGetRGB(pic.Color);
}



/*****************************************************************

    static RGBType *Blinn(VECTOR *,VECTOR *,VECTOR *,POINT *)

    Evaluates the color using the Blinn illumination model.

*/

static RGBType *Blinn(VECTOR *Normal,VECTOR *Light,VECTOR *View,POINT *px)
{
    int   ct;
    double N_dot_L, N_dot_V, D, G;
    VECTOR *T,*H;

    if(PaperSelfShadow(Light,px)==TRUE)
        for (ct=0; ct<pic.Samples; ct++)
            pic.Color[ct]=lgt.Ambient[ct]*mtl.Ambient[ct];
    else
        {
        /* Calculate micro facet normal vector H
           and refracted vector T */

        H=VectorH(Light,View);
        T=VectorRefracted(Light,Normal,N_AIR,mtl.Ni);

        /* Calculate nicro facet distribution D
           and geometric attenuation G
           and Fresnell reflection pic.Fressnell */

        D=MFacetBlinn(Normal,Light,View,mtl.SpecularPower);
        G=GeometricTerm(Normal,Light,View,H);
        FresnelApproxFr(Normal,Light,T,mtl.Specular,pic.Fresnell,N_AIR,
                    mtl.Ni,mtl.AveRefl,pic.Samples);

        N_dot_L=VectorDot(Normal,Light);
        N_dot_V=VectorDot(Normal,View);

        if(N_dot_V > 0.0001)
            for(ct=0; ct<pic.Samples; ct++)
                pic.Color[ct]=lgt.Ambient[ct]*mtl.Ambient[ct]
                      +(N_dot_L*mtl.Diffuse[ct]
                      +(D*G*pic.Fresnell[ct])/N_dot_V)
                      *lgt.Specular[ct];
        else
            for(ct=0; ct<pic.Samples; ct++)  /* Full grazing angle */
                pic.Color[ct]=lgt.Ambient[ct]*mtl.Ambient[ct]+lgt.Specular[ct];
        }
    return ColorGetRGB(pic.Color);
}



/*****************************************************************

    static double CalculateSpectralColors(POINT *px)

    Returns specular beta value.

*/

static double CalculateSpectralColors(POINT *px)
{
    double inkM,beta=0.0;

    inkM=InkTransfer(px);
    if(inkM==0.0)
        {
        mtl.Specular=paper->Specular;
        mtl.Diffuse=paper->Diffuse;
        mtl.Ambient=paper->Ambient;
        beta=PaperGetSpecularBeta(px);
        }
    else
        {
        double papM;
        int ct;

        papM=exp(-2*inkM*InkAbsorptionCoefficient());
        inkM=1-papM;
        beta=InkGetSpecularBeta();
        mtl.Specular=ink->Specular;
        for(ct=0;ct<pic.Samples;ct++)
            {
            diffuse[ct]=papM*paper->Diffuse[ct]+inkM*ink->Diffuse[ct];
            ambient[ct]=papM*paper->Ambient[ct]+inkM*ink->Ambient[ct];
            }
        mtl.Diffuse=diffuse;
        mtl.Ambient=ambient;
        }
    return beta;
}



/*****************************************************************

    double GeometricTerm(VECTOR *Normal,VECTOR *Light,
                         VECTOR *View,Vector *H)

    Returns geometric attenuation (Torrance and Sparrow
    1967). The direction vectors Normal, View, and Light are
    assumed to be oriented to the same side of the surface.

*/

static double GeometricTerm(VECTOR *Normal,VECTOR *Light,
                            VECTOR *View,VECTOR *H)
{
    double N_dot_H, V_dot_H;
    double N_dot_V, N_dot_L;
    double g1, g2;

    N_dot_H = VectorDot(Normal,H);
    V_dot_H = VectorDot(View,H);
    N_dot_V = VectorDot(Normal,View);
    N_dot_L = VectorDot(Normal,Light);

    if((g1=(2.0*N_dot_H*N_dot_V)/V_dot_H)>1.0)
        g1 = 1.0;
    if((g2=(2.0*N_dot_H*N_dot_L)/V_dot_H)<g1)
        return g2;
    else
        return g1;
}



/*****************************************************************

    static double FresnelApproxN(ColorType mtl,double *Ro,int samples)

    Returns the approximate n, and loads Ro. This gives the
    correct n for a single "material" reflectance if the material
    is a dielectric, and the correct n assuming k=0 for a conductor.

*/

static double FresnelApproxN(ColorType mtl,double *Ro,int samples)
{
    int ct;

    *Ro=0.0;
    for(ct=0; ct<samples;ct++ )
        *Ro += mtl[ct];
    *Ro /= (double)samples;
    return (1.0+sqrt(*Ro))/(1.0-sqrt(*Ro));
}



/*****************************************************************

    static void FresnelApproxFr(VECTOR *N,VECTOR *L,VECTOR *T,
                            ColorType mtl,ColorType Fr,
                            double ni,double nt,double Ro,int samples)

    Loads the vector Fr with the approximate reflectance for each
    sample point for a dielectric material.
    FresnelApproxN() to approximate the index of refraction.

*/

static void FresnelApproxFr(VECTOR *N,VECTOR *L,VECTOR *T,ColorType mtl,ColorType Fr,
                            double ni,double nt,double Ro,int samples)
{
    double factor;
    int ct;

    if (T == NULL)      /* internal reflection */
        for (ct=0; ct<samples; ct++)
            Fr[ct] = 1.0;
    else
        {
         factor=(FresnelDR(N,L,T,ni,nt)-Ro)/(1.0-Ro);
         for(ct=0;ct<samples;ct++)
            {
            if ((Fr[ct] = mtl[ct] + ((1.0-mtl[ct])*factor))<0.0)
                Fr[ct] = 0.0;
            }
        }
    return;
}



/*****************************************************************

    static double FresnelDR(VECTOR *N,VECTOR *L,VECTOR *T,
                            double ni,double nt)

    Returns the average reflectance for a dielectric. N and L are
    assumed to be to the same side of the surface.

*/

static double FresnelDR(VECTOR *N,VECTOR *L,VECTOR *T,
                        double ni,double nt)
{
    double Rpl,Rpp,N_dot_L,N_dot_T;

    N_dot_L = VectorDot(N, L);
    N_dot_T = VectorDot(N, T);
    Rpp=((ni*N_dot_L)+(nt*N_dot_T))/((ni*N_dot_L)-(nt*N_dot_T));
    Rpl=((nt*N_dot_L)+(ni*N_dot_T))/((nt*N_dot_L)-(ni*N_dot_T));
    return (Rpl*Rpl+Rpp*Rpp)/2.0;
}



