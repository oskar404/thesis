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
    create_r.h  - Headerfile for create_r.c
*/


#ifndef __CREATE_R_H__
#define __CREATE_R_H__


#include "c_types.h"


typedef unsigned char DSTbuf;


#define DST_MAX_POINTS 1024          /* Max number of points in a file */


Logical DSTReadData(String,DSTbuf *);
DSTbuf *DSTAllocBuf(void);
void DSTFreeBuf(DSTbuf *);

#define DSTElement(buf,i) buf[i]


#endif /* __CREATE_R_H__ */

