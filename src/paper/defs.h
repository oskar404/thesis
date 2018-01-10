
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

#define AUTHOR  "Oskar L”nnberg"
#define PROGRAM "Paper Maker"
#define VERSION "0.01"
#define DATE    "24.11.1993"


/* Default extensions */

#define PAPER_EXTENSION ".p"
#define DST_EXTENSION ".dst"


/* Default size for largest number (number of characters) in paper,
   ink and ligth files */

#define STR_SIZE 50
#define NUM_SIZE 10
#define NUM_FORMAT " %.10f"
#define INT_FORMAT " %d"
#define I_SIZE_FORMAT "  %d  %d "
#define D_SIZE_FORMAT "  %.10f  %.10f "


/* Default file names */

#define SCRIPT_FILE "proof.scr"
#define PAPER_FILE "paper.p"
#define NONAME ""


/* Usage string */

#define USAGE_STRING ""


#endif /* __DEFS__ */


