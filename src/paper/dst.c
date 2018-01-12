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
    dst.c   - Module handles ds (Digital Storage) files
*/


#include <stdio.h>
#include "dst.h"
#include "buffer.h"
#include "message.h"



#define RUBBISH 128



/**************************************************************

    Logical DSTReadData(String name,DSTbuf *buf)

    Read data from DST file to vector 'buf'

*/

Logical DSTReadData(String name,DSTbuf *buf)
{
    int i;
    unsigned char c[1];    /* character to read */
    unsigned char rubbish[RUBBISH];
    FILE *fp;

    if ((fp = fopen (name, "rb")) == NULL)
        goto error;

    /* Read 128 characters of useless information from file */

    if ( fread(rubbish,sizeof(unsigned char),RUBBISH,fp) !=  128)
        goto error;

    for (i=0; i<DST_MAX_POINTS; i++)
        {
        if(fread(c,sizeof(unsigned char),1,fp) != 1)
            goto error;
        buf[i]=c[0];
        }
    fclose (fp);
    return TRUE;

error:
    if(fp!=NULL)
        fclose(fp);
    MessageWarning2("Unsuccessful reading from file",name);
    return FALSE;
}



/**************************************************************

    DSTbuf *DSTAllocBuf(void)

    Allocate memory for a vector

*/

DSTbuf *DSTAllocBuf(void)
{
    return MemoryAllocate(DSTbuf,DST_MAX_POINTS);
}



/**************************************************************

    void DSTFreeBuf(DSTbuf *buf)

    Free memory allocated to vector 'buf'

*/

void DSTFreeBuf(DSTbuf *buf)
{
    MemoryFree(buf);
    return;
}





