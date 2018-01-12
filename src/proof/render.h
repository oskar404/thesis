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
    Render.h    - Headerfile for Render.c
*/


#ifndef __RENDER__
#define __RENDER__


#include "c_types.h"
#include "access.h"

typedef struct  {
    String      Name;
    TIFF *      Tif;
    double      PixelSize;
    int         X,Y;
    int         Model;
    double      ViewDir;
    Logical     UseInk;
                } RenderType;

Logical RenderImage(RenderType);



#endif /* __RENDER__ */
