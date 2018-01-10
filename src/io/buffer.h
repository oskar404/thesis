
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
	buffer.h	- Headerfile for buffer handling functions 
*/


#ifndef __BUFFER_H__

#define __BUFFER_H__


#include <stdlib.h>


/* Buffer types */

typedef char * cBuffer;


#define BUFFER_SIZE 500


#define MemoryAllocate(t,sz) (t *)malloc(sizeof(t)*sz)
#define MemoryFree(b)        (void)free(b)


#define BufferAllocate(sz)   (char *)malloc(sizeof(char)*sz)
#define BufferFree(b)        (void)free(b)


int BufferReadDouble(cBuffer,double *,int);
int BufferReadInt(cBuffer,int *,int);
int BufferReadWord(cBuffer,char *,int);
int BufferCheckNewline(cBuffer,int);


#endif /* __BUFFER_H__ */

