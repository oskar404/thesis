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
	create_c.h	- Headerfile for create_c.c
*/


#ifndef __CREATE_C_H__
#define __CREATE_C_H__


#include "c_types.h"


Logical CreateAmbient(String,double,String);
Logical CreateDiffuse(String,double,String);
Logical CreateSpecular(String,double,double,String);


#endif /* __CREATE_C_H__ */

