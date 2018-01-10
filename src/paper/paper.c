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
	paper.c	- Main program for paper structure creation program
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"
#include "proof.h"
#include "buffer.h"
#include "fileio.h"
#include "message.h"
#include "getopt.h"



/**************************************************************/

/* structure and type definitions */


/* Global variables and definitions */

String ProgramName;
String ProgramUsage;

static Logical ReadOptionsFromFile=FALSE;

/* Options which the program understands*/
#define OPTIONS "fp:i:Il:t:o:x:y:d:PB?HV:"

#define ERROR -1
#define OK 0


/* Internal functions */

static Logical InitProcedure(void);
static void    ExitProcedure(void);

static void    VersionPrint(void);
static void    ExecutionTime(void);

static void    ReadArguments(int,char **);
static Logical AnalyzeOptions(int);

static void    CreateManyPictures(void);


/**************************************************************/



/**************************************************************

	int main(int argc,char **argv)

	Program Main function

*/

int main(int argc,char **argv)
{
	ProgramName=argv[0];
	ProgramUsage=USAGE_STRING;
	if(InitProcedure()==FALSE)
		return ERROR;
	ReadArguments(argc,argv);
	if(ReadOptionsFromFile==FALSE)
		PictureCreate();
	else
		CreateManyPictures();
	ExitProcedure();
	return OK;
}



/**************************************************************
	Internal functions
**************************************************************/



/**************************************************************

	static Logical InitProgram()

	Initializes the picture and message modules

*/

static Logical InitProcedure(void)
{
	MessageInit();
	VersionPrint();
	return TRUE;
}



/**************************************************************

	static void ExitProcedure(void)

	Exit procedures for program

*/

static void ExitProcedure(void)
{
	MessageExit();
	return;
}



/**************************************************************

    static void VersionPrint(void)
    
    Prints the name and version of Program

*/

static void VersionPrint(void)
{
    char buf[256];

    strcpy(buf,PROGRAM);
    strcat(buf," version ");
    strcat(buf,VERSION);
    strcat(buf,", ");
    strcat(buf,DATE);
    MessagePrint(buf);
    return;
}



/**************************************************************

	static void CreateManyPictures(void)
    
	Reads arguments from a script file and creates many
	pictures.

*/

static void CreateManyPictures(void)
{
	int     argc;
	char   *argv[20];
	cBuffer buf=NULL;
	int     bSize=BUFFER_SIZE;
	int     prev_i,i;

	argv[0]=ProgramName;
	if (FileIOOpen(SCRIPT_FILE,READ) == FALSE)
		goto error;
	if((buf=BufferAllocate(bSize))==NULL)
		goto error;
	while (FileIOReadLine(SCRIPT_FILE,buf,bSize)!=FALSE)
		{
		switch (buf[0])
			{
			case '#':       /* comment */
				break;
			default:
				prev_i=i=0;
				optind=1;   /* getopt() option indicator */
				argc=1;
				while(buf[i]!='\n')
					{
					while(buf[i]==' ' || buf[i]=='\t')
						buf[i++]='\0';
					prev_i=i;
					while(buf[i]!=' ' && buf[i]!='\t' && buf[i]!='\n')
						i++;
					if(buf[i]!='\n')
						buf[i++]='\0';
					argv[argc]=buf+prev_i;
					argc++;
					}
				buf[i]='\0';
				ReadArguments(argc,argv);
                PictureCreate();
				break;
			}
		}
	(void)FileIOClose(SCRIPT_FILE);
    BufferFree(buf);
    return;

error:
	(void)FileIOClose(SCRIPT_FILE);
	if(buf!=NULL)
		BufferFree(buf);
    MessageWarning("Error in script file handling");
    return;
}




