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
	Light.h	- Headerfile for Light.c
*/


#ifndef __LIGHT__
#define __LIGHT__


#include "c_types.h"
#include "color.h"
#include "vector.h"


/* Default ligth position */

Logical LightInit(String);
Logical LightExit(void);
ColorType LightSpecColor(void);
ColorType LightAmbientColor(void);
Logical LightVector(VECTOR *,POINT *);


#endif /* __LIGHT__ */

