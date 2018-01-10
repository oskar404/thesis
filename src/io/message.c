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
	Message.c	- Handles screen messages
*/


#include <stdio.h>
#include <stdlib.h>
#include "message.h"


extern String ProgramName;
extern String ProgramUsage;


static FILE           *fperr;
static FILE           *fpout;



/* Initializes error stream */

void MessageInit(void)
{
	fperr = stderr;
    fpout = stdout;
	return;
}



/* Exit Message Structure */

void MessageExit(void)
{
	fperr=NULL;
    fpout=NULL;
	return;
}



/* Prints a message to fperr */

void MessagePrint(String str)
{
    fprintf(fpout, "%s\n",str);
	return;
}



/* Prints a message with a integer to fperr */

void MessageNumber(String str,int numb)
{
    fprintf(fpout, "%s %d\n",str,numb);
	return;
}



/* Prints warning message to fperr stream */

void MessageWarning(String str)
{
	fprintf(fperr, "%s: %s\n",ProgramName,str);
    return;
}



/* Prints warning message to fperr stream */

void MessageWarning2(String str1,String str2)
{
	fprintf(fperr, "%s: %s %s\n",ProgramName,str1,str2);
    return;
}



/* Prints error message to fperr stream */

void MessageError(String str)
{
	fprintf(fperr, "%s: %s\n",ProgramName,str);
	exit(-1);
}



/* Prints usage information to fperr stream */

void MessageUsage(void)
{
	fprintf(fperr, "Usage: %s [options]\n",ProgramName);
	fprintf(fperr, "%s\n",ProgramUsage);
	exit(-1);
}

