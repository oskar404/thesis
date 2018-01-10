/**************************************************************/
/* This software was made by Oskar Lönnberg                   */
/*                                                            */
/* Copyright (c) by Oskar Lönnberg                            */
/*                                                            */
/* Permission to use, copy, modify, distribute, and sell this */
/* software and its documentation for any purpose is not      */
/* granted without permission from the copyright owner.       */
/**************************************************************/


/*
	Paper.h	- Headerfile for Paper.c
*/


#ifndef __PAPER__
#define __PAPER__


#include "c_types.h"
#include "color.h"
#include "vector.h"


Logical PaperInit(String);
Logical PaperExit(void);

Logical PaperGetNormalVector(VECTOR *,POINT *);
Logical PaperHiddenPixel(VECTOR *,POINT *,POINT *);
Logical PaperSelfShadow(VECTOR *,POINT *);
Logical PaperContact(POINT *);

ColorType PaperGetSpecular(void);
ColorType PaperGetDiffuse(void);
ColorType PaperGetAmbient(void);

double PaperGetSpecularBeta(POINT *);
double PaperGetSpecularScale(void);

double PaperRoughness(POINT *);


#endif /* __PAPER__ */

