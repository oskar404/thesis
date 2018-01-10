/**************************************************************/
/* This software was made by Oskar L�nnberg                   */
/*                                                            */
/* Copyright (c) by Oskar L�nnberg                            */
/*                                                            */
/* Permission to use, copy, modify, distribute, and sell this */
/* software and its documentation for any purpose is not      */
/* granted without permission from the copyright owner.       */
/**************************************************************/


/*
	Message.h	- Headerfile for Message.c 
*/


#ifndef __MESSAGE__
#define __MESSAGE__

#include "c_types.h"

void MessageInit(void);
void MessageExit(void);

void MessagePrint(String);
void MessageNumber(String,int);

void MessageWarning(String);
void MessageWarning2(String,String);
void MessageError(String);

void MessageUsage(void);


#endif /* __MESSAGE__ */
