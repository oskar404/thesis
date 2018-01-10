/*
	Header file for Acces.c package which contains functions
	to help using TIFF image
*/
#ifndef _ACCESS_
#define _ACCESS_

#include "tiffiop.h"
#include "tiffio.h"

#define RGB	    3      /* file types indicating also how many */
#define CMYK	4      /* samples per pixel are used.         */

#define RED	    4      /* RGB picture colors */
#define GREEN 	5
#define BLUE 	6

#define CYAN 	0      /* CMYK picture colors */
#define MAGENTA	1
#define YELLOW 	2
#define BLACK 	3

#ifndef TRUE
#define TRUE	1      /* logical values */
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define READ  "r"      /* The mode when the files are opened */
#define WRITE "w"


typedef int logical;   /* Type for using logical value */

typedef char *buffer_t;  /* Row buffer type */

/* typedef unsigned long u_long; */  /* Unsigned types */
/* typedef unsigned short u_short; */
/* typedef unsigned char u_char; */

typedef u_char value_t;  /* Type of the color value used in row buffers */

typedef struct {         /* TIFF image size and resolution type */
	u_long width,length;
	float resolution;
	} ImageSize;



/* ----------------------------------------------------------- */
/* Function prototypes                                         */
/* ----------------------------------------------------------- */

TIFF *OpenImage(char *,char *,...);	/* This function opens the TIFF structure */
void CloseImage(TIFF *);            /* This closes the open TIFF structure */


/* Two macros for using the OpenImage() fuvtion. */
#define OpenForWriting(n,t,w,l,r)  OpenImage(n,WRITE,t,(u_long)w,(u_long)l,(double)r)
#define OpenForReading(name)	   OpenImage(name,READ)


ImageSize *GetImageSize(TIFF *);    /* Returns the size and resolution of TIFF
                                       structure as a pointer to structure
                                       called ImageSize. */


/* These functions works with row buffers. */
buffer_t AllocRowBuffer(TIFF *);     /* Allocate memory for buffer */
logical WriteRowBuffer(TIFF *,buffer_t,u_long); /* Write buffer to file */
logical ReadRowBuffer(TIFF *,buffer_t,u_long);  /* Read buffer from file */
void FreeRowBuffer(buffer_t);        /* Free allocated memory. */

/* Macros for getin and puting pixel in a buffer */
#define GetPixel(buf,type,col,i)	          buf[type*i+(col-(type==RGB?RED:CYAN))]
#define PutPixel(buf,type,col,i,val)	      buf[type*i+(col-(type==RGB?RED:CYAN))]=val


/* Sets the compression scheme for output file. */
void SetCompression(int);


/* Gets the type of the TIFF file, RGB or CMYK */
int GetTifType(TIFF *);


/* Function for error handling */
void Error(char *,char *,TIFF *,TIFF *);
#endif /* _ACCESS_ */
