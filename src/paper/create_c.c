/**************************************************************/
/* This software was made by Oskar L”nnberg                   */
/*                                                            */
/* Copyright (c) by Oskar L”nnberg                            */
/*                                                            */
/* Permission to use, copy, modify, distribute, and sell this */
/* software and its documentation for any purpose is not      */
/* granted without permission from the copyright owner.       */
/**************************************************************/


/*
    create_c.c  - Module for handling paper structure
                  color creation
*/



#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "default.h"
#include "create_c.h"
#include "buffer.h"
#include "fileio.h"
#include "message.h"



/**************************************************************/

/* structure and type and other definitions */



/* Global variables for this file */



/* Internal functions and macros */

static Logical WriteColorCurve(String,String);


/**************************************************************/



/**************************************************************

    Logical CreateAmbient(String Name,double Val,
                          String SpectFile)

    This function creates a Ambient entry in a file given in
    first argument.

*/

Logical CreateAmbient(String Name,double Val,String SpectFile)
{
    char buf[BUFFER_SIZE];
    char num[STR_SIZE];

    buf[0]='\0';

    if(FileIOWriteLine(Name,COLOR_STR)==FALSE)
        return FALSE;

    strcpy(buf,AMBIENT);
    if(sprintf(num,NUM_FORMAT,Val)==0)
        return FALSE;
    strcat(buf,num);
    BufferCheckNewline(buf,0);
    if(FileIOWriteLine(Name,buf)==FALSE)
        return FALSE;

    if(SpectFile==NULL || strcmp(SpectFile,NONAME)=0)
        {
        if(FileIOWriteLine(Name,SPECTRAL_CURVE)==FALSE)
            return FALSE;
        }
    else
        if(WriteColorCurve(Name,SpectFile)==FALSE)
            return FALSE;
    return TRUE;
}



/**************************************************************

    Logical CreateDiffuse(String Name,double Val,
                          String SpectFile)

    This function creates a Diffuse entry in a file given in
    first argument.

*/

Logical CreateDiffuse(String Name,double Val,String SpectFile)
{
    char buf[BUFFER_SIZE];
    char num[STR_SIZE];

    buf[0]='\0';
    if(FileIOWriteLine(Name,COLOR_STR)==FALSE)
        return FALSE;

    strcpy(buf,DIFFUSE);
    if(sprintf(num,NUM_FORMAT,Val)==0)
        return FALSE;
    strcat(buf,num);
    BufferCheckNewline(buf,0);
    if(FileIOWriteLine(Name,buf)==FALSE)
        return FALSE;

    if(SpectFile==NULL || strcmp(SpectFile,NONAME)=0)
        {
        if(FileIOWriteLine(Name,SPECTRAL_CURVE)==FALSE)
            return FALSE;
        }
    else
        if(WriteColorCurve(Name,SpectFile)==FALSE)
            return FALSE;
    return TRUE;
}



/**************************************************************

    Logical CreateSpecular(String Name,double Val,double Beta,
                           String SpectFile)

    This function creates a Specular entry in a file given in
    first argument.

*/

Logical CreateSpecular(String Name,double Val,double Beta,
                       String SpectFile)
{
    char buf[BUFFER_SIZE];
    char num[STR_SIZE];

    buf[0]='\0';

    if(FileIOWriteLine(Name,COLOR_STR)==FALSE)
        return FALSE;

    strcpy(buf,SPECULAR);
    if(sprintf(num,NUM_FORMAT,Val)==0)
        return FALSE;
    strcat(buf,num);
    if(sprintf(num,NUM_FORMAT,Beta)==0)
        return FALSE;
    strcat(buf,num);
    BufferCheckNewline(buf,0);
    if(FileIOWriteLine(Name,buf)==FALSE)
        return FALSE;

    if(SpectFile==NULL || strcmp(SpectFile,NONAME)=0)
        {
        if(FileIOWriteLine(Name,SPECTRAL_CURVE)==FALSE)
            return FALSE;
        }
    else
        if(WriteColorCurve(Name,SpectFile)==FALSE)
            return FALSE;
    return TRUE;
}



/**************************************************************
    Internal functions for this file
***************************************************************/



/**************************************************************

    static Logical WriteColorCurve(String Name,String Curve)

    This function creates a spectral curve by reading a sample
    from a file given in the second argument.

*/

static Logical WriteColorCurve(String Name,String Curve)
{
    cBuffer buf=NULL;
    int     bSize=BUFFER_SIZE;
    int     index=0,i;

    if (FileIOOpen(Curve,READ) == FALSE)
        goto error;
    if((buf=BufferAllocate(bSize))==NULL)
        goto error;

    while (FileIOReadLine(Curve,buf,bSize)!=FALSE)
        if(isdigit(buf[0]))
            if(FileIOWriteLine(Name,buf)==FALSE)
                goto error;

    (void)FileIOClose(Curve);
    BufferFree(buf);
    return TRUE;

error:
    (void)FileIOClose(Curve);
    if(buf!=NULL)
        BufferFree(buf);
    MessageWarning2("Can't write spectral curve",Curve);
    return FALSE;
}




