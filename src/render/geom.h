/*****************************************************************/
/*                           geom.h                              */
/*****************************************************************/

/*         
	Include file for the geometric attenuation functions
*/
         
#ifndef __MFACET_H__
#define __MFACET_H__

/*
	Microfacet distribution routines
*/

double GeomTorrance(VECTOR *,VECTOR *,VECTOR *,double);
double GeomSancer(VECTOR *,VECTOR *,VECTOR *,double);


#endif /* __MFACET_H__ */
