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
	Proof.c	- Main program for softproofing
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"
#include "proof.h"
#include "buffer.h"
#include "fileio.h"
#include "picture.h"
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
	if(PictureInit()==FALSE)
		goto error;
	return TRUE;

error:
	MessageError("Could not initialize program");
	return FALSE;
}



/**************************************************************

	static void ExitProcedure(void)

	Exit procedures for program

*/

static void ExitProcedure(void)
{
	PictureExit();
    ExecutionTime();
	MessageExit();
	return;
}



/**************************************************************

	static void ReadArguments(int argc,char **argv)

	Reads arguments and analyzes them

*/

static void ReadArguments(int argc,char **argv)
{
	int c;

	while((c=getopt(argc,argv,OPTIONS))!=EOF) /* This while loop gets     */
		{                                     /* command line options and */
		if(AnalyzeOptions(c)==FALSE)          /* analyzes them            */
			MessageUsage();
		}
	return;
}



/**************************************************************

	static Logical AnalyzeOptions(int option_letter)

	This function analyzes option letters and if there are
	any values it saves them for future use.

*/

static Logical AnalyzeOptions(int option_letter)
{
	switch(option_letter)
		{
        case 'f':       /* read arguments from file */
            ReadOptionsFromFile=TRUE;
            break;
		case 'p':       /* Paper file name */
			PictureChangeName(optarg,PAPER);
			break;
		case 'i':       /* Ink file name */
			PictureChangeName(optarg,INK);
			break;
		case 'I':       /* Ink is used during rendering */
        	PictureUseInk();
			break;
		case 'l':       /* Ligth file name */
			PictureChangeName(optarg,LIGHT);
			break;
		case 'o':
		case 't':       /* Tif file name */
			PictureChangeName(optarg,PICTURE);
			break;
		case 'd':       /* Dot size in micrometers */
			PictureChangeDotSize(atof(optarg));
			break;
		case 'x':       /* Image width in millimeters */
			PictureChangeSize(atof(optarg),-1.0);
			break;
		case 'y':       /* Image length in millimeters */
			PictureChangeSize(-1.0,atof(optarg));
			break;
		case 'P':
			PictureIllumModel(PHONG);
			break;
		case 'B':
			PictureIllumModel(BLINN);
			break;
		case 'V':
			PictureViewDirection(atof(optarg));
			break;
		case 'H':
		case '?':
		dedfault:
			return FALSE;
		}
	return TRUE;
}



/**************************************************************

	static void ExecutionTime(void)

    Prints the time from the beginning of execution

*/

static void ExecutionTime(void)
{
	char buf[100];
	struct tm *time;
	time_t execution_time;

	execution_time=(time_t)(clock()/CLK_TCK);
	time=gmtime(&execution_time);
	strftime(buf,100,"Time used by program: %H:%M:%S",time);
	MessagePrint(buf);
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




