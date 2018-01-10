
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
#define PROGRAM "Scaling program"
#define VERSION "1.11"
#define DATE "1.10.1993"


/* Default string size */

#define STR_SIZE 20


/* Default file names */

#define OUTPUT_FILE "paper.p"
#define INPUT_FILE "example.p"


/* Default scaling values */

#define ORIGO 128
#define SCALE 1
/* #define SCALE 0.00390625 */  /* =1/256 */
#define PRECISION 6


/* Usage string */

#define USAGE_STRING "-i   the name of input file\n\
-o   the name of output file\n\
-O   Origo tranformation value\n\
-S   Scaling value\n\
-I   The numbers are converted to integers \n\
-?   This message"


#endif /* __DEFS__ */


