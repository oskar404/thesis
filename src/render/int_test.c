/*****************************************************************/
/*                        int_test.c                             */
/*****************************************************************/

/*

    integrator test - when run this should produce
        integral is 3.142539 (32 incs)
        integral is 3.141829 (64 incs)
        integral is 3.141652 (128 incs)

*/



#include <stdio.h>
#include "vector.h"



double ident();



main ()
{
    double      integrate();

    printf ("integral is %f (32 incs)",integrate(32,0.0,ident));
    printf ("integral is %f (64 incs)",integrate(64,0.0,ident));
    printf ("integral is %f (128 incs)",integrate(128,0.0,ident));
    return;
}



double ident (VECTOR *N,VECTOR *L,VECTOR *V)
{
    return 1.0;
}