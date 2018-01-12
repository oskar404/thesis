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
    Convert.c   - Module for handling conversion
*/



#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "defs.h"
#include "convert.h"
#include "buffer.h"
#include "fileio.h"
#include "Message.h"



/**************************************************************/

/* structure and type and other definitions */

#define ELEMENT_F "%.6f "
#define ELEMENT_I "%u "


/* Global variables for this file */


/* Internal functions and macros */


/**************************************************************/



/**************************************************************

    int ConvertFile(String FIn,String FOut,double origo
                    ,double scale,int type)

    Converts a file to an other

*/

int ConvertFile(String FIn,String FOut,double origo,double scale,
                int type)
{
    cBuffer buf=NULL,bufTmp=NULL;
    char    str[STR_SIZE];
    int     bSize=BUFFER_SIZE;

    if (FileIOOpen(FIn,READ) == FALSE)
        goto error;
    if (FileIOOpen(FOut,WRITE) == FALSE)
        goto error;
    if((buf=BufferAllocate(bSize))==NULL)
        goto error;
    if((bufTmp=BufferAllocate(bSize))==NULL)
        goto error;
    while (FileIOReadLine(FIn,buf,bSize)!=FALSE)
        {
        if(isdigit(buf[0]) || buf[0]=='-' || buf[0]=='.')
            {
            int index=0,ind2=0,i;
            double dTmp;

            while((index=BufferReadDouble(buf,&dTmp,index))>=0)
                {
                dTmp=(dTmp+origo)*scale;
                (type==FALSE?sprintf(str,ELEMENT_F,dTmp):
                             sprintf(str,ELEMENT_I,(unsigned int)dTmp));
                i=0;
                while(str[i]!='\0')
                    bufTmp[ind2++]=str[i++];
                }
            bufTmp[ind2++]='\n';
            bufTmp[ind2]='\0';
            if(FileIOWriteLine(FOut,bufTmp)==FALSE)
                goto error;
            }
        else
            if(FileIOWriteLine(FOut,buf)==FALSE)
                goto error;
        }
    return TRUE;

    /* No roughness matrix has been read */

error:
    (void)FileIOClose(FIn);
    (void)FileIOClose(FOut);
    if(buf!=NULL)
        BufferFree(buf);
    if(bufTmp!=NULL)
        BufferFree(bufTmp);
    MessageWarning("Can't convert file");
    return FALSE;
}



/**************************************************************
    Internal functions for this file
***************************************************************/


