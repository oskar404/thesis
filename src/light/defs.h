
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
	Defs.h	- Headerfile for common definitions for whole program
*/


#ifndef __DEFS__

#define __DEFS__


/* Global defines */

#define AUTHOR "Oskar L”nnberg"
#define PROGRAM "Soft Proofing"
#define VERSION "2.01"
#define DATE "9.11.1993"


/* Handles to different modules */

#define INK 0
#define LIGHT 1
#define PAPER 2
#define PICTURE 3


/* Default file exstensions */

#define PAPER_EXTENSION ".p"
#define INK_EXTENSION ".i"
#define LIGHT_EXTENSION ".l"
#define PICTURE_EXTENSION ".tif"


/* Default size for largest number (number of characters) in paper,
   ink and ligth files */

#define STR_SIZE 20


/* Default values for image */

#define PICTURE_X_SIZE 10  /* Size in millimeters */
#define PICTURE_Y_SIZE 10  /* Size in millimeters */
#define PICTURE_VIEW_DIRECTION 0
#define DOT_SIZE 20        /* In micrometers, dots are square dots */


/* Default file names */

#define SCRIPT_FILE "proof.scr"
#define PAPER_FILE "paper.p"
#define INK_FILE "ink.i"
#define LIGHT_FILE "light.l"
#define PICTURE_FILE "picture.tif"
#define NONAME ""


/* Default values for Paper */

#define PAPER_PIXEL_SIZE 20.0
#define PAPER_AMBIENT_COEFFICIENT 0.20
#define PAPER_DIFFUSE_COEFFICIENT 0.20
#define PAPER_SPECULAR_COEFFICIENT 0.80
#define PAPER_SPECULAR_BETA 0.5
#define PAPER_MAX_ROUGHNESS 10
#define PAPER_CONTACT_LEVEL 250


/* Default values for Ligth */

#define LIGHT_X_POSITION 1000
#define LIGHT_Y_POSITION 1000
#define LIGHT_Z_POSITION 1000
#define LIGHT_INTENSITY 255
#define LIGHT_AMBIENT_INTENSITY 0


/* Default values for Ink */

#define INK_PIXEL_SIZE 20.0
#define INK_SPLITTING 0.5
#define INK_DEPOSITION 0.5
#define INK_ABSORPTION 2
#define INK_AMBIENT_COEFFICIENT 0.10
#define INK_DIFFUSE_COEFFICIENT 0.10
#define INK_SPECULAR_COEFFICIENT 0.90
#define INK_SPECULAR_BETA 0.2
#define INK_LAYER 1


/* Default color values */

#define MAX_WAWE_LENGTH 780
#define MIN_WAWE_LENGTH 380


/* Usage string */

#define USAGE_STRING "\
-H   Prints this help screen\n\
-f   Reads the options from 'proof.scr' file\n\
-x   the x size of picture in millimeters\n\
-y   the y size of picture in millimeters\n\
-d   pixel size in micrometers\n\
-t   picture name in which the tif file is saved\n\
-l   light information file name\n\
-p   paper information file name\n\
-i   ink information file name\n\
-I   use ink information in rendering\n\
-P   Uses the Phong illumination model for rendering\n\
-B   Uses the Blinn illumination model for rendering\n\
-V   Defines the view direction according to picture \n\
	 normal. Default is zero max is 90 degrees."


#endif /* __DEFS__ */


