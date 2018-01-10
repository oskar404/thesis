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
	create_r.c	- Module for handling paper structure
*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "default.h"
#include "create_r.h"
#include "buffer.h"
#include "fileio.h"



/**************************************************************/

/* structure and type and other definitions */


typedef unsigned char RoughType; /* Roughness type defined for  */
								  /* saving space in roughness */
								  /* matrix in PaperStruct. */

typedef struct  {
	int			x,y;
				} IPoint;


typedef struct  {
	double		x,y;
				} DPoint;


typedef struct  {
	RoughType **Rough;
				} PaperStruct ;


#define LOW_VALUE 0      /* Smallest and larges value in Rough matrix */
#define HIGH_VALUE 255

#define BETA_MAX 100
#define ROUGHNESS_MAX 256


/* Global variables for this file */

static PaperStruct paper;


/* Internal functions and macros */

static Logical ReadRoughnessMatrix(cBuffer,int,String);
static Logical AllocateRoughnessMem(void);
static Logical FreeRoughnessMem(void);

static void GetPoint(POINT*,IPoint*);
static void GetBetaPoint(POINT*,IPoint*);

#define GetElement(x,y) ((double)paper.Rough[x][y]*paper.Range/(double)HIGH_VALUE)
#define GetBetaElement(x,y) ((double)paper.Beta[x][y]+(double)paper.SpecularBeta)
#define GetMaxRange() paper.Range
#define Remainder(x) (x-floor(x)) /* Remainder of a division */
#define Round(x) (floor(x+0.5))


/**************************************************************/



/**************************************************************

	Logical CreatePixelSize(String Name,double Val)

	This function creates a Pixel size entry in a file given
	infirst argument.

*/

Logical CreatePixelSize(String Name,double Val)
{
	char buf[BUFFER_SIZE];
	char num[STR_SIZE];

	buf[0]='\0';
	if(FileIOWriteLine(Name,PIXEL_SIZE_STR)==FALSE)
		return FALSE;
	strcpy(buf,PIXEL_SIZE);
	if(sprintf(num,NUM_FORMAT,Val)==0)
    	return FALSE;
	strcat(buf,num);
	BufferCheckNewline(buf,0);
	if(FileIOWriteLine(Name,buf)==FALSE)
		return FALSE;
	return TRUE;
}



/**************************************************************

	Logical CreateContact(String Name,double Val)

	This function creates a contact level entry in a file given
	infirst argument.

*/

Logical CreateContact(String Name,double Val)
{
	char buf[BUFFER_SIZE];
	char num[STR_SIZE];

	buf[0]='\0';
	if(FileIOWriteLine(Name,CONTACT_STR)==FALSE)
		return FALSE;
	strcpy(buf,CONTACT);
	if(sprintf(num,NUM_FORMAT,Val)==0)
    	return FALSE;
	strcat(buf,num);
	BufferCheckNewline(buf,0);
	if(FileIOWriteLine(Name,buf)==FALSE)
		return FALSE;
	return TRUE;
}



/**************************************************************

	Logical CreateMaxRoughness(String Name,double Val)

	This function creates a Max Roughness entry in a file given
	infirst argument.

*/

Logical CreateMaxRoughness(String Name,double Val)
{
	char buf[BUFFER_SIZE];
	char num[STR_SIZE];

	buf[0]='\0';
	if(FileIOWriteLine(Name,MAX_ROUGHNESS_STR)==FALSE)
		return FALSE;
	strcpy(buf,MAX_ROUGHNESS);
	if(sprintf(num,NUM_FORMAT,Val)==0)
    	return FALSE;
	strcat(buf,num);
	BufferCheckNewline(buf,0);
	if(FileIOWriteLine(Name,buf)==FALSE)
		return FALSE;
	return TRUE;
}



/**************************************************************

	Logical CreateRoughness(String Name,int x,int y,
	                        String MFile)

	This function creates a Roughness matrix entry in a file
	given in first argument.

*/

Logical CreateRoughness(String Name,int x,int y,String MFile)
{
	char buf[BUFFER_SIZE];
	char num[STR_SIZE];

	buf[0]='\0';
	if(FileIOWriteLine(Name,ROUGHNESS_STR)==FALSE)
		return FALSE;
	strcpy(buf,ROUGHNESS);
	if(sprintf(num,I_SIZE_FORMAT,x,y)==0)
    	return FALSE;
	strcat(buf,num);
	BufferCheckNewline(buf,0);
	if(FileIOWriteLine(Name,buf)==FALSE)
		return FALSE;
	if(MFile==NULL || strcmp(MFile,NONAME)=0)
		{
		int i,j;

		for(j=0;j<y;j++)
        	{
			buf[0]='\0';
			for(i=0;i<x;i++)
            	{
				if(sprintf(num,INT_FORMAT,(rand()%ROUGHNESS_MAX))==0)
    				return FALSE;
				strcat(buf,num);
                }
			BufferCheckNewline(buf,0);
			if(FileIOWriteLine(Name,buf)==FALSE)
				return FALSE;
            }
		}
	else if(CheckExtension(MFile,DST_EXTENSION)==TRUE)
		{
		if(MakeDSTMatrix(Name,x,y,MFile)==FALSE)
			return FALSE;
        }
	else
    	{
		if(MatrixFromFile(Name,MFile)==FALSE)
			return FALSE;
        }
	return TRUE;
}



/**************************************************************

	Logical CreateAngleOfBeta(String Name,int x,int y,
	                          String MFile)

	This function creates a Angle of beta variation matrix
	entry in a file given in first argument.

*/

Logical CreateAngleOfBeta(String Name,int x,int y,String MFile)
{
	char buf[BUFFER_SIZE];
	char num[STR_SIZE];

	buf[0]='\0';
	if(FileIOWriteLine(Name,ANGLE_OF_BETA_STR)==FALSE)
		return FALSE;
	strcpy(buf,ANGLE_OF_BETA);
	if(sprintf(num,I_SIZE_FORMAT,x,y)==0)
    	return FALSE;
	strcat(buf,num);
	BufferCheckNewline(buf,0);
	if(FileIOWriteLine(Name,buf)==FALSE)
		return FALSE;
	if(MFile==NULL || strcmp(MFile,NONAME)=0)
		{
		int i,j;

		for(j=0;j<y;j++)
        	{
			buf[0]='\0';
			for(i=0;i<x;i++)
            	{
				if(sprintf(num,NUM_FORMAT,(rand()%BETA_MAX)/BETA_MAX)==0)
    				return FALSE;
				strcat(buf,num);
                }
			BufferCheckNewline(buf,0);
			if(FileIOWriteLine(Name,buf)==FALSE)
				return FALSE;
            }
		}
    else
		if(MatrixFromFile(Name,MFile)==FALSE)
			return FALSE;
	return TRUE;
}



/**************************************************************
	Internal functions for this file
***************************************************************/







/**************************************************************

	static Logical MatrixFromFile(String Name,String Matrix)

	This function creates a matrix by reading a sample
	from a file given in the second argument.

*/

static Logical MatrixFromFile(String Name,String Matrix)
{
	cBuffer buf=NULL;
	int     bSize=BUFFER_SIZE;
	int     index=0,i;

	if (FileIOOpen(Matrix,READ) == FALSE)
		goto error;
	if((buf=BufferAllocate(bSize))==NULL)
		goto error;

	while (FileIOReadLine(Matrix,buf,bSize)!=FALSE)
		if(isdigit(buf[0]))
			if(FileIOWriteLine(Name,buf)==FALSE)
    			goto error;

	(void)FileIOClose(Matrix);
	BufferFree(buf);
	return TRUE;

error:
	(void)FileIOClose(Matrix);
	if(buf!=NULL)
		BufferFree(buf);
    MessageWarning2("Can't write matrix",Matrix);
    return FALSE;
}



/**************************************************************

	static Logical MakeDSTMatrix(String Name,int x,int y,
	                             String MFile)

	This function creates a roughness matrix by reading a sample
	vector from a dst file given in arguments.

*/

static Logical MakeDSTMatrix(String Name,int x,int y,String MFile)
{
/*	cBuffer buf=NULL;
	int     bSize=BUFFER_SIZE;
	int     index=0,i;

	if (FileIOOpen(Matrix,READ) == FALSE)
		goto error;
	if((buf=BufferAllocate(bSize))==NULL)
		goto error;

	while (FileIOReadLine(Matrix,buf,bSize)!=FALSE)
		if(isdigit(buf[0]))
			if(FileIOWriteLine(Name,buf)==FALSE)
    			goto error;

	(void)FileIOClose(Matrix);
	BufferFree(buf);*/
	return TRUE;
/*
error:
	(void)FileIOClose(Matrix);
	if(buf!=NULL)
		BufferFree(buf);
    MessageWarning2("Can't write matrix",Matrix);
    return FALSE;*/
}



/**************************************************************

	String CheckExtension(String name,String ext)

	Checks the extension of file name. 

*/

static Logical CheckExtension(String name,String ext)
{
	int i=0,j=0;
	char c;
	String str;

	while((c=name[i++])!='.' && c!='\0');
	if(c=='\0')
		return FALSE;
	else
		{
        i--;
		while(name[i+j]==ext[j] && name[i+j]!='\0' && ext[j]!='\0')
        	j++;
		if(name[i+j]=='\0' && ext[j]=='\0')
			return TRUE;
		}
	return FALSE;
}

