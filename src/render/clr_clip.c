/*****************************************************************/
/*                         clr_clip.c                            */
/*****************************************************************/

/*
    MODULE PURPOSE:
        This module contains routines for bringing colors outside
        the displayable gamut into the displayable gamut.

    MODULE CONTENTS:
        ClrClampRGB     - clamps rgb values to 0.0-1.0 range
        ClrScaleRGB     - scales rgb values to 0.0-1.0 range
        ClrClipRGB         - clips rgb values to 0.0-1.0 range
*/



#include <stdio.h>
#include <math.h>
#include "clr.h"



/*
    define the max and min scaling values for equal intensity
    clipping. These are defined to be slightly inside of the
    0 to 1 range to avoid numerical problems that occur when
    colors are on or very close to the clipping boundary.
*/

#define MAX_CLIP    0.9999  /* 1 - MAX_CLIP > 1 res step */
#define MIN_CLIP    0.0001  /* less than 1 res step */



/*****************************************************************

    CLR_RGB ClrClampRGB (CLR_RGB in_rgb)
        in_rgb (in) - the input rgb

    Returns clamped color. Values greater than 1 are clamped to 1,
    values less than 0 are clamped to 0.

*/

CLR_RGB ClrClampRGB (CLR_RGB in_rgb)
{
    CLR_RGB rgb;

    rgb = in_rgb;
    if(rgb.r < 0.0)
        rgb.r = 0.0;
    else if (rgb.r > 1.0)
        rgb.r = 1.0;
    if (rgb.g < 0.0)
        rgb.g = 0.0;
    else if (rgb.g > 1.0)
        rgb.g = 1.0;
    if (rgb.b < 0.0)
        rgb.b = 0.0;
    else if (rgb.b > 1.0)
        rgb.b = 1.0;
    return rgb;
}



/*****************************************************************

    CLR_RGB ClrScaleRGB (CLR_RGB in_rgb)
        in_rgb (in) - the input rgb

    Returns scaled color. Values less than 0 are clamped to zero.
    If any values are greater than 1, all color components are
    scaled so the offending value is equal to 1.

*/

CLR_RGB ClrScaleRGB (CLR_RGB in_rgb)
{
    double      scale=1.0;
    double      tmp_scale;
    CLR_RGB rgb;

    rgb = in_rgb;
    if (rgb.r < 0.0)
        rgb.r = 0.0;
    else if((rgb.r>1.0)&&((tmp_scale=(1.0/rgb.r))<scale))
        scale = tmp_scale;
    if (rgb.g < 0.0)
        rgb.g = 0.0;
    else if((rgb.g>1.0)&&((tmp_scale=(1.0/rgb.g))<scale))
         scale = tmp_scale;
    if (rgb.b < 0.0)
        rgb.b = 0.0;
    else if((rgb.b>1.0)&&((tmp_scale=(1.0/rgb.b))<scale))
         scale = tmp_scale;
    if (scale < 1.0)
        {
        rgb.r *= scale;
        rgb.g *= scale;
        rgb.b *= scale;
        }
    return rgb;
}



/*****************************************************************

    CLR_RGB CrlClipRGB(CLR_RGB in_rgb)
        in_rgb (in) - the input rgb

    Returns the clipped color - clipping is by desaturating the
    color by shifting it towards the neutral axis in a plane
    perpendicular to the neutral axis. The neutral axis is taken
    as the vector from the monitor black to the monitor white.

*/

CLR_RGB ClrClipRGB(CLR_RGB in_rgb)
{
    CLR_RGB diff, out_rgb;
    double  axis_rgb, diff_mult, tmp_mult;

    /*
        check to see if clipping is required
    */

    if ((in_rgb.r < 0.0) || (in_rgb.r > 1.0) ||
           (in_rgb.g < 0.0) || (in_rgb.g > 1.0) ||
           (in_rgb.b < 0.0) || (in_rgb.b > 1.0))
        {
    /*
        clipping is required, determine the distance from
        the origin to the equal intensity plane containing
        the color. The distance is normalized the origin
        at color (0,0,0) and a distance of 1 at (1,1,1)
    */

        axis_rgb = (in_rgb.r + in_rgb.g + in_rgb.b) * 0.333333;

    /*
        check for the intensity plane of the color being
        outside the displayable range -- if it is, set
        color to either black or white.
    */

        if (axis_rgb <= MIN_CLIP)

        /*
            this is not a visible color -- it should not
            have been computed
        */

            out_rgb.r = out_rgb.g = out_rgb.b = 0.0;
        else if (axis_rgb >= MAX_CLIP)

        /*
            This is way beyond white in intensity, set it
            to the white point.
        */
             out_rgb.r = out_rgb.g = out_rgb.b = 1.0;
        else
            {

        /*
            the intensity plane is within the displayable
             range. Compute the vector from the neutral
             axis to the color on it's intensity plane.
             The intersection of the neutral axis and the
             intensity plane is at r=g=b=axis_rgb.
        */

             diff.r = in_rgb.r - axis_rgb;
             diff.g = in_rgb.g - axis_rgb;
             diff.b = in_rgb.b - axis_rgb;

        /*
            determine the relative length of the vector
             to the edge of the displayable color gamut.
        */

             diff_mult = 1.0;
            if (in_rgb.r > 1.0)
                {
                if((tmp_mult=(MAX_CLIP-axis_rgb)/diff.r)<diff_mult)
                    diff_mult = tmp_mult;
                }
            else if (in_rgb.r < 0.0)
                {
                if((tmp_mult=(MIN_CLIP-axis_rgb)/diff.r)<diff_mult)
                    diff_mult = tmp_mult;
                }

            if (in_rgb.g > 1.0)
                {
                if((tmp_mult=(MAX_CLIP-axis_rgb)/diff.g)<diff_mult)
                    diff_mult = tmp_mult;
                }
            else if (in_rgb.g < 0.0)
                {
                if ((tmp_mult=(MIN_CLIP-axis_rgb)/diff.g)<diff_mult)
                    diff_mult = tmp_mult;
                 }
            if (in_rgb.b > 1.0)
                {
                if((tmp_mult=(MAX_CLIP-axis_rgb)/diff.b)<diff_mult)
                    diff_mult = tmp_mult;
                 }
            else if (in_rgb.b < 0.0)
                {
                if((tmp_mult=(MIN_CLIP-axis_rgb)/diff.b)<diff_mult)
                    diff_mult = tmp_mult;
                }

        /*
             determine the location of the color at the
            edge of the displayable color gamut.
        */

             out_rgb.r = axis_rgb + (diff.r * diff_mult);
             out_rgb.g = axis_rgb + (diff.g * diff_mult);
             out_rgb.b = axis_rgb + (diff.b * diff_mult);
            }
        return out_rgb;
        }
    else
        return in_rgb;
}






