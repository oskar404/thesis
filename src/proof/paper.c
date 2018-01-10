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
	Paper.c	- Module for handling paper structure
*/



#include <math.h>
#include "defs.h"
#include "paper.h"
#include "buffer.h"
#include "fileio.h"
#include "message.h"



/**************************************************************/

/* structure and type and other definitions */


typedef unsigned char RoughType; /* Roughness type defined for  */
								  /* saving space in roughness */
								  /* matrix in PaperStruct. */

typedef double BetaType;    /* Beta matrix element type */


typedef struct  {
	int			x,y;
				} IPoint;


typedef struct  {
	double		x,y;
				} DPoint;


typedef struct  {
	double		PixelSize;    /* Size in um */

	ColorType	Diffuse;      /* Diffuse coefficient and scale */
	double 		DiffuseScale;

	ColorType	Specular;     /* Specular coefficient and scale */
	double		SpecularScale;
	double      SpecularBeta;

	ColorType	Ambient;      /* Ambient Coefficient and scale */
	double		AmbientScale;

    double		Contact;	  /* Contact level for ink transfer */
	double		Range;        /* Max rougness tells the maximum
								 possible roughness between 0 and
								 1 in roughness matrix the value
								 is in micro meters   */

	IPoint		ISize;        /* Number of rows and columns in matrix */

	DPoint		DSize;        /* Actual size of matrix */

	IPoint		IBeta;        /* Number of rows and columns in Beta matrix */

	DPoint		DBeta;        /* Actual size of Beta matrix */

	BetaType  **Beta;

	RoughType **Rough;

				} PaperStruct ;


#define LOW_VALUE 0      /* Smallest and larges value in Rough matrix */
#define HIGH_VALUE 255

#define Z_DIV 0.0001


/* Global variables for this file */

static PaperStruct paper;
static int init=FALSE;


/* Internal functions and macros */

static Logical InitializeStructure(void);
static void ExitStructures(void);

static Logical ReadRoughnessMatrix(cBuffer,int,String);
static Logical AllocateRoughnessMem(void);
static Logical FreeRoughnessMem(void);

static Logical ReadBetaMatrix(cBuffer,int,String);
static Logical AllocateBetaMem(void);
static Logical FreeBetaMem(void);

static void GetPoint(POINT*,IPoint*);
static void GetRealPoint(POINT*,IPoint*);
static void GetBetaPoint(POINT*,IPoint*);

#define GetElement(x,y) ((double)paper.Rough[x][y]*paper.Range/(double)HIGH_VALUE)
#define GetBetaElement(x,y) ((double)paper.Beta[x][y]+(double)paper.SpecularBeta)
#define GetMaxRange() paper.Range
#define Remainder(x) (x-floor(x)) /* Remainder of a division */
#define Round(x) (floor(x+0.5))


/**************************************************************/



/**************************************************************

	int PaperInit(String PaperName)

	Reads the paper file and initializes the paper struct

*/

int PaperInit(String PaperName)
{
	cBuffer buf=NULL;
	int     bSize=BUFFER_SIZE;
	int 	samples;
	Logical RoughFlag=FALSE;

	if(init==TRUE)
		goto error;
	if(InitializeStructure()==FALSE)
		goto error;
	samples=ColorGetSize();
	if (FileIOOpen(PaperName,READ) == FALSE)
		goto error;
	if((buf=BufferAllocate(bSize))==NULL)
		goto error;
	while (FileIOReadLine(PaperName,buf,bSize)!=FALSE)
		{
		switch (buf[0])
			{
			case 'p':       /* Pixel size in rougness matrix */
				if(BufferReadDouble(buf,&paper.PixelSize,0)<=0)
					goto error;
				break;


			case 'c':       /* Contact level for roughness matrix */
				if(BufferReadDouble(buf,&paper.Contact,0)<=0)
					goto error;
				break;


			case 'm':       /* Maximum roughness scale in matrix */
				if(BufferReadDouble(buf,&paper.Range,0)<=0)
					goto error;
				break;


			case 'S':       /* Specular reflection coefficient */
				{
				int index=0,i;

				if((index=BufferReadDouble(buf,&paper.SpecularScale,index))<=0)
					goto error;
				if((index=BufferReadDouble(buf,&paper.SpecularBeta,index))<=0)
					goto error;
				if(ColorReadVector(buf,paper.Specular,PaperName,bSize) ==FALSE)
					goto error;
				index=0;
				for(i=0;i<samples;i++)
					paper.Specular[i]=paper.Specular[i]*paper.SpecularScale;
				}
				break;


			case 'D':       /* Diffuse reflection coefficient */
				{
				int i;

				if(BufferReadDouble(buf,&paper.DiffuseScale,0)<=0)
					goto error;
				if(ColorReadVector(buf,paper.Diffuse,PaperName,bSize)==FALSE)
					goto error;
				for(i=0;i<samples;i++)
					paper.Diffuse[i]=paper.Diffuse[i]*paper.DiffuseScale;
				}
				break;


			case 'A':       /* Ambient reflection coefficient */
				{
				int i;

				if(BufferReadDouble(buf,&paper.AmbientScale,0)<=0)
					goto error;
				if(ColorReadVector(buf,paper.Ambient,PaperName,bSize)==FALSE)
					goto error;
				for(i=0;i<samples;i++)
					paper.Ambient[i]=paper.Ambient[i]*paper.AmbientScale;
				}
				break;


			case 'r':       /* roughness matrix */
				{
				int index=0,i;

				/* The size of the roughness matrix */
				if((index=BufferReadInt(buf,&paper.ISize.x,index))<=0)
					goto error;
				if(BufferReadInt(buf,&paper.ISize.y,index)<=0)
					goto error;
				index=0;

				/* roughness matrix */
				if(ReadRoughnessMatrix(buf,bSize,PaperName)==FALSE)
					goto error;
				RoughFlag=TRUE;
				}
				break;


			case 'a':       /* angle of beta */
				{
				int index=0,i;

				/* The size of the Beta matrix */
				if((index=BufferReadInt(buf,&paper.IBeta.x,index))<=0)
					goto error;
				if(BufferReadInt(buf,&paper.IBeta.y,index)<=0)
					goto error;
				index=0;

				/* beta matrix */
				if(ReadBetaMatrix(buf,bSize,PaperName)==FALSE)
					goto error;
				}
				break;


			case '#':       /* comment */
			case ' ':
			default:        /* Ignore unknown character */
				break;
			}
		}
	if(RoughFlag==FALSE)
		goto error;
	paper.DSize.x=paper.PixelSize*paper.ISize.x;
	paper.DSize.y=paper.PixelSize*paper.ISize.y;
	paper.DBeta.x=paper.PixelSize*paper.IBeta.x;
	paper.DBeta.y=paper.PixelSize*paper.IBeta.y;
	(void)FileIOClose(PaperName);
	BufferFree(buf);
	init=TRUE;
	return TRUE;

	/* No roughness matrix has been read */

error:
	(void)FileIOClose(PaperName);
	if(buf!=NULL)
		BufferFree(buf);
	FreeRoughnessMem();
	FreeBetaMem();
	ExitStructures();
	MessageWarning("Can't initialize paper structure");
	return FALSE;
}



/**************************************************************

	Logical PaperExit(PaperStruct *paper)

	Exits the using of paper structure and frees the memory
	used by the paper matrix.

*/

Logical PaperExit(void)
{
	if(init==FALSE)
		return FALSE;
	init=FALSE;
	ExitStructures();
	FreeRoughnessMem();
	FreeBetaMem();
	return TRUE;
}



/**************************************************************

	Logical PaperGetNormalVector(VECTOR *Normal,POINT *px)

	This function calculates the normal vector of a point px

	Point px is in micrometers (um).

*/

Logical PaperGetNormalVector(VECTOR *Normal,POINT *px)
{
	double val1X,val1Y,val3X,val3Y;
	IPoint ind;
	VECTOR a,b,*c;

	if(init==FALSE)
		goto error;
	GetPoint(px,&ind);

	/* Get the point values from roughness matrix */

	val1X=GetElement((ind.x-1),ind.y);
	val1Y=GetElement(ind.x,(ind.y-1));
	val3X=GetElement((ind.x+1),ind.y);
	val3Y=GetElement(ind.x,(ind.y+1));

	/* Calculate the vectors for the tanget plane */

	a.k=(val1X-val3X)/2;
	a.i=paper.PixelSize;
	a.j=0.0;
	b.k=(val1Y-val3Y)/2;
	b.j=paper.PixelSize;
	b.i=0.0;

	/* Calculate the Normal vector of the surface */

	c=VectorCross(&a,&b);
	Normal->i=c->i;
	Normal->j=c->j;
	Normal->k=c->k;
	VectorNorm(Normal);
	return TRUE;

error:
	Normal->i=0.0;
	Normal->j=0.0;
	Normal->k=0.0;
	return FALSE;
}



/**************************************************************

	Logical PaperSelfShadow(VECTOR *Light,POINT *px)

	Calculates the self shadow effect for point px of light
	direction L. Light vector has to be normalized.

	Point px is in micrometers (um).

*/

Logical PaperSelfShadow(VECTOR *Light,POINT *px)
{
	IPoint ind;
	POINT ScanPX,BscPX,LgtPX;
	double dx;
	Logical MainDirection=TRUE;

	if(init==FALSE)
		return FALSE;

	/* Prevent dividing by zero or a very small value */

	if(Light->i<Z_DIV && Light->i>(-Z_DIV) &&
	   Light->j<Z_DIV && Light->j>(-Z_DIV))
		return FALSE;

	dx=paper.PixelSize;
	GetRealPoint(px,&ind);
	if(Light->i!=0.0)
		LgtPX.x=Light->i/fabs(Light->i);
	else
		LgtPX.x=1.0;
	if(Light->j!=0.0)
		LgtPX.y=Light->j/fabs(Light->j);
	else
		LgtPX.y=1.0;
	if(fabs(Light->i)<fabs(Light->j))
		MainDirection=FALSE;
	BscPX.x=ScanPX.x=px->x;
	BscPX.y=ScanPX.y=px->y;
	BscPX.z=ScanPX.z=GetElement(ind.x,ind.y);

	do
		{
		if(MainDirection==TRUE)
			{
			ScanPX.x+=LgtPX.x*dx;
			ScanPX.y+=(Light->j/fabs(Light->i))*dx;
			}
		else
			{
			ScanPX.y+=LgtPX.y*dx;
			ScanPX.x+=(Light->i/fabs(Light->j))*dx;
			}
		GetRealPoint(&ScanPX,&ind);
		ScanPX.z = GetElement(ind.x,ind.y);
		LgtPX.z = Light->k*sqrt((ScanPX.x-BscPX.x)*(ScanPX.x-BscPX.x)
							   +(ScanPX.y-BscPX.y)*(ScanPX.y-BscPX.y))
							   +BscPX.z;
		if(LgtPX.z<ScanPX.z)
			return TRUE;
		} while(LgtPX.z<GetMaxRange());
	return FALSE;
}



/**************************************************************

	Logical PaperHiddenPixel(VECTOR *V,POINT *px,POINT *out)

	Searches the pixel seen from direction V when looking
	at pixel px. The actualy seen pixel is stored to out pixel.

	Points px and out are in micrometers (um).

*/

Logical PaperHiddenPixel(VECTOR *V,POINT *px,POINT *out)
{
	IPoint ind;
	POINT ScanPX,BscPX,V_PX;
	double dx;
	Logical MainDirection=TRUE;

	if(init==FALSE)
		return FALSE;

	out->x=px->x;
	out->y=px->y;
	out->z=px->z;

	/* Prevent dividing by zero or a very small value */

	if(V->i<Z_DIV && V->i>(-Z_DIV) &&
	   V->j<Z_DIV && V->j>(-Z_DIV))
		return FALSE;

	dx=paper.PixelSize;
	GetRealPoint(px,&ind);
	if(V->i!=0.0)
		V_PX.x=V->i/fabs(V->i);
	else
		V_PX.x=1.0;
	if(V->j!=0.0)
		V_PX.y=V->j/fabs(V->j);
	else
		V_PX.y=1.0;
	if(fabs(V->i)<fabs(V->j))
		MainDirection=FALSE;
	BscPX.x=ScanPX.x=px->x;
	BscPX.y=ScanPX.y=px->y;
	BscPX.z=ScanPX.z=GetElement(ind.x,ind.y);

	do
		{
		if(MainDirection==TRUE)
			{
			ScanPX.x+=V_PX.x*dx;
			ScanPX.y+=(V->j/fabs(V->i))*dx;
			}
		else
			{
			ScanPX.y+=V_PX.y*dx;
			ScanPX.x+=(V->i/fabs(V->j))*dx;
			}
		GetRealPoint(&ScanPX,&ind);
		ScanPX.z = GetElement(ind.x,ind.y);
		V_PX.z = V->k*sqrt((ScanPX.x-BscPX.x)*(ScanPX.x-BscPX.x)
							+(ScanPX.y-BscPX.y)*(ScanPX.y-BscPX.y))
							+BscPX.z;
		if(V_PX.z<ScanPX.z)
        	{
			out->x=ScanPX.x;
			out->y=ScanPX.y;
			out->z=ScanPX.z;
            }
		} while(V_PX.z<GetMaxRange());
	return TRUE;
}



/**************************************************************

	Logical PaperContact(POINT *px)

	Checks the contact level for point px.

	Point px is in micrometers (um).

*/

Logical PaperContact(POINT *px)
{
	IPoint ind;

	if(init==FALSE)
		return FALSE;
	GetPoint(px,&ind);
	if(GetElement(ind.x,ind.y)>=(paper.Range-paper.Contact))
		return TRUE;
	else
    	return FALSE;
}



double PaperRoughness(POINT *px)
{
	IPoint ind;

	if(init==FALSE)
		return -1.0;
	GetPoint(px,&ind);
	return (paper.Range-GetElement(ind.x,ind.y));
}



double PaperGetSpecularBeta(POINT *px)
{
	IPoint ind;

	if(init==FALSE)
		return -1.0;
	if(paper.Beta==NULL || px==NULL)
		return paper.SpecularBeta;
	GetBetaPoint(px,&ind);
	return GetBetaElement(ind.x,ind.y);
}



double PaperGetSpecularScale(void)
{
	if(init==FALSE)
		return 0.0;
	return paper.SpecularScale;
}



ColorType PaperGetSpecular(void)
{
	if(init==FALSE)
		return NULL;
	return paper.Specular;
}



ColorType PaperGetDiffuse(void)
{
	if(init==FALSE)
		return NULL;
	return paper.Diffuse;
}



ColorType PaperGetAmbient(void)
{
	if(init==FALSE)
		return NULL;
	return paper.Ambient;
}



double PaperGetPixelSize(void)
{
	if(init==FALSE)
		return -1.0;
	return paper.PixelSize;
}





/**************************************************************
	Internal functions for this file
***************************************************************/



/**************************************************************

	Logical GetPoint(*DPoint px,*IPoint ind)

	This function converts the actual point information
	to matrix index. If the index is on the border of matrix
	the index is shifted towards the center. The index is
	saved to second argument.

*/

static void GetPoint(POINT *px,IPoint *ind)
{
	double dX,dY;

    GetRealPoint(px,ind);
	if(ind->x==0)
		ind->x=1;
	else if(ind->x==(paper.ISize.x-1))
		ind->x--;
	if(ind->y==0)
		ind->y=1;
	else if(ind->y==(paper.ISize.y-1))
		ind->y--;
	return;
}



/**************************************************************

	Logical GetRealPoint(*DPoint px,*IPoint ind)

	This function converts the actual point information
	to matrix index. The index is saved to second argument.

*/

static void GetRealPoint(POINT *px,IPoint *ind)
{
	double dX,dY;

	dX=Remainder(px->x/paper.DSize.x)*paper.DSize.x;
	dY=Remainder(px->y/paper.DSize.y)*paper.DSize.y;
	ind->x=Round(dX/paper.PixelSize);
	ind->y=Round(dY/paper.PixelSize);
	return;
}



/**************************************************************

	Logical GetBetaPoint(*DPoint px,*IPoint ind)

	This function converts the actual point information
	to Beta matrix index. The index is saved to second argument.

*/

static void GetBetaPoint(POINT *px,IPoint *ind)
{
	double dX,dY;

	dX=Remainder(px->x/paper.DBeta.x)*paper.DBeta.x;
	dY=Remainder(px->y/paper.DBeta.y)*paper.DBeta.y;
	ind->x=Round(dX/paper.PixelSize);
	ind->y=Round(dY/paper.PixelSize);
	return;
}



/**************************************************************

	Logical InitializeStructure(void)

	This function puts the default values to paper struct.

*/

static Logical InitializeStructure(void)
{
	paper.PixelSize=PAPER_PIXEL_SIZE;

	if((paper.Diffuse=ColorVectorInit())==NULL)
		return FALSE;
	paper.DiffuseScale=PAPER_DIFFUSE_COEFFICIENT;

	if((paper.Specular=ColorVectorInit())==NULL)
		return FALSE;
	paper.SpecularScale=PAPER_SPECULAR_COEFFICIENT;
	paper.SpecularBeta=PAPER_SPECULAR_BETA;

	if((paper.Ambient=ColorVectorInit())==NULL)
		return FALSE;
	paper.AmbientScale=PAPER_AMBIENT_COEFFICIENT;

	paper.Range=PAPER_MAX_ROUGHNESS;
	paper.Contact=PAPER_CONTACT_LEVEL;
	paper.ISize.x=0;
	paper.ISize.y=0;
	paper.DSize.x=0.0;
	paper.DSize.y=0.0;
	paper.IBeta.x=0;
	paper.IBeta.y=0;
	paper.DBeta.x=0.0;
	paper.DBeta.y=0.0;
	paper.Rough=NULL;
	paper.Beta=NULL;
	return TRUE;
}



/**************************************************************

	static void ExitStructures(void)

	This function frees the memory used by vectors.

*/

static void ExitStructures(void)
{
	ColorVectorExit(paper.Ambient);
	paper.Ambient=NULL;
	ColorVectorExit(paper.Diffuse);
	paper.Diffuse=NULL;
	ColorVectorExit(paper.Specular);
	paper.Specular=NULL;
	return;
}



/**************************************************************

	Logical ReadRoughnessMatrix(PaperStruct *paper,cBuffer buf,
							int bSize,String PaperName)

	Reads the roughness matrix from the file

*/

static Logical ReadRoughnessMatrix(cBuffer buf,int bSize,String PaperName)
{
	int x,y,index;
	int tmp;

	if(paper.ISize.x==0 || paper.ISize.y==0)
		return FALSE;
	if(AllocateRoughnessMem()==FALSE)
		return FALSE;
	for(y=0;y<paper.ISize.y;y++)
		{
		if(FileIOReadLine(PaperName,buf,bSize)==FALSE)
			{
			FreeRoughnessMem();
			return FALSE;
			}
		index=0;
		for(x=0;x<paper.ISize.x;x++)
			{
			if((index=BufferReadInt(buf,&tmp,index))<=0)
				{
				FreeRoughnessMem();
				return FALSE;
				}
			paper.Rough[x][y]=(RoughType)tmp;
			}
		}
	return TRUE;
}



/**************************************************************

	Logical AllocateRoughnessMem(PaperStruct *paper)

	Allocates memory for a matrix

*/

static Logical AllocateRoughnessMem(void)
{
	int i;

	paper.Rough=MemoryAllocate(RoughType*,paper.ISize.x);
	if(paper.Rough==NULL)
		return FALSE;
	for(i=0;i<paper.ISize.x;i++)
		paper.Rough[i]=MemoryAllocate(RoughType,paper.ISize.y);
	return TRUE;
}



/**************************************************************

	Logical FreeRoughnessMem(void)

	Frees the memory used by roughness matrix

*/

static Logical FreeRoughnessMem(void)
{
	int       i;

	for(i=0;i<paper.ISize.x;i++)
		MemoryFree(paper.Rough[i]);
	MemoryFree(paper.Rough);
	paper.Rough=NULL;
	paper.ISize.x=0;
	paper.ISize.y=0;
	paper.DSize.x=0.0;
	paper.DSize.y=0.0;
	return TRUE;
}



/**************************************************************

	static Logical ReadBetaMatrix(cBuffer buf,int bSize,
								  String PaperName)

	Reads the beta matrix from the file

*/

static Logical ReadBetaMatrix(cBuffer buf,int bSize,String PaperName)
{
	int x,y,index;
	double tmp;

	if(paper.IBeta.x==0 || paper.IBeta.y==0)
		return FALSE;
	if(AllocateBetaMem()==FALSE)
		return FALSE;
	for(y=0;y<paper.IBeta.y;y++)
		{
		if(FileIOReadLine(PaperName,buf,bSize)==FALSE)
			{
			FreeBetaMem();
			return FALSE;
			}
		index=0;
		for(x=0;x<paper.IBeta.x;x++)
			{
			if((index=BufferReadDouble(buf,&tmp,index))<=0)
				{
				FreeBetaMem();
				return FALSE;
				}
			paper.Beta[x][y]=(BetaType)tmp;
			}
		}
	return TRUE;
}



/**************************************************************

	static Logical AllocateBetaMem(void)

	Allocates memory for a matrix

*/

static Logical AllocateBetaMem(void)
{
	int i;

	paper.Beta=MemoryAllocate(BetaType*,paper.IBeta.x);
	if(paper.Beta==NULL)
		return FALSE;
	for(i=0;i<paper.IBeta.x;i++)
		paper.Beta[i]=MemoryAllocate(BetaType,paper.IBeta.y);
	return TRUE;
}



/**************************************************************

	static Logical FreeBetaMem(void)

	Frees the memory used by beta matrix

*/

static Logical FreeBetaMem(void)
{
	int       i;

	for(i=0;i<paper.IBeta.x;i++)
		MemoryFree(paper.Beta[i]);
	MemoryFree(paper.Beta);
	paper.Beta=NULL;
	paper.IBeta.x=0;
	paper.IBeta.y=0;
	paper.DBeta.x=0.0;
	paper.DBeta.y=0.0;
	return TRUE;
}






