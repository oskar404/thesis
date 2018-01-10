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
	Picture.c	- Module for making a printed picture
*/



#include <string.h>
#include "defs.h"
#include "c_types.h"
#include "buffer.h"
#include "picture.h"
#include "render.h"
#include "paper.h"
#include "ink.h"
#include "light.h"
#include "access.h"
#include "message.h"



/**************************************************************/

/* structure and type definitions */

typedef struct {
		double 	sizeX,sizeY;  /* Size in millimeters */
		double 	DotSize;      /* Resolution, size of pixel (um) */
		double  ViewDirection;/* The view angle */
		int    	PixX,PixY;    /* Size of the picture in pixels */
		int    	IllumModel;   /* Illumination model (PHONG/BLINN) */
		String 	name;         /* Name of the picture file */
		String 	light;        /* Name of the light file */
		String 	paper;        /* Name of the paper file */
		String 	ink;          /* Name of the ink file */
		Logical	UseInk;       /* TRUE if ink is used */
		TIFF   *tif;          /* Pointer to TIFF structure */
		} PictureStruct;


/* Global variables and definitions for this file */

static PictureStruct picture;
static Logical init=FALSE;

#define RESOLUTION 600
#define INCH 25.4       /* Lenght of inch in millimeters */
#define MICROMETER 1000 /* one millimetermeter in micrometers */
#define MAX_VIEW_DIR 90

#ifndef M_PI
#define M_PI 3.141592654
#endif /* M_PI */


/* Internal functions */

static String CheckExtension(String,String);

static Logical InitExtStruct(void);
static void    ExitExtStruct(void);


/**************************************************************/




/**************************************************************

	Logical PictureInit(void)

	Puts default values to picture structure

*/

Logical PictureInit(void)
{
	if(init==TRUE)
		return FALSE;
	init=TRUE;
	picture.sizeX=PICTURE_X_SIZE;
	picture.sizeY=PICTURE_Y_SIZE;
	picture.DotSize=DOT_SIZE;
	picture.ViewDirection=PICTURE_VIEW_DIRECTION;
	picture.IllumModel=PHONG;
	picture.UseInk=FALSE;
	picture.paper=CheckExtension(PAPER_FILE,PAPER_EXTENSION);
	picture.ink=CheckExtension(INK_FILE,INK_EXTENSION);
	picture.light=CheckExtension(LIGHT_FILE,LIGHT_EXTENSION);
	picture.name=CheckExtension(PICTURE_FILE,PICTURE_EXTENSION);;
	picture.tif=NULL;
	return TRUE;
}



/**************************************************************

	Logical PictureExit(void)

	Exits the picture structure

*/

Logical PictureExit(void)
{
	if(init==FALSE)
		return FALSE;
	init=FALSE;
	if(picture.name!=NULL)
		{
		MemoryFree(picture.name);
		picture.name=NULL;
		}
	if(picture.ink!=NULL)
		{
		MemoryFree(picture.ink);
		picture.ink=NULL;
		picture.UseInk=FALSE;
		}
	if(picture.paper!=NULL)
		{
		MemoryFree(picture.paper);
		picture.paper=NULL;
		}
	if(picture.light!=NULL)
		{
		MemoryFree(picture.light);
		picture.light=NULL;
		}
	return TRUE;
}



/**************************************************************

	Logical PictureCreate(void)

	This function creates and renders a picture file.

*/

Logical PictureCreate(void)
{
	double Resolution=RESOLUTION;
	RenderType pic;

	if(init==FALSE)
		return FALSE;
	if(InitExtStruct()!=TRUE)
		goto error;
	Resolution=INCH*MICROMETER/picture.DotSize;
	if((picture.tif=OpenForWriting(picture.name,RGB,picture.PixX,
								   picture.PixY,Resolution))==NULL)
		goto error;

	/* Making of the picture */

	pic.Name=picture.name;
	pic.Tif=picture.tif;
	pic.PixelSize=picture.DotSize;
	pic.X=picture.PixX;
	pic.Y=picture.PixY;
	pic.Model=picture.IllumModel;
	pic.UseInk=picture.UseInk;
	pic.ViewDir=picture.ViewDirection;

	if(RenderImage(pic)==FALSE)
		goto error;


	CloseImage(picture.tif);
	picture.tif=NULL;
	ExitExtStruct();
	return TRUE;

error:
	if(picture.tif!=NULL)
		{
		CloseImage(picture.tif);
		picture.tif=NULL;
		}
	ExitExtStruct();
	MessageWarning("Can't initialize picture");
	return FALSE;
}



/**************************************************************

	Logical PictureChangeName(String name,int mode)

	Saves the file name to picture structure to th correct
	place

*/

Logical PictureChangeName(String name,int mode)
{
	if(init==FALSE)
		return FALSE;
	if(name==NULL || strcmp(name,NONAME)==0)
		return FALSE;
	switch(mode)
		{
		case INK:
			if(picture.ink!=NULL)
				{
				MemoryFree(picture.ink);
				picture.ink=NULL;
				}
			picture.UseInk=TRUE;
			picture.ink=CheckExtension(name,INK_EXTENSION);
			break;
		case PAPER:
			if(picture.paper!=NULL)
				{
				MemoryFree(picture.paper);
				picture.paper=NULL;
				}
			picture.paper=CheckExtension(name,PAPER_EXTENSION);
			break;
		case LIGHT:
			if(picture.light!=NULL)
				{
				MemoryFree(picture.light);
				picture.light=NULL;
				}
			picture.light=CheckExtension(name,LIGHT_EXTENSION);
			break;
		case PICTURE:
			if(picture.name!=NULL)
				{
				MemoryFree(picture.name);
				picture.name=NULL;
				}
			picture.name=CheckExtension(name,PICTURE_EXTENSION);
			break;
		default:
			break;
		}
	return TRUE;
}



/**************************************************************

	Logical PictureChangeDotSize(double size)

	Changes the dot size in picture structure

*/

Logical PictureChangeDotSize(double size)
{
	if(init==FALSE)
		return FALSE;
	if(size<=0.0)
		return FALSE;
	picture.DotSize=size;
	return TRUE;
}



/**************************************************************

	Logical PictureChangeSize(double sX,double sY)

	Changes the picture size in milli meters

*/

Logical PictureChangeSize(double sX,double sY)
{
	if(init==FALSE)
		return FALSE;
	if(sX>0.0)
		picture.sizeX=sX;
	if(sY>0.0)
		picture.sizeY=sY;
	return TRUE;
}



/**************************************************************

	Logical PictureIllumModel(int model)

	Selects the illumination model used by rendering

*/

Logical PictureIllumModel(int model)
{
	if(init==FALSE)
		return FALSE;
	switch(model)
		{
		case PHONG:
			picture.IllumModel=PHONG;
			break;
		case BLINN:
			picture.IllumModel=BLINN;
			break;
		default:
			return FALSE;
		}
	return TRUE;
}



/**************************************************************

	Logical PictureUseInk(void)

	Enables using ink in rendering

*/

Logical PictureUseInk(void)
{
	if(init==FALSE)
		return FALSE;
	picture.UseInk=TRUE;
	return TRUE;
}



/**************************************************************

	Logical PictureViewDirection(double angle)

	the direction of view vector according to picture normal

*/

Logical PictureViewDirection(double angle)
{
	if(init==FALSE)
		return FALSE;
	if(angle>MAX_VIEW_DIR || angle<-MAX_VIEW_DIR)
		return FALSE;
	picture.ViewDirection=angle*M_PI/180;
	return TRUE;
}



/**************************************************************

	double PictureGetDotSize(void)

	retruns the dot size in the structure

*/

double PictureGetDotSize(void)
{
	if(init==FALSE)
		return -1.0;
	return picture.DotSize;
}



/**************************************************************
	Internal functions for this file
***************************************************************/



/**************************************************************

	static Logical InitExtStruct(void)

	Initializes external structures used by picture module

*/

static Logical InitExtStruct(void)
{
	if(ColorInit()==FALSE)
		return FALSE;
	if(PaperInit(picture.paper)==FALSE)
		return FALSE;
	if(picture.UseInk==TRUE)
		if(InkInit(picture.ink)==FALSE)
			return FALSE;
	if(LightInit(picture.light)==FALSE)
		return FALSE;

	/* size of the picture in pixels */
	picture.PixX=(int)(picture.sizeX*(MICROMETER/picture.DotSize));
	picture.PixY=(int)(picture.sizeY*(MICROMETER/picture.DotSize));
	return TRUE;
}



/**************************************************************

	static void ExitExtStruct(void)

	Exits external structures used by picture module

*/

static void ExitExtStruct(void)
{
	PaperExit();
	InkExit();
	LightExit();
	ColorExit();
	return;
}



/**************************************************************

	String CheckExtension(String name,String ext)

	Checks the extension of file name. If there is an extension
	the file name is copied as it is to the picture structure.

*/

static String CheckExtension(String name,String ext)
{
	int i=0;
	char c;
	String str;

	while((c=name[i++])!='.' && c!='\0');
	if(c=='\0')
		{
		i=strlen(name)+strlen(ext)+1;
		str=MemoryAllocate(char,i);
		strcpy(str,name);
		strcat(str,ext);
		}
	else
		{
		i=strlen(name)+1;
		str=MemoryAllocate(char,i);
		strcpy(str,name);
		}
	return str;
}


