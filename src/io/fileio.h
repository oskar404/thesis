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
    fileio.h    - Headerfile for fileio.c
*/


#ifndef __FILE_IO__
#define __FILE_IO__


#include "c_types.h"
#include "buffer.h"


typedef char *Mode;


#define READ "r"
#define WRITE "w"
#define APPEND "a"


Logical FileIOOpen(String,Mode);
Logical FileIOClose(String);

Logical FileIOReadLine(String,cBuffer,int);
Logical FileIOWriteLine(String,cBuffer);

#endif /* __FILE_IO__ */
