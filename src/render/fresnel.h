/**************************************************************/        
/*                      fresnel.h                             */
/**************************************************************/

/*         
   include file for the micro facet distribution functions
*/

         
#ifndef __FRESNEL_H__
#define __FRESNEL_H__
         
double FresnelDpl(VECTOR *,VECTOR *,VECTOR *,double,double);
double FresnelDpp(VECTOR *,VECTOR *,VECTOR *,double,double);
double FresnelDR(VECTOR *,VECTOR *,VECTOR *,double,double);
double FresnelCpl(VECTOR *,VECTOR *,double,double);
double FresnelCpp(VECTOR *,VECTOR *,double,double);
double FresnelCR(VECTOR *,VECTOR *,double,double);
double FresnelApproxN(double *,int,double *);
double FresnelApproxK(double *,int,double *);
double *FresnelApproxFr(VECTOR *,VECTOR *,double,double,
                        double,int,double *,double *);
double *FresnelApproxFrFt(VECTOR *,VECTOR *,VECTOR *,double,double,
						  double,int,double *,double *,double *);

#endif /* __FRESNEL_H__ */
         
