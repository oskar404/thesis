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
    Ink.c   - Module for calculating ink transfer to paper
*/



#include <math.h>
#include <string.h>
#include "ink.h"
#include "defs.h"
#include "color.h"
#include "buffer.h"
#include "fileio.h"
#include "message.h"
#include "paper.h"



/**************************************************************/

/* structure and type definitions */

typedef double ConvType;
typedef char   ImageType;

typedef struct  {
    int         x,y;
                } IPoint;

typedef struct  {
    double      x,y;
                } DPoint;

typedef struct  {

    double      PixelSize;

    DPoint      Location;
    IPoint      ISize;
    DPoint      DSize;
    ImageType **Image;
    int         PicType;
    double      ImageScale;

    IPoint      IConv;
    DPoint      DConv;
    ConvType  **Convolution;
    IPoint      IConvCenter;

    double      Splitting;
    double      Deposition;
    double      Absorption;

    double      Transfer;

    ColorType   Ambient;
    double      AmbientScale;

    ColorType   Diffuse;
    double      DiffuseScale;

    ColorType   Specular;
    double      SpecularScale;
    double      SpecularBeta;

                } InkStruct ;

#define NONE   0
#define DOT    1
#define BOX    2
#define MATRIX 3

#define strDOT    "DOT"
#define strBOX    "BOX"
#define strMATRIX "MATRIX"

#define MICROMETER 1000

#define IMAGE_FLAG '1'
#define NO_IMAGE_FLAG '0'

#define WORD_SIZE 20


/* Global variables for this file */

static InkStruct ink;
static int init=FALSE;


/* Internal functions */

static void GetPoint(POINT *,IPoint *);
static Logical InitializeStructure(void);
static void ExitStructure(void);
static Logical ReadImageType(cBuffer,int,String);
static Logical ReadConvMatrix(cBuffer,int,String);
static Logical AllocateConvMem(void);
static Logical FreeConvMem(void);
static Logical ReadImageType(cBuffer,int,String);
static Logical FreeImageMem(void);
static Logical AllocateImageMem(void);

#define GetImageElement(x,y) (ink.Image[x][y]==IMAGE_FLAG ? ink.ImageScale : 0.0)
#define Round(x) (floor(x+0.5))


/**************************************************************/



/**************************************************************

    Logical InkInit(String InkFile)

    Reads the ink file and puts the values to ink structure

*/

Logical InkInit(String InkFile)
{
    cBuffer buf=NULL;
    int     samples;
    int     bSize=BUFFER_SIZE;

    if(init==TRUE)
        goto error;
    if(InitializeStructure()==FALSE)
        goto error;
    samples=ColorGetSize();
    if (FileIOOpen(InkFile,READ) == FALSE)
        goto error;
    if((buf=BufferAllocate(bSize))==NULL)
        goto error;
    while (FileIOReadLine(InkFile,buf,bSize)!=FALSE)
        {
        switch (buf[0])
            {

            case 'a':       /* absorption coefficient */
                if(BufferReadDouble(buf,&ink.Absorption,0)<=0)
                    goto error;
                break;


            case 'd':       /* deposition coefficient */
                if(BufferReadDouble(buf,&ink.Deposition,0)<=0)
                    goto error;
                break;


            case 'p':       /* Pixel size in convolution matrix */
                if(BufferReadDouble(buf,&ink.PixelSize,0)<=0)
                    goto error;
                break;


            case 's':       /* Splitting coefficient */
                if(BufferReadDouble(buf,&ink.Splitting,0)<=0)
                    goto error;
                break;


            case 'l':       /* location of picture of ink */
                {
                int index=0;

                if((index=BufferReadDouble(buf,&ink.Location.x,index))<=0)
                    goto error;
                if((index=BufferReadDouble(buf,&ink.Location.y,index))<=0)
                    goto error;
                ink.Location.x=ink.Location.x*MICROMETER;
                ink.Location.y=ink.Location.y*MICROMETER;
                }
                break;


            case 'i':       /* picture type */
                if(ReadImageType(buf,bSize,InkFile)==FALSE)
                    goto error;
                break;


            case 'S':       /* Specular reflection coefficient */
                {
                int index=0,i;

                if((index=BufferReadDouble(buf,&ink.SpecularScale,index))<=0)
                    goto error;
                if((index=BufferReadDouble(buf,&ink.SpecularBeta,index))<=0)
                    goto error;
                if(ColorReadVector(buf,ink.Specular,InkFile,bSize) ==FALSE)
                    goto error;
                for(i=0;i<samples;i++)
                    ink.Specular[i]=ink.Specular[i]*ink.SpecularScale;
                }
                break;


            case 'D':       /* Diffuse reflection coefficient */
                {
                int i;

                if(BufferReadDouble(buf,&ink.DiffuseScale,0)<=0)
                    goto error;
                if(ColorReadVector(buf,ink.Diffuse,InkFile,bSize)==FALSE)
                    goto error;
                for(i=0;i<samples;i++)
                    ink.Diffuse[i]=ink.Diffuse[i]*ink.DiffuseScale;
                }
                break;


            case 'A':       /* Ambient reflection coefficient */
                {
                int i;

                if(BufferReadDouble(buf,&ink.AmbientScale,0)<=0)
                    goto error;
                if(ColorReadVector(buf,ink.Ambient,InkFile,bSize)==FALSE)
                    goto error;
                for(i=0;i<samples;i++)
                    ink.Ambient[i]=ink.Ambient[i]*ink.AmbientScale;
                }
                break;


            case 'c':       /* convolution matrix */
                {
                int index=0,i;

                /* The size of the Convolution matrix */
                if((index=BufferReadInt(buf,&ink.IConv.x,index))<=0)
                    goto error;
                if(BufferReadInt(buf,&ink.IConv.y,index)<=0)
                    goto error;

                /* convolution matrix */
                if(ReadConvMatrix(buf,bSize,InkFile)==FALSE)
                    goto error;

                ink.IConvCenter.x=Round(ink.IConv.x/2);
                ink.IConvCenter.y=Round(ink.IConv.y/2);
                ink.DConv.x=ink.PixelSize*ink.IConv.x;
                ink.DConv.y=ink.PixelSize*ink.IConv.y;
                }
                break;


            case '#':       /* comment */
            case ' ':
            default:        /* Ignore unknown character */
                break;
            }
        }
    init=TRUE;
    BufferFree(buf);
    (void)FileIOClose(InkFile);
    return TRUE;

error:
    (void)FileIOClose(InkFile);
    if(buf!=NULL)
        BufferFree(buf);
    if(init!=TRUE)
        {
        FreeImageMem();
        FreeConvMem();
        }
    ExitStructure();
    MessageWarning("Can't initialize ink structure");
    return FALSE;
}



/**************************************************************

    Logical InkExit(void)

    Exits the ink structure

*/

Logical InkExit(void)
{
    if(init==FALSE)
        return FALSE;
    init=FALSE;
    FreeImageMem();
    FreeConvMem();
    ExitStructure();
    return TRUE;
}



double InkPicturePixel(POINT *px)
{
    IPoint ind;

    if(init==FALSE)
        return 0.0;
    if ( ink.PicType==NONE)
        return 0.0;
    if ( px->x < ink.Location.x
            || px->y < ink.Location.y
            || px->x > (ink.Location.x+ink.DSize.x)
            || px->y > (ink.Location.y+ink.DSize.y) )
        return 0.0;
    GetPoint(px,&ind);
    return GetImageElement(ind.x,ind.y);
}



/**************************************************************

    Logical InkTransfer(POINT *px)

    Calculates ink transfer to paper for a point px.

    g(x)=[A(x)f(x)] o h(x) [s+dz(x)]
    'o'  means convolution

*/

double InkTransfer(POINT *px)
{
    POINT pnt;
    int ix,iy;
    double result=0;

    if(init==FALSE)
        return 0.0;
    pnt.x=px->x-ink.IConvCenter.x*ink.PixelSize;
    pnt.z=0.0;
    for(ix=0;ix<ink.IConv.x;ix++,pnt.x+=ink.PixelSize)
        {
        pnt.y=px->y-ink.IConvCenter.y*ink.PixelSize;
        for(iy=0;iy<ink.IConv.y;iy++,pnt.y+=ink.PixelSize)
            result+=((double)PaperContact(&pnt))*
                      InkPicturePixel(&pnt)*
                      ink.Convolution[ix][iy];
        }
    ink.Transfer=ink.ImageScale*result*(ink.Splitting+ink.Deposition*PaperRoughness(px));
    return ink.Transfer;
}



double InkAbsorptionCoefficient(void)
{
    if(init==FALSE)
        return 0.0;
    return ink.Absorption;
}



double InkGetSpecularBeta(void)
{
    if(init==FALSE)
        return 0.0;
    return ink.SpecularBeta;
}



ColorType InkGetSpecular(void)
{
    if(init==FALSE)
        return NULL;
    return ink.Specular;
}



ColorType InkGetDiffuse(void)
{
    if(init==FALSE)
        return NULL;
    return ink.Diffuse;
}



ColorType InkGetAmbient(void)
{
    if(init==FALSE)
        return NULL;
    return ink.Ambient;
}



/**************************************************************
    Internal functions for this file
***************************************************************/



/**************************************************************

    Logical GetRealPoint(*DPoint px,*IPoint ind)

    This function converts the actual point information
    to image matrix index. The index is saved to second
    argument.

*/

static void GetPoint(POINT *px,IPoint *ind)
{
    ind->x=Round( (px->x-ink.Location.x) / ink.PixelSize);
    ind->y=Round( (px->y-ink.Location.y) / ink.PixelSize);
    return;
}



/**************************************************************

    static Logical InitializeStructure(void)

    Puts default values to ink structure

*/

static Logical InitializeStructure(void)
{
    ink.PixelSize=INK_PIXEL_SIZE;
    ink.Location.x=0.0;
    ink.Location.y=0.0;
    ink.Image=NULL;
    ink.Convolution=NULL;
    ink.ISize.x=0;
    ink.ISize.y=0;
    ink.DSize.x=0.0;
    ink.DSize.y=0.0;
    ink.IConv.x=0;
    ink.IConv.y=0;
    ink.DConv.x=0.0;
    ink.DConv.y=0.0;
    ink.IConvCenter.x=0;
    ink.IConvCenter.y=0;
    ink.Splitting=INK_SPLITTING;
    ink.Deposition=INK_DEPOSITION;
    ink.Absorption=INK_ABSORPTION;
    ink.Transfer=0.0;
    ink.ImageScale=INK_LAYER;
    ink.PicType=NONE;
    if((ink.Ambient=ColorVectorInit())==NULL)
        return FALSE;
    ink.AmbientScale=INK_AMBIENT_COEFFICIENT;
    if((ink.Diffuse=ColorVectorInit())==NULL)
        return FALSE;
    ink.DiffuseScale=INK_DIFFUSE_COEFFICIENT;
    if((ink.Specular=ColorVectorInit())==NULL)
        return FALSE;
    ink.SpecularScale=INK_SPECULAR_COEFFICIENT;
    ink.SpecularBeta=INK_SPECULAR_BETA ;
    return TRUE;
}



/**************************************************************

    static void ExitStructure(void)

    Exits ink structure.

*/

static void ExitStructure(void)
{
    ColorVectorExit(ink.Ambient);
    ink.Ambient=NULL;
    ColorVectorExit(ink.Diffuse);
    ink.Diffuse=NULL;
    ColorVectorExit(ink.Specular);
    ink.Specular=NULL;
    return;
}



/**************************************************************

    static Logical ReadConvMatrix(cBuffer buf,int bSize,
                                  String InkName)

    Reads the convolution matrix from the file

*/

static Logical ReadConvMatrix(cBuffer buf,int bSize,String InkName)
{
    int x,y,index;
    double tmp;

    if(ink.IConv.x==0 || ink.IConv.y==0)
        return FALSE;
    if(AllocateConvMem()==FALSE)
        return FALSE;
    for(y=0;y<ink.IConv.y;y++)
        {
        if(FileIOReadLine(InkName,buf,bSize)==FALSE)
            {
            FreeConvMem();
            return FALSE;
            }
        index=0;
        for(x=0;x<ink.IConv.x;x++)
            {
            if((index=BufferReadDouble(buf,&tmp,index))<=0)
                {
                FreeConvMem();
                return FALSE;
                }
            ink.Convolution[x][y]=(ConvType)tmp;
            }
        }
    return TRUE;
}



/**************************************************************

    static Logical AllocateConvMem(void)

    Allocates memory for a convolution matrix

*/

static Logical AllocateConvMem(void)
{
    int i;

    ink.Convolution=MemoryAllocate(ConvType*,ink.IConv.x);
    if(ink.Convolution==NULL)
        return FALSE;
    for(i=0;i<ink.IConv.x;i++)
        ink.Convolution[i]=MemoryAllocate(ConvType,ink.IConv.y);
    return TRUE;
}



/**************************************************************

    static Logical FreeConvMem(void)

    Frees the memory used by convolution matrix

*/

static Logical FreeConvMem(void)
{
    int       i;

    for(i=0;i<ink.IConv.x;i++)
        MemoryFree(ink.Convolution[i]);
    MemoryFree(ink.Convolution);
    ink.Convolution=NULL;
    ink.IConv.x=0;
    ink.IConv.y=0;
    ink.DConv.x=0.0;
    ink.DConv.y=0.0;
    ink.IConvCenter.x=0;
    ink.IConvCenter.y=0;
    return TRUE;
}



/**************************************************************

    static Logical ReadImageType(cBuffer buf,int bSize,
                                 String InkFile)

    This function reads the image type and size. After
    allocation of memory this function puts the correct
    values to the allocated matrix.

*/

static Logical ReadImageType(cBuffer buf,int bSize,String InkFile)
{
    char tmp[WORD_SIZE];
    int index=0;

    if((index=BufferReadWord(buf,tmp,index))<=0)
        return FALSE;
    if((index=BufferReadWord(buf,tmp,index))<=0)
        return FALSE;
    if(strcmp(strDOT,tmp)==0)
        {             /* Picture is a DOT */
        if((index=BufferReadDouble(buf,&ink.ImageScale,index))<=0)
            return FALSE;
        ink.ISize.x=1;
        ink.ISize.y=1;
        ink.DSize.x=ink.PixelSize;
        ink.DSize.y=ink.PixelSize;
        ink.PicType=DOT;
        AllocateImageMem();

        /* Fill the allocated matrix */

        ink.Image[0][0]=IMAGE_FLAG;
        }

    else if(strcmp(strBOX,tmp)==0)
        {             /* Picture is a BOX */
        int x,y;

        if((index=BufferReadDouble(buf,&ink.DSize.x,index))<=0)
            return FALSE;
        if(BufferReadDouble(buf,&ink.DSize.y,index)<=0)
            return FALSE;
        if((index=BufferReadDouble(buf,&ink.ImageScale,index))<=0)
            return FALSE;
        ink.DSize.x=ink.DSize.x*MICROMETER;
        ink.DSize.y=ink.DSize.y*MICROMETER;
        ink.ISize.x=(int)(ink.DSize.x/ink.PixelSize);
        ink.ISize.y=(int)(ink.DSize.y/ink.PixelSize);
        ink.PicType=BOX;
        AllocateImageMem();

        /* Fill the allocated matrix */

        for(x=0;x<ink.ISize.x;x++)
            for(y=0;y<ink.ISize.y;y++)
                ink.Image[x][y]=IMAGE_FLAG;
        }

    else if(strcmp(strMATRIX,tmp)==0)
        {             /* Picture is a matrix */
        int x,y;

        if((index=BufferReadInt(buf,&ink.ISize.x,index))<=0)
            return FALSE;
        if(BufferReadInt(buf,&ink.ISize.y,index)<=0)
            return FALSE;
        if((index=BufferReadDouble(buf,&ink.ImageScale,index))<=0)
            return FALSE;
        ink.DSize.x=ink.ISize.x*ink.PixelSize;
        ink.DSize.y=ink.ISize.y*ink.PixelSize;
        ink.PicType=MATRIX;
        AllocateImageMem();

        /* Fill the allocated matrix */

        for(y=0;y<ink.ISize.y;y++)
            {
            if(FileIOReadLine(InkFile,buf,bSize)==FALSE)
                {
                FreeImageMem();
                return FALSE;
                }
            for(x=0;x<ink.ISize.x;x++)
                {
                if(buf[x]=='1')
                    ink.Image[x][y]=IMAGE_FLAG;
                else if(buf[x]=='0')
                    ink.Image[x][y]=NO_IMAGE_FLAG;
                else
                    {
                    FreeImageMem();
                    return FALSE;
                    }
                }
            }
        }

    else
        return FALSE;

    return TRUE;
}



/**************************************************************

    static Logical AllocateImageMem(void)

    Allocates memory for a picture matrix

*/

static Logical AllocateImageMem(void)
{
    int i;

    ink.Image=MemoryAllocate(ImageType*,ink.ISize.x);
    if(ink.Image==NULL)
        return FALSE;
    for(i=0;i<ink.ISize.x;i++)
        ink.Image[i]=MemoryAllocate(ImageType,ink.ISize.y);
    return TRUE;
}



/**************************************************************

    static Logical FreeImageMem(void)

    Frees the memory used by image matrix

*/

static Logical FreeImageMem(void)
{
    int       i;

    for(i=0;i<ink.ISize.x;i++)
        MemoryFree(ink.Image[i]);
    MemoryFree(ink.Image);
    ink.Image=NULL;
    ink.ISize.x=0;
    ink.ISize.y=0;
    ink.DSize.x=0.0;
    ink.DSize.y=0.0;
    return TRUE;
}
