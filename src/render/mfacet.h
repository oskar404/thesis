
/********************************************************/ 
/*                       mfacet.h                       */                                 
/********************************************************/

/*         
  include file for the micro facet distribution functions
*/

         
#ifndef __MFACET_H__
#define __MFACET_H__
         
/* Microfacet distribution routines */
         
double MFacetPhongInit(double);
double MFacetPhong(VECTOR *,VECTOR *,VECTOR *,double);
double MFacetBlinnInit(double);
double MFacetBlinn(VECTOR *,VECTOR *,VECTOR *,double);
double MFacetGaussianInit(double);
double MFacetGaussian(VECTOR *,VECTOR *,VECTOR *,double);
double MFacetReitzInit(double);
double MFacetReitz(VECTOR *,VECTOR *,VECTOR *,double);
double MFacetCookInit(double);
double MFacetCook(VECTOR *,VECTOR *,VECTOR *,double);
double MFacetG(VECTOR *,VECTOR *,VECTOR *,double,double);
double MFacetTau(double,double);
double MFacetSigma(double,double);
double MFacetM(double,double);
double MFacetCoherent(double);
double MFacetIncoherent(VECTOR *,VECTOR *,VECTOR *,
						double,double,double,double);
double MFacetVxz(VECTOR *,VECTOR *,VECTOR *,double);


#endif /* __MFACET_H__ */
      

	   