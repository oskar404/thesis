/*****************************************************************/
/*                          integrat.c                           */
/*****************************************************************/

/*
	MODULE PURPOSE:
		This is a numerical integrator to integrate
		an intensity function over an illuminating
		hemisphere.

*/



#include <math.h>
#include "vector.h"



#ifndef M_PI
#define M_PI 3.141592654
#endif /* M_PI */



/*****************************************************************

	double integrate (int incs,double theta_i,double (*func_ptr)())
		incs (in) 		- number of elevation samples
		theta_i (in) 	- incident angle (radians)
		func ptr (in) 	- pointer to the function to be integrate

	Elevation sample rates as low as 16 can be used with
	relatively constant functions. Functions with extreme
	variation may require rates upwards of 256. The radial
	sample rate is 4*incs.

	The function is called with pointers to N (the surface
	normal), L (the light vector at the incident angle given
	by 'theta_i'), and V (the direction for which the
	intensity function is being evaluated). The intensity
	is assumed to be emitted or reflected from a unit
	surface area.

*/

double integrate (int incs,double theta_i,double (*func_ptr)())
{
	double result = 0.0, elevation_ang = 0.0;
	double      riddling, angle_inc, omega, tmp;
	VECTOR  N, L, V;
	int    elev, radial;

	N.i = N.j = 0.0;
	N.k = 1.0;
	L.i = 0.0;
	L.j = sin(theta_i);
	L.k = cos(theta_i);
	angle_inc = M_PI / (2.0 * incs);
	for(elev=incs; --elev>=0; elevation_ang+=angle_inc)
		{
		omega=angle_inc*(cos(elevation_ang)
		               -cos(elevation_ang+angle_inc));
        riddling = 0.0;
        V.k = cos(elevation_ang + (0.5 * angle_inc));
        tmp = sin(elevation_ang + (0.5 * angle_inc));
		for(radial=4*incs; --radial>=0; riddling += angle_inc)
			{
         	V.i = tmp * sin(riddling + (0.5 * angle_inc));
         	V.j = tmp * cos(riddling + (0.5 * angle_inc));
			result += omega * V.k * (*func_ptr)(&N, &L, &V);
            }
        }
	return result;
}
         
