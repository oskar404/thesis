/*****************************************************************/
/*                 illum.h                                       */
/*****************************************************************/

/*
  include file for the illumination models
*/
         
#ifndef __ILLUM_H__
#define __ILLUM_H__

#include "vector.h"
         
/*
   The material structure maintains the description of a material
   interface. An interface between a conductor or dielectric and
   air is characterized by loading the properties of the material
   and an index of refraction of 1 for the outside material. An
   interface between two materials is characterised by generating
   a pseudo-material.
                  
   In filling the material structure, the reflected direction is
   the 'outside' of the material. That is, for an interface
   between air and glass, for example, the the reflected direction
   or 'outside' is air (Ar_spectral = NULL, nr=1.0), and the
   transmitted direction is glass (nt=1.5, etc.)
         
   Individual spectral components of the material are characterised
   by the sampled spectral values and a multiplier to scale these
   values.
         
*/
         
typedef struct {
	double Ka_scale;		/* ambient multiplier */
    double *Ka_spectral; 	/* sampled ambient spectral curve */
    double Kd_scale;       	/* diffuse Multiplier */
	double *Kd_spectral; 	/* sampled diffuse spectral curve */
    double Ks_scale;       	/* specular multipiier */
    double *Ks_spectral; 		/* sampled specular spectral curve */
    double Kt_scale;       	/* transmitted Multiplier */
    double *Kt_spectral; 	/* sampled specular transmitted */
         					/* curve. The Kt properties are */
         					/* used for the Whitted model only */
    double *Ar_spectral;	/* sampled filter spectral curve. */
    double *At_spectral; 	/* sampled filter spectral curve */
							/* These are used in the Hall model */
                            /* only. Filter attenuation is */
                       		/* ignored and a NULL pointer is */
                       		/* specified.      */
    double beta;            /* roughness indicator */
    double (*D)();          /* micro facet distribution */
    double D_coeff;         /* the coefficient for the */
         					/* micro facet distribution function */
         					/* computed from by micro facet */
         					/* distribution init function */
    double (*G)();          /* geometric attenuation function */
	double G_coeff;  		/* the coefficient for the geometric */
                       		/* attenuation function (m_2 for */
                       		/* the Sancer function, unused for */
                       		/* the Torrance-Sparrow function) */
    double Ro; 				/* average reflectance */
    double nr;      		/* index of refraction (incident) */
    double nt; 				/* index of refraction (transmitted) */
    double k;  				/* absorption coefficient */
    int conductor; 			/* flags the specular material as a */
                       		/* conductor */
    int transparent;    	/* flags whether the material is */
                       		/* transparent --- note, composite */
                       		/* materials have a dielectric */
                       		/* specular component but may not */
                       		/* be transparent */
    int r_is_air; 			/* flags that the 'outside' or */
                       		/* reflect side of the interface as */
                       		/* air */
         } ILLUM_MATL;



typedef struct {
	double I_scale;			/* illumination multiplier */
    double *I_spectral;     /* sampled source spectral curve */
    POINT center;           /* center of the light source */
         } ILLUM_LGT;



int IllumInit(int,int (*get_clr_routine)());
double *IllumBouknight(LINE *,LINE *,ILLUM_MATL *,ILLUM_LGT **,double *);
double *IllumPhong(LINE *,LINE *,ILLUM_MATL *,ILLUM_LGT **,double *);
double *IllumBlinn(LINE *,LINE *,ILLUM_MATL *,ILLUM_LGT **,double *);
double *IllumWhitted(LINE *,LINE *,ILLUM_MATL *,ILLUM_LGT **,double *);
double *IllumHall(LINE *,LINE *,ILLUM_MATL *,ILLUM_LGT **,double *);
int IllumExit(void);



#endif /* __ILLUM_H__ */
