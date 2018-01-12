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
    Fileio.c    - Module for handling file io
*/



#include <stdio.h>
#include <string.h>
#include "c_types.h"
#include "fileio.h"
#include "buffer.h"



/**************************************************************/

/* Type definition to this file */

#define FILE_NAME_SIZE 20
#define MODE_STR_SIZE 3

typedef struct node {
        FILE        *fp;
        char        name[FILE_NAME_SIZE];
        char        mode[MODE_STR_SIZE];
        struct node *next;
        } FileStruct;

#define Allocate() MemoryAllocate(FileStruct,1)
#define Free(pt)   MemoryFree(pt)


/* Global variables in this file */

static FileStruct *first=NULL;
static FileStruct *prev=NULL;


/* Internal function prototypes */

static FileStruct *SearchOpenFile(String);


/**************************************************************/



/**************************************************************

    Logical FileIOOpen(String FileName,Mode mode)
        mode argument must be READ, WRITE or APPEND

    This function opens a file for reading or writing.

*/

Logical FileIOOpen(String FileName,Mode mode)
{
    FileStruct *next=first;

    if(SearchOpenFile(FileName)!=NULL)
        return FALSE;
    first=Allocate();
    if(first==NULL)
        return FALSE;
    first->next=next;
    strcpy(first->mode,mode);
    strcpy(first->name,FileName);
    first->fp=fopen(FileName,mode);
    if(first->fp == NULL)
        {
        Free(first);
        first=next;
        return FALSE;
        }
    return TRUE;
}



/**************************************************************

    Logical FileIOClose(String FileName)

    This function closes previously opened file.

*/

Logical FileIOClose(String FileName)
{
    FileStruct *file;

    if((file=SearchOpenFile(FileName))==NULL)
        return FALSE;
    if(file==first)
        prev=first=file->next;
    else
        prev->next=file->next;
    (void)fclose(file->fp);
    Free(file);
    return TRUE;
}



/**************************************************************

    Logical FileIOReadLine(String FileName,cBuffer buf,
                           bSize size)

    This function reads a line to buffer given in arguments

*/

Logical FileIOReadLine(String FileName,cBuffer buf,int size)
{
    FileStruct *file;

    if((file=SearchOpenFile(FileName))==NULL)
        return FALSE;
    if(strcmp(file->mode,READ)==0)
        {
        if(fgets(buf,size,file->fp)==0)
            return FALSE;
        }
    else
        return FALSE;
    return TRUE;
}



/**************************************************************

    Logical FileIOWriteLine(String FileName,cBuffer buf)

    This function writes a line to file

*/

Logical FileIOWriteLine(String FileName,cBuffer buf)
{
    FileStruct *file;

    if((file=SearchOpenFile(FileName))==NULL)
        return FALSE;
    if(strcmp(file->mode,WRITE)==0)
        {
        if(fputs(buf,file->fp)==0)
            return FALSE;
        }
    else
        return FALSE;
    return TRUE;
}



/**************************************************************
    Internal functions for this file
***************************************************************/



/**************************************************************

    static FileStruct *SearchOpenFile(String name)

    Finds the open file corresponding to argument name.

*/

static FileStruct *SearchOpenFile(String name)
{
    FileStruct *file;

    file=prev=first;
    if(first==NULL)
        return NULL;
    while(strcmp(file->name,name)!=0 && file->next!=NULL)
        {
        prev=file;
        file=file->next;
        }
    if(strcmp(file->name,name)==0)
        return file;
    return NULL;
}

