#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "error.h"    //JP
#include "map8.h"
#include "specie.h"
#include "site.h"
#include "landis.h"
#include "reclass2.h"

///////////////////////////////////////////////////////////////////////////
//			STATIC MODULE DATA STRUCTURES			 //
///////////////////////////////////////////////////////////////////////////

static int maximum[30];   //Maximum table.
static int BOOL[30][30];  //Boolean table of class values.
static int numClasses;    //Number of output classes.

////////////////////////////////////////////////////////////////////////////
//			  INTERNAL CLASS IMPLEMENTATIONS		  //
////////////////////////////////////////////////////////////////////////////

static void reset()
//This will reset the reclassification system.

{
int specAtnum=speciesAttrs.number();
for (int i=1;i<=specAtnum;i++)
   {
    //maximum[i-1]=speciesAttrs(i)->longevity;
    maximum[i-1]=(int)((float)(speciesAttrs(i)->longevity)*(float)
                    (1.0/(float)(speciesAttrs(i)->reclassCoef)));
    }	 
}

void readInClassDescrip(FILE *infile)
//This will read in a class description file given the file name
//and the number of classes in the file (m).

{
int i, j, bvalue;
int specAtnum=speciesAttrs.number();
char str[200], sub[200], *p;

for (i=0;i<30;i++)     /// if this is not done the reclass maps
    for (j=0;j<30;j++) /// do not get reclassified _CSH
       BOOL[i][j]=0;

i=1;
numClasses=0;
while (!eof(infile))
{
	fgets(str,199,infile);
   p=str;
   int numChar;
   if (sscanf(p,"%s%n",sub,&numChar)==1)
   {
   	numClasses++;
      p+=numChar+1;
      while (*p!='\0' && *p!='\n' && *(p-1)!='\0' && *(p-1)!='\n')
      {
      	if (sscanf(p,"%s%n",sub,&numChar)!=1)
	         break;
         p+=numChar+1;
	      if (sub[0]=='!')
         {
		   	bvalue=-1;
		   	strcpy(sub,&sub[1]);
		  	}
         else
         {
         	bvalue=1;
		  	}
         for (int j=1;j<=specAtnum;j++)
         if (strcmp(speciesAttrs(j)->name,sub)==0)
         	BOOL[i][j]=bvalue;
      }
      	i++;
   }
}
}

int reclassificationValue(SPECIE *s)
//This will calculate the reclassification value given a specie list for a 
//site.

{
int i,c=0;
unsigned long t;

t=s->getYoung();  //JP
if (t)
{
	for (i=10;i<=320;i+=10)
   {
   	if (t&1)
      c+=i;
      t=t>>1;
   }
}
t=s->getOld(); //JP
if (t)
{
	for (i=330;i<=640;i+=10)
   {
   	if (t&1)
      c+=i;
      t=t>>1;
   }
}       
   
return c;
}

int reclassifySite(SITE *site, int m)
//This will reclassify a singular site.  M is the number of possible output
//classes.

{
float sval[30];
int i, j=1;
SPECIE *specie;
float c;
//float a,b;
for (i=0;i<30;i++)
    sval[i]=0.0;
specie=site->first();
while (specie!=NULL)
{
     //a=(float)specie->oldestDense();
     //b=(float)specie->oldest();
     c=(float)specie->oldestDense()/(float)maximum[j-1]; //Dense added. JP. Landif.
     //if (b>a)
     //   c+=0.2*(b/(float)maximum[j-1]);
     if (c>1.0)
        c=1.0;
     for (i=1;i<=m;i++) //Nim: changed int i to i
     {
		if (BOOL[i][j]!=0)
	 	{
	  		(BOOL[i][j]>0) ? (sval[i]+=c) : (sval[i]-=c) ;
      	//if (sval[i] != 0)                     //Removed These. JP.
	   	//{                                     //They spoil proper
	    //		if (sval[i]>1.0) sval[i]=1.0;   //reclassification!
	    //		if (sval[i]<0.0) sval[i]=0.0;
	   	//}
	 	}
     }
     j++;
     specie=site->next();
}

int mx=0;
float mxVal=0.0;
for (i=1;i<=m;i++)
   if (sval[i]>mxVal)
   {
	   mxVal=sval[i];
      mx=i;
   }

if (mxVal>0.0)
    return mx;
else
    return m+1;
}

//////////////////////////////////////////////////////////////////////////////
//	    			EXTERNAL FUNCTIONS	                    //
//////////////////////////////////////////////////////////////////////////////

void fileReclass(MAP8 &m, const char *fname)
//This will reclass the Landis map and return the result m.  The map will
//be reclassified using a method defined in a an output reclassification
//text file.

{
char str[200], sub[200];
FILE *infile;
int i=2, n=0;
int snr, snc;

//float mxDiff;                   ttt
int mxNum[30]={0};

if ((infile=fopen(fname,"r"))==NULL)
	 errorSys("Could not open rcs file.",STOP);    //JP
reset();
snr=sites.numRows();
snc=sites.numColumns();
sprintf(str,"Reclassification file: %s",fname);
m.rename(str);
m.dim(snr,snc);
m.assignLeg(0,"Water");
m.assignLeg(1,"Non-active");

while (!eof(infile))
{
	 fgets(str,199,infile);
	 if (sscanf(str,"%s",sub)==1)
    {
		sub[strlen(sub)-1]='\0';
	 	m.assignLeg(i,sub);
	 	i++;
	 	n++;
	}
}
m.assignLeg(i,"Other");
for (int j=i+1;j<=14;j++)
	m.assignLeg(j,"");
rewind(infile);
readInClassDescrip(infile);

for (i=snr;i>=1;i--)
{
	for (int j=1;j<=snc;j++)
   {
   	if (sites(i,j)->landUnit->active())
      {
		  m(i,j)=(unsigned char)(reclassifySite(sites(i,j),n)+2);
		  mxNum[m(i,j)-2]++;
      }

	 	else if (sites(i,j)->landUnit->lowland())
		  m(i,j)=2;
	 	else if (sites(i,j)->landUnit->water())
		  m(i,j)=1;
	 	else
		  m(i,j)=0;
   }
}
fclose(infile);
}
