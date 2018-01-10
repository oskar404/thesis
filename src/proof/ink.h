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
	Ink.h	- Headerfile for Ink.c
*/


#ifndef __INK__
#define __INK__


#include "c_types.h"
#include "vector.h"
#include "color.h"


Logical InkInit(String);
Logical InkExit(void);

double InkPicturePixel(POINT *);
double InkTransfer(POINT *);
double InkGetSpecularBeta(void);
double InkAbsorptionCoefficient(void);

ColorType InkGetSpecular(void);
ColorType InkGetDiffuse(void);
ColorType InkGetAmbient(void);



#endif /* __INK__ */
