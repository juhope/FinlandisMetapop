#ifdef __UNIX__
#define REVERSEBYTES
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "system1.h"
#include "error.h"
#ifdef REVERSEBYTES
#include "mbiconvt.h"
#endif
#include "map8.h"

MAP8::MAP8()
//Constructor, No dimensions.

{
int i; //Nim: added this line
strcpy(title,"NO TITLE");
for (i=0;i<maxLeg;i++)           //Nim: changed int i to i and <= to <
    {
     sprintf(legend[i],"%d",i+1);
    }
largeCell=0;
data=NULL;
for (i=0;i<32;i++) header[i]=0;  //Nim: changed int i to i
}


MAP8::MAP8(unsigned long* dest)
//Constructor, No dimensions but sets the coordinates.
{
int i; //Nim: added this line
strcpy(title,"NO TITLE");
for (i=0;i<maxLeg;i++)           //Nim: changed int i to i and <= to <
    {
     sprintf(legend[i],"%d",i+1);
    }
largeCell=0;
data=NULL;
for (i=0;i<32;i++) header[i]=dest[i]; //Nim: changed int i to i
}

MAP8::MAP8(int r, int c)
//Constructor, Dimensions given.

{
int i; //Nim: added this line
strcpy(title,"NO TITLE");
for (i=0;i<=21;i++) //Nim: changed int i to i
    {
     sprintf(legend[i],"<%d>",i+1);
    }
largeCell=0;
data=NULL;
for (i=0;i<32;i++) header[i]=0; //Nim: changed int i to i and 31 to 32
dim(r, c);
}

MAP8::~MAP8()
//Destructor.

{
if(data)
	delete[] data;
}

int MAP8::readLegend(char* fn)
//Read legend from a file.  First line of the file is a title.  All remaining
//lines are legends for up to fifteen classes.

{
FILE *fp;

if ((fp=fopen(fn,"r"))==NULL)
   {
    return 1;
   }
fgets(title,60,fp);
*(title+strlen(title)-1)='\0';

int i=0;
while (!feof(fp))
   {
    fgets(legend[i],60,fp);
     *(legend[i]+strlen(legend[i])-1)='\0';
    i++;
   }
fclose(fp);
return 0;
}

int MAP8::read(const char *fn)
//Read map from a file.   Input file is an ERDAS GIS File with no extension.

{
char str[100];
long dest[32];
float temp;
int i, j, numread, xDim, yDim;
FILE *fp, *fp2;

sprintf(str,"%s",fn);
if ((fp=fopen(str,"rb"))==NULL)
   {
    return 1;
   }

fread((char*)header,4,32,fp);

#ifdef REVERSEBYTES
float temp2=0;
fread((char*)&temp,4,1,fp);
ERDr4_f(&temp,cellSize);
fread((char*)&temp,4,1,fp);
ERDr4_f(&temp,temp2);
printf("%f %f\n", temp2, cellSize);
if (temp2!=cellSize)
   errorSys("Maps don't account for inequality in x and y.",CONTINUE);
cellSize*=temp2;
#else
fread((char*)&temp,4,1,fp);
cellSize=temp;
fread((char*)&temp,4,1,fp);
if (temp!=cellSize)
   errorSys("Maps don't account for inequality in x and y.",CONTINUE);
cellSize*=temp;
#endif

#ifdef REVERSEBYTES
ERDi4_c(&header[4],xDim);
ERDi4_c(&header[5],yDim);
#else
xDim=header[4];
yDim=header[5];
#endif

dim(yDim,xDim);
for (i=yDim;i>0;i--)
   {
    for (j=1;j<=xDim;j++)
       {
        numread=fread((char*)(&((*this)(i,j))),1,1,fp);
        if (numread>0)
            {
             if ((*this)(i,j)>largeCell)
                largeCell=(*this)(i,j);
            }
       }
   }
fclose(fp);

/*TRAILER FILE*/
sprintf(str,"%s.trl",fn);
if ((fp2=fopen(str,"rb"))==NULL)
     {
      errorSys("TRAILER FILE NOT FOUND.\n",CONTINUE);
     }
else
     {
      fread((char*)dest,1,128,fp2);        /*Header*/
      fread((char*)green,1,128,fp2);       /*Green*/
      fread((char*)&green[128],1,128,fp2); /*Green*/
      fread((char*)red,1,128,fp2);         /*Red*/
      fread((char*)&red[128],1,128,fp2);   /*Red*/
		fread((char*)blue,1,128,fp2);        /*Blue*/
      fread((char*)&blue,1,128,fp2);       /*Blue*/
		fread((char*)dest,1,128,fp2);        /*Empty Record*/
      for (i=9;i<=16;i++)                  /*Histogram*/
          fread((char*)dest,1,128,fp2);
      for (i=0;i<=(int)largeCell;i++)
          {
           fread(legend[i],1,32,fp2);
           for (j=0;j<=31;j++)
               if (legend[i][j]=='~')
                   break;
           legend[i][j]='\0';
          }
     fclose(fp2);
   }

return 0;
}

int MAP8::write(const char *fn)
//This will write the map to a file.  It will use the default color scheme.

{
return write(fn, red, green, blue);
}

int MAP8::write(const char *fn, int *red, int *green, int *blue)
//Write map to a file.  There is no extension on the file name.  Three
//arrays of colors cooresponding to the palette (RGB) are also included.

{
int i;
unsigned char ured[maxLeg], //Nim: changed 260 to 256
      ugreen[maxLeg],       //Nim: changed 260 to 256
      ublue[maxLeg];        //Nim: changed 260 to 256

/// 27JUL94_CSH
for (i=0;i<maxLeg;i++)       //Nim: changed 16 to 256
	{
    ured[i]=(unsigned char)red[i];
    ugreen[i]=(unsigned char)green[i];
    ublue[i]=(unsigned char)blue[i];
   }
return write(fn, (unsigned char*)&ured[0], (unsigned char*)&ugreen[0], (unsigned char*)&ublue[0]);
}

int MAP8::write(const char *fn, unsigned char *red, unsigned char *green, unsigned char *blue)
//Write map to a file.  There is no extension on the file name.  Three
//arrays of colors cooresponding to the palette (RGB) are also included.

{
char str[100];
unsigned long dest[32];
unsigned char dst[256];
int i, j, numrow, numcol;
FILE *fp;
numrow=rows();
numcol=cols();
sprintf(str,"%s.gis",fn);
if ((fp=fopen(str,"wb"))==NULL)
   {
    return 1;
   }

for (i=0;i<32;i++) dest[i]=0; //Nim: changed 31 to 32
for (i=0;i<256;i++) dst[i]=0; //Nim: changed 255 to 256

for (i=numrow;i>0;i--)
    {
     for (j=1;j<=numcol;j++)
         {
			 if ((*this)(i,j)>largeCell)
                   largeCell=(*this)(i,j);
			}
    }

strcpy((char*)header,"HEAD74");
*(((char*)header)+7)=(char)0;
*(((char*)header)+8)=(char)1;
#ifdef REVERSEBYTES
int itemp;
ERDc_i4(numCols,&header[4]);
ERDc_i4(numRows,&header[5]);
itemp=largeCell;
ERDi_i2(itemp,(&header[22])+2);
#endif

fwrite((char*)header,4,32,fp);
for (i=numrow;i>0;i--)
    {
     for (j=1;j<=numcol;j++)
         {
          fwrite((char*)(&((*this)(i,j))),1,1,fp);
         }
    }
fclose(fp);

sprintf(str,"%s.trl",fn);
if ((fp=fopen(str,"wb"))==NULL)
   {
    return 1;
	}

//Trailer files.
//Record 1:
for (i=0;i<32;i++)
    dest[i]=0;
strcpy((char*)dest,"TRAIL74");
strcpy(str,title);
strcat(str,"~");
strcpy(((char*)&dest[18]),str);
fwrite((char*)dest,4,32,fp);
//Record 2-7:
fwrite((char*)green,1,128,fp);
fwrite((char*)(&green[128]),1,128,fp);
fwrite((char*)red,1,128,fp);
fwrite((char*)(&red[128]),1,128,fp);
fwrite((char*)blue,1,128,fp);
fwrite((char*)&blue[128],1,128,fp);
//Record 8:
for (i=0;i<32;i++)
    dest[i]=0;
strcpy((char*)dest,"TRAIL74");
fwrite((char*)dest,4,32,fp);
//Record 9-16:
for (i=0;i<32;i++)
    dest[i]=0;
for (i=9;i<=16;i++)
   fwrite((char*)dest,4,32,fp);
//Record 17-56: maximum map legend 50
sprintf((char*)dest,"NA~");
fwrite((char*)dest,1,32,fp);
for (i=1;i<=maxLeg;i++)
   {
    for (j=0;j<32;j++)
        dest[j]=0;
    sprintf((char*)dest,"%s~",legend[i-1]);
	 fwrite((char*)dest,1,32,fp);
   }
fclose(fp);
return 0;
}

void MAP8::copy(const MAP8 &n)
//This will copy the contents of one map into another.  This
//includes legends, titles, and map dimensions.

{
int i;
largeCell=n.largeCell;
dim(n.numRows, n.numCols);
strcpy(title,n.title);
for (i=0;i<22;i++)
    strcpy(legend[i],n.legend[i]);
for (i=0;i<numRows*numCols;i++)
    data[i]=n.data[i];
}


MAP8& MAP8::operator=(const MAP8 &n)

{
int i;
largeCell=n.largeCell;
dim(n.numRows, n.numCols);
strcpy(title,n.title);
for (i=0;i<maxLeg;i++)
    strcpy(legend[i],n.legend[i]);
for (i=0;i<numRows*numCols;i++)
    data[i]=n.data[i];
return *this;
}

void MAP8::dim(int r, int c)
//This will dimension or redimension the map size.

{
if (data!=NULL)
    delete [] data; //Nim: added []

numCols=c;
numRows=r;
data=new unsigned char[numCols*numRows];
}

int MAP8::cols()
//This will return the number of columns in the map.

{
return numCols;
}

int MAP8::rows()
//This will return the number of rows in the map.

{
return numRows;
}

unsigned char& MAP8::operator()(int r, int c)
//This will return a single map element.

{
int x;
#ifdef BOUNDSCHECK
if (r<=0 || r>numRows || c<=0 || c>numCols)
    {
     char err[80];
     sprintf(err,"MAP8::operator() (int,int)-> (%d, %d) are illegal map\
						coordinates",r,c);
     errorSys(err,STOP);
	 }
#endif
/// 20JUN94_CSH
//x=(r-1)*numCols;
//x=x+c-1;
x=((r-1)*numCols)+c-1;
return data[x];
}

void MAP8::rename(char *c)
//This will rename a map.

{
strncpy(title,c,45);
}

void MAP8::assignLeg(int pos, char *c)
//This will assign a legend element to a map.

{
strncpy(legend[pos],c,50);
if ((int)largeCell<pos)
    largeCell=(unsigned char)(pos+1);
}

void MAP8::fill(const unsigned char c)
//This will fill a map with a single value.

{
int i;
for (i=0;i<numCols*numRows;i++)
    data[i]=c;
}

void MAP8::setCellSize(int size)
//This will set the size of a single cell.

{
cellSize=(float)size;
}

int MAP8::high()
//This will return the highest value present in the map.

{
return largeCell;
}

float MAP8::sizeOfCells()
//This will return the cell size of a map.

{
return cellSize;
}

char* MAP8::legendItem(int pos)
//This will return the contents of a legend item.

{
//Note: strdup uses malloc.
return strdup(legend[pos]);
}

void MAP8::setHeader(unsigned long* dest)
//sets the map coordinates.
{
int i; //Nim: added this line
for (i=0;i<32;i++) header[i]=dest[i]; //Nim: changed int i to i
}
