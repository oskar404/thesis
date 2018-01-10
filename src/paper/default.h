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
	default.h	- Default values for creatin paper structure
				  file.
*/


#ifndef __DEFAULT_H__

#define __DEFAULT_H__


#define HEADING_STR "\
#\n\
# This is a file of paper structure\n\
#\n"


#define COMMENT_STR "\
#\n\
# Comments starts always with a character '#'\n\
#\n"


#define EMPTY_COMMENT_STR "#\n#\n"


#define COMMENT "# "


#define PIXEL_SIZE "pixel_size "


#define PIXEL_SIZE_STR "\
#\n\
#\n\
# Pixel size tells the size of the pixels in\n\
#            roughness matrix. Pixel size is\n\
#            in micro meters\n\
#\n"


#define COLOR_STR "\
#\n\
#\n\
# Color is created with ambient diffuse and \n\
#       specular term and with corresponding\n\
#       spectral curves.\n\
#\n"


#define AMBIENT      "Ambient "

#define DIFFUSE      "Diffuse "

#define SPECULAR     "Specular "


#define SPECTRAL_CURVE "\
380 1.0\n\
780 1.0\n"


#define ANGLE_OF_BETA "angle_of_beta "


#define ANGLE_OF_BETA_STR "\
#\n\
#\n\
# Angle of beta matrix tells how the specular\n\
#       beta angle varies spatially.\n\
#\n"


#define CONTACT "contact "


#define CONTACT_STR "\
#\n\
#\n\
# Contact level for paper profile
#  level is calculated from max roughness
#\n"


#define MAX_ROUGHNESS "max_roughness "


#define MAX_ROUGHNESS_STR "\
#\n\
#\n\
# Max rougness tells the maximum possible roughness\n\
#              between 0 and 255 in roughness matrix\n\
#              the value is in micro meters\n\
#\n"



#define ROUGHNESS "roughness "


#define ROUGHNESS_STR "\
#\n\
#\n\
# Rougness matrix contains paper structure variation.\n\
#                 Values are integers between 0 and 255.\n\
#\n"



#endif /* __DEFAULT_H__ */

