/*
	Access.c contains functions to help using TIFF image.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "access.h"


#define BITSPERSAMPLE			8
#define ROWSPERSTRIP            8
#define RESOLUTIONUNIT			2

typedef struct {                   /* Control structure for TIFF files. */
               TIFF *tif;
               int type;
               } control_t;

static control_t output={NULL,0};
static control_t input={NULL,0};
static u_short CompressionFlag=COMPRESSION_NONE;



void InitTIFF(int,TIFF *,u_long,u_long,double); /* Initializes tif image for writing */
int CheckImage(TIFF *);                    /* Checks if the TIFF file is RGB or CMYK */



/* ------------------------------------------------------------------- */
/* Functions for opening and closing RGB and CMYK tiff images          */
/* ------------------------------------------------------------------- */


/* Open a tiff image. As arguments this function takes the name of the
   file and opening mode (READ or WRITE). If the opening mode is WRITE
   the function expects to find the width, length and resolution of the
   new file on the argument line. The data types are u_long, u_long and
   double. */

TIFF *OpenImage(char *name,char *mode,...)
{
	TIFF *tif;
	va_list ap;

	tif = TIFFOpen(name,mode);
	if(!tif)
		Error("Can't open file: %s\n",name,tif,(TIFF *)0);
	if(strcmp(mode,WRITE)==0)
		{
		u_long width,length;  /* If mode is WRITE search for file type */
		double resolution;    /* width, length and resolution of the file */
        int filetype;

		va_start(ap,mode);
        filetype=va_arg(ap,int);
		width=va_arg(ap,u_long);
		length=va_arg(ap,u_long);
		resolution=va_arg(ap,double);
		InitTIFF(filetype,tif,width,length,resolution);
		va_end(ap);
        output.tif=tif;
        output.type=filetype;
		}
	else
        {
		input.type=CheckImage(tif);
        input.tif=tif;
        }
	return tif;
}



/* This function closes tiff image. The only argument is a pointer
   to open TIFF structure. */

void CloseImage(TIFF *tif)
{
	TIFFClose(tif);
	return;
}



/* Function GetImageSize returns a pointer to a structure in which
   are the size of the the TIFF image structure and the resolution */

ImageSize *GetImageSize(TIFF *tif)
{
	static ImageSize TifImageSize;
	u_long imagelength;
    u_long imagewidth;
    float resolution;

	if(!TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&imagelength))
		return NULL;
	if(!TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imagewidth))
		return NULL;
	if(!TIFFGetField(tif,TIFFTAG_XRESOLUTION,&resolution))
		return NULL;
	TifImageSize.width=imagewidth;
	TifImageSize.length=imagelength;
	TifImageSize.resolution=resolution;
	return &TifImageSize;
}



/* ------------------------------------------------------------------- */
/* Functions for handling row buffers                                  */
/* ------------------------------------------------------------------- */


/* Allocate memory for a row buffer. */

buffer_t AllocRowBuffer(TIFF *tif)
{
	buffer_t buffer;
	u_long i;

	buffer=(buffer_t) malloc(TIFFScanlineSize(tif));
	if(buffer==NULL)
		Error("Can't allocate memory for row buffer%s\n","",tif,(TIFF *)0);
	for(i=0;buffer[i]!='\0';i++)
		buffer[i]=(value_t) 255;
	return buffer;
}



/* This function writes a scanline to TIFF image structure. The
   arguments for this function are pointer to a open TIFF structure,
   pointer to a buffer, row number. The last argument in fuction call
   TIFFWriteScanLine is color plane which is used when the colors are
   saved separately not RGBRGBRGBRGB... or CMYKCMYK... .*/

logical WriteRowBuffer(TIFF *tif,buffer_t buffer,u_long row)
{
	if(TIFFWriteScanline(tif,buffer,row,(int) 0)!=1)
		return FALSE;
	return TRUE;
}



/* This function reads a scanline to a buffer of type buffer_t. The
   arguments for this function are pointer to a open TIFF structure,
   pointer to a buffer and row number. The last argument in fuction call
   TIFFWriteScanLine is color plane which is used when the colors are
   saved separately not RGBRGBRGBRGB... or CMYKCMYK... . */

logical ReadRowBuffer(TIFF *tif,buffer_t buffer,u_long row)
{
	if(TIFFReadScanline(tif,buffer,row,(int) 0)!=1)
		return FALSE;
	return TRUE;
}



/* Free memory used in row buffer. */

void FreeRowBuffer(buffer_t buffer)
{
	free(buffer);
	return;
}



/* ------------------------------------------------------------------- */
/* Other useful functions                                              */
/* ------------------------------------------------------------------- */

/* Gets the type of the TIFF image. RGB or CMYK */

int GetTifType(TIFF *tif)
{
    if(output.tif==tif)
        return output.type;
    if(input.tif==tif)
        return input.type;
    return (int) -1;
}



/* Sets the compression scheme for the Output TIFF image. */

void SetCompression(int type)
{
    CompressionFlag=type;
    return;
}



/* ------------------------------------------------------------------- */
/* Functions for error handling                                        */
/* ------------------------------------------------------------------- */

/* This function closes the open tif structures and prints error message */

void Error(char *s1,char *s2,TIFF *tif1,TIFF *tif2)
{
	if(tif1!=(TIFF *)0)
		TIFFClose(tif1);
	if(tif2!=(TIFF *)0)
		TIFFClose(tif2);
    if(strcmp(s2,"")==0)
    	printf(s1);
    else
    	printf(s1, s2);
    exit(-1);
}



/* ------------------------------------------------------------------- */
/* Functions for internal use                                          */
/* ------------------------------------------------------------------- */


/* InitTIFF initializes the fieldsfor RGB and CMYK TIFF structures. The
   argument type indicates if the file is RGB or CMYK file. */

void InitTIFF(int type,TIFF *tif,u_long width,u_long length,double resolution)
{
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (u_short) BITSPERSAMPLE);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, (u_short) CompressionFlag);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (u_long) ROWSPERSTRIP);
	TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, (u_short) RESOLUTIONUNIT);
	TIFFSetField(tif, TIFFTAG_XRESOLUTION, resolution);
	TIFFSetField(tif, TIFFTAG_YRESOLUTION, resolution);
	TIFFSetField(tif, TIFFTAG_SOFTWARE, "TKK/GRA programs");
	TIFFSetField(tif, TIFFTAG_ARTIST, "Oskar L”nnberg");
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (u_short) PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (u_short) (type==RGB?PHOTOMETRIC_RGB:PHOTOMETRIC_SEPARATED));
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (u_short) type);
	return;
}



/* ChecImage Checks that the image is RGB or CMYK type of image otherwise
   it breaks the execution of the program. The function returns the type
   of the file, RGB ot CMYK.*/

int CheckImage(TIFF *tif)
{
	char 	*str="",
	        *err="";
	u_short photo,comp,samples,planar,bits;
    int     type;

	if(!TIFFGetField(tif,TIFFTAG_COMPRESSION,&comp) ||
            !(comp==COMPRESSION_NONE || comp==COMPRESSION_PACKBITS))
		err="Compression";
	else if(!TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo) ||
			!(photo==PHOTOMETRIC_RGB || photo==PHOTOMETRIC_SEPARATED))
		err="PhotometricInterpretation";
    type=(photo==PHOTOMETRIC_RGB?RGB:CMYK);
	if(!TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samples) ||
			samples!=(type==RGB?RGB:CMYK))
		err="SamplesPerPixel";
	else if(!TIFFGetField(tif,TIFFTAG_PLANARCONFIG,&planar) ||
			planar!=PLANARCONFIG_CONTIG)
		err="PlanarConfiguration";
	else if(!TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bits) || bits!=BITSPERSAMPLE)
		err="BitsPerSample";
	if(strcmp(err,"")!=0)
		{
		str=strcat(strcat("Not a ",(type==RGB?"RGB":"CMYK"))," image: Tag %s invalid\n");
		Error(str,err,tif,(TIFF *)0);
		}
	return type;
}
