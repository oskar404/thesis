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
	Scale.c	- Main program for scaling
*/


#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "convert.h"
#include "message.h"
#include "getopt.h"



/**************************************************************/

/* structure and type definitions */


/* Global variables and definitions */

String ProgramName;
String ProgramUsage;

static String FileIn;
static String FileOut;

static double Origo;
static double Scale;
static int    IntType=FALSE;

#define OPTIONS "i:o:S:O:I" /* Options which the program understands*/

#define ERROR -1
#define OK 0


/* Internal functions */

static void ReadArguments(int,char **);
static Logical InitProgram(void);
static Logical AnalyzeOptions(int);


/**************************************************************/



/**************************************************************

	int main(int argc,char **argv)

	Program Main function

*/

int main(int argc,char **argv)
{
	ProgramName=argv[0];
	ProgramUsage=USAGE_STRING;
	InitProgram();
	ReadArguments(argc,argv);
	if(ConvertFile(FileIn,FileOut,Origo,Scale,IntType)==FALSE)
		return ERROR;
	return OK;
}



/**************************************************************
	Internal functions
**************************************************************/



/**************************************************************

	static Logical InitProgram()

	Initializes the picture and message modules

*/

static Logical InitProgram()
{
	MessageInit();
	FileIn=INPUT_FILE;
	FileOut=OUTPUT_FILE;
	Origo=ORIGO;
	Scale=SCALE;
	return TRUE;
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
		case 'i':       /* Input file name */
			FileIn=optarg;
			break;
		case 'o':       /* Output file name */
			FileOut=optarg;
			break;
		case 'O':       /* Origo */
			Origo=atof(optarg);
			break;
		case 'S':       /* Scale */
			Scale=atof(optarg);
			break;
		case 'I':       /* Scale */
			IntType=TRUE;
			break;
		case '?':
		dedfault:
			return FALSE;
		}
	return TRUE;
}







