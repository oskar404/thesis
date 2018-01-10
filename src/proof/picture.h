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
	Picture.h	- Headerfile for Picture.c
*/


#ifndef __PICTURE_H__
#define __PICTURE_H__

#include "c_types.h"


#define PHONG 0
#define BLINN 1


Logical PictureInit(void);
Logical PictureExit(void);

Logical PictureCreate(void);

Logical PictureChangeName(String,int);
Logical PictureChangeDotSize(double);
Logical PictureChangeSize(double,double);
Logical PictureIllumModel(int);
Logical PictureUseInk(void);
Logical	PictureViewDirection(double);

double PictureGetDotSize(void);



#endif /* __PICTURE_H__ */

