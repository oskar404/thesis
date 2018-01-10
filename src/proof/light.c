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
	Light.c	- Light structure operations
*/



#include "defs.h"
#include "color.h"
#include "light.h"
#include "buffer.h"
#include "fileio.h"
#include "message.h"



/**************************************************************/

/* structure and type definitions */

typedef struct  {
	double              X,Y,Z;
	double              Intensity;
	double              AmbientIntensity;
	ColorType           Color;
	ColorType           AmbientColor;
				}   LightStruct ;


/* Global variables for this file */

static LightStruct light;
static int init=FALSE;


/* Internal functions */

static Logical InitializeStructure(void);
static void ExitStructure(void);


/**************************************************************/



/**************************************************************

	int lightInit(String lightName)

	Reads the light file and initializes the paper struct

*/

int LightInit(String LightName)
{
	cBuffer buf=NULL;
	int     bSize=BUFFER_SIZE;
	int     size;

	if(init==TRUE)
		goto error;
	if(InitializeStructure()==FALSE)
		goto error;
	if (FileIOOpen(LightName,READ) == FALSE)
		goto error;
	if((buf=BufferAllocate(bSize))==NULL)
		goto error;
	size=ColorGetSize();
	while (FileIOReadLine(LightName,buf,bSize)!=FALSE)
		{
		switch (buf[0])
			{

			case 'X':       /* light position X coordinate */
				if(BufferReadDouble(buf,&light.X,0)<=0)
					goto error;
				break;

			case 'Y':       /* light position Y coordinate */
				if(BufferReadDouble(buf,&light.Y,0)<=0)
					goto error;
				break;

			case 'Z':       /* light position Z coordinate */
				if(BufferReadDouble(buf,&light.Z,0)<=0)
					goto error;
				break;

			case 'c':       /* Color and intensity of light */
				{
				int i;

				if(BufferReadDouble(buf,&light.Intensity,0)<=0)
					goto error;
				if((ColorReadVector(buf,light.Color,
									  LightName,bSize))==FALSE)
					goto error;
				for(i=0;i<size;i++)
					light.Color[i]=light.Color[i]*light.Intensity;
                }
				break;

			case 'a':       /* Color and Intensity of ambient light */
				{
				int i;

				if(BufferReadDouble(buf,&light.AmbientIntensity,0)<=0)
					goto error;
				if((ColorReadVector(buf,light.AmbientColor,
									  LightName,bSize))==FALSE)
					goto error;
				for(i=0;i<size;i++)
					light.AmbientColor[i]=light.AmbientColor[i]*light.AmbientIntensity;
                }
				break;

			case '#':       /* comment */
			case ' ':
			default:        /* Ignore unknown character */
				break;
			}
		}
	init=TRUE;
	(void)FileIOClose(LightName);
	BufferFree(buf);
	return TRUE;

error:
	(void)FileIOClose(LightName);
	if(buf!=NULL)
		BufferFree(buf);
	ExitStructure();
	MessageWarning("Can't initialize light structure");
	return FALSE;
}



/**************************************************************

	Logical LightExit(void)

	Exits the using of light structure

*/

Logical LightExit(void)
{
	if(init==FALSE)
		return FALSE;
	init=FALSE;
	ExitStructure();
	return TRUE;
}



/**************************************************************

	Logical lightVector(VECTOR *Dir,POINT *px)

	Gets the light direction vector of point px

*/

Logical LightVector(VECTOR *Dir,POINT *px)
{
	if(init==FALSE)
		return FALSE;
	Dir->i = light.X - px->x;
	Dir->j = light.Y - px->y;
	Dir->k = light.Z - px->z;
	VectorNorm(Dir);
	return TRUE;
}



/**************************************************************

	ColorType LightColor(void)

	Returns the color vector and NULL if Light structure
	is not defined

*/

ColorType LightSpecColor(void)
{
	if(init==FALSE)
		return NULL;
	return light.Color;
}



/**************************************************************

	ColorType LightAmbientColor(void)

	Returns the color vector of ambient light and NULL if
	Light structure is not defined

*/

ColorType LightAmbientColor(void)
{
	if(init==FALSE)
		return NULL;
	return light.AmbientColor;
}




/**************************************************************
	Internal functions for this file
***************************************************************/



/**************************************************************

	Logical InitializeStructure(void)

	This function puts the default values to light struct.

*/

static Logical InitializeStructure(void)
{
	light.X=LIGHT_X_POSITION;
	light.Y=LIGHT_Y_POSITION;
	light.Z=LIGHT_Z_POSITION;
	if((light.Color=ColorVectorInit())==NULL)
		return FALSE;
	if((light.AmbientColor=ColorVectorInit())==NULL)
		return FALSE;
	light.Intensity=LIGHT_INTENSITY;
	light.AmbientIntensity=LIGHT_AMBIENT_INTENSITY;
	return TRUE;
}



/**************************************************************

	static void ExitStructure(void)

	This function frees memory used by vectors.

*/

static void ExitStructure(void)
{
	ColorVectorExit(light.Color);
	light.Color=NULL;
	ColorVectorExit(light.AmbientColor);
	light.AmbientColor=NULL;
	return;
}
