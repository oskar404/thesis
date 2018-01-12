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
    Color.h - Color operations header file
*/



#ifndef __COLOR_H__
#define __COLOR_H__


#include "c_types.h"
#include "buffer.h"


typedef double BasicColorType;
typedef BasicColorType * ColorType;
typedef struct {
    unsigned char r,g,b;
                } RGBType;

Logical ColorInit(void);
void ColorExit(void);
ColorType ColorVectorInit(void);
void ColorVectorExit(ColorType);

Logical ColorReadVector(cBuffer,ColorType,String,int);
int ColorGetSize(void);

RGBType *ColorGetRGB (ColorType);


#endif /* __COLOR_H__ */

