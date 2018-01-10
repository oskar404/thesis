/***************************************************************/
/*                         vector.h                            */
/***************************************************************/

/*
   include file for the geometric vector utilities
*/

#ifndef __VECTOR__
#define __VECTOR__


#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */


/* common vector geometric constructs */

typedef struct {double i, j, k;}   		VECTOR;
typedef struct {double x, y, z;}   		POINT;
typedef struct {double x, y, z, h;}    	POINT4;
typedef struct {POINT start;
                VECTOR dir;}      		LINE;


/* geometric manipulation routines */

double	VectorDot(VECTOR *,VECTOR *);		/* vector dot product */
VECTOR	*VectorCross(VECTOR *,VECTOR *);	/* vector cross product */
double	VectorNorm(VECTOR *); 				/* vector normalize */
double	VectorLine(POINT *,POINT *,LINE *);	/* vector between two points */
VECTOR	*VectorReflected(VECTOR *,VECTOR *);
                                            /* reflected vector */
VECTOR 	*VectorRefracted(VECTOR *,VECTOR *,double,double);
                                            /* refracted vector */
VECTOR 	*VectorH(VECTOR *,VECTOR *);     	/* H vector */
VECTOR 	*VectorHt(VECTOR *,VECTOR *,double,double);
											/* H' vector */


#endif /* __VECTOR__ */
