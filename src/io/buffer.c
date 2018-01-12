
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
    buffer.c  - Buffer handling functions and macros
*/



#include <stdlib.h>
#include <ctype.h>
#include "buffer.h"



#define STR_SIZE 20




/**************************************************************

    int BufferReadDouble(cBuffer buf,double *value,int index)

    Read a double from a character buffer starting from point
    indicated by argument index. The value read from the buffer
    is stored to the variable pointed by argument value.

    The buffer has to end to character '\0' or '\n' otherwise
    will happen overflow. All spaces and words consisting of
    normal letters will be read before reading the actual number.
    After one number has been found the scanning stops.

    If no number have been found the function returns negative
    value otherwise the return value is indicating the index
    of next point after the number.

*/

int BufferReadDouble(cBuffer buf,double *value,int index)
{
    int i=0;
    char str[STR_SIZE],c;

    while((c=buf[index++])==' ' || c=='\t' || c=='_' || isalpha(c));
    if(c=='\n' || c=='\0')
        return -1;   /* No numbers in this buffer anymore */
    if(c=='-')
        {      /* Read sign of the number */
        str[i++]=c;
        c=buf[index++];
        }
    while(isdigit(c))
        {      /* Read digits */
        str[i++]=c;
        c=buf[index++];
        }
    if(c=='.')
        {      /* Read point */
        str[i++]=c;
        c=buf[index++];
        while(isdigit(c))
            {  /* Read digits after the point */
            str[i++]=c;
            c=buf[index++];
            }
        }
    str[i]='\0';
    *value=atof(str);
    return index;
}



/**************************************************************

    int BufferReadInt(cBuffer buf,int *value,int index)

    Read an integer from a character buffer starting from point
    indicated by argument index. The value read from the buffer
    is stored to the variable pointed by argument value.

    The buffer has to end to character '\0' or '\n' otherwise
    will happen overflow. All spaces and words consisting of
    normal letters will be read before reading the actual number.
    After one number has been found the scanning stops.

    If no number have been found the function returns negative
    value otherwise the return value is indicating the index
    of next point after the number.

*/

int BufferReadInt(cBuffer buf,int *value,int index)
{
    int i=0;
    char str[STR_SIZE],c;

    while((c=buf[index++])==' ' || c=='\t' || c=='_' || isalpha(c));
    if(c=='\n' || c=='\0')
        return -1;   /* No numbers in this buffer anymore */
    if(c=='-')
        {      /* Read sign of the number */
        str[i++]=c;
        c=buf[index++];
        }
    while(isdigit(c))
        {      /* Read digits */
        str[i++]=c;
        c=buf[index++];
        }
    str[i]='\0';
    *value=atoi(str);
    return index;
}



/**************************************************************

    int BufferReadWord(cBuffer buf,char *str,int index)

    Read a word from a character buffer starting from point
    indicated by argument index. The value read from the buffer
    is stored to the variable pointed by argument str.

    The buffer has to end to character '\0' or '\n' otherwise
    will happen overflow.

    If no word have been found the function returns negative
    value otherwise the return value is indicating the index
    of next point after the number.

*/

int BufferReadWord(cBuffer buf,char *str,int index)
{
    int i=0;
    char c;

    while((c=buf[index++])==' ' || c=='\t');
    if(c=='\n' || c=='\0')
        return -1;   /* No words in this buffer anymore */
    str[i++]=c;
    while((c=buf[index++])!=' ' && c!='\t' && c!='\n' && c!='\0')
        str[i++]=c;
    str[i]='\0';
    return index;
}



/**************************************************************

    int BufferCheckNewline(cBuffer buf,int index)

    This function searches for a newline character from
    the buffer 'buf'. If no newline is found the
    function puts a newline character to the end of
    buffer. The place of next character of newline
    is return.

    The buffer has to end to character '\0' otherwise
    will happen overflow.

*/

int BufferCheckNewline(cBuffer buf,int index)
{
    int i=0;
    char c;

    while((c=buf[index++])!='\n' || c!='\0');
    if(c=='\n')
        return index;   /* Newline found */
    buf[index-1]='\n';
    buf[index]='\0';
    return index;
}



