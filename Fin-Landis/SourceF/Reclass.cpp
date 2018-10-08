#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "map8.h"
#include "specie.h"
#include "site.h"
#include "landis.h"
#include "reclass.h"

void luReclass(MAP8 &m)
//This will perform a reclassification based on the underlying map 
//land units.  It will perform the reclassification on the global 
//object sites and place the results in m.

{
char b[78];
int snr, snc, lun;
int i, j, k; //Nim: added this line
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc);
sprintf(b,"Land units");
m.rename(b);
char *tempstr;
lun=landUnits.number();
for (i=snr;i>=1;i--) //Nim: changed int i to i
    for (j=1;j<=snc;j++) //Nim: changed int j to j
       {
        tempstr=sites(i,j)->landUnit->name;
        for (k=1;k<=lun;k++) //Nim: changed int k to k
            {
             if (strcmp(tempstr,landUnits(k)->name)==0)
                 {
         	  m(i,j)=(unsigned char)(k-1);
	          break;	  
	         }
            }
       }    
for (i=0;i<lun-1;i++) //Nim: changed int i to i
   {
    m.assignLeg(i,landUnits(i+2)->name);
   }
}


void ageReclass(MAP8 &m)
//This will perform a reclassification based upon the oldest cohort upon
//a landis stand.  The cohorts will be scaled into 16 age classes.

{
char str[200];
int i, j;   //i=2 ttt
int snr, snc;
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc);
sprintf(str,"Age class representation");
m.rename(str);
m.assignLeg(0,"Water");
m.assignLeg(1,"NonActive");
for (j=1;j<=14;j++)         //ttt
   m.assignLeg(j,"");
for (i=2;i<13;i++)
   {
    sprintf(str,"%3d - %3d yr",(i-2)*30+1,(i-2)*30+30);
    m.assignLeg(i,str);
   }   
sprintf(str,"     >%3d yr", (i-2)*30);   
m.assignLeg(13,str);   
for (i=snr;i>=1;i--)
  {
    for (j=1;j<=snc;j++)
        {
         if (sites(i,j)->landUnit->active())
	     {
	      m(i,j)=2;
	      SPECIE *s=sites(i,j)->first();
	      while (s!=NULL)
	         {
	          if ((s->oldest()/30+3)>m(i,j))
		      m(i,j)=(unsigned char)(s->oldest()/30+3);
		  s=sites(i,j)->next();
		        }
	      if (m(i,j)>14)
	         m(i,j)=14;
	     }
	 else if (sites(i,j)->landUnit->water() ||
	          sites(i,j)->landUnit->lowland())
	     m(i,j)=1;
	 else
	     m(i,j)=0;
        }
    }
}

void ageReclassDense(MAP8 &m)
//Same as before but only for dense cohorts. JP.

{
char str[200];
int i, j;   //i=2 ttt
int snr, snc;
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc);
sprintf(str,"Age class representation");
m.rename(str);
m.assignLeg(0,"Water");
m.assignLeg(1,"NonActive");
for (j=1;j<=14;j++)         //ttt
   m.assignLeg(j,"");
for (i=2;i<13;i++)
   {
    sprintf(str,"%3d - %3d yr",(i-2)*30+1,(i-2)*30+30);
    m.assignLeg(i,str);
   }
sprintf(str,"     >%3d yr", (i-2)*30);
m.assignLeg(13,str);
for (i=snr;i>=1;i--)
  {
    for (j=1;j<=snc;j++)
        {
         if (sites(i,j)->landUnit->active())
	     {
	      m(i,j)=2;
	      SPECIE *s=sites(i,j)->first();
	      while (s!=NULL)
	         {
	          if ((s->oldestDense()/30+3)>m(i,j))
		      m(i,j)=(unsigned char)(s->oldestDense()/30+3);
		  s=sites(i,j)->next();
		        }
	      if (m(i,j)>14)
	         m(i,j)=14;
	     }
	 else if (sites(i,j)->landUnit->water() ||
	          sites(i,j)->landUnit->lowland())
	     m(i,j)=1;
	 else
	     m(i,j)=0;
        }
    }
}


void speciesAgeMap(MAP8 &m, char *ageFile)
//This will output age at 10 year step for each specified species.
//The cohorts can be up to 50 age classes, 0-500 years.
{
FILE *infile;
char str[200],speciesName[10];
int i, j;//i=2 ttt
int snr, snc;

if ((infile=fopen(ageFile,"r"))==NULL)
{
	printf("%s%s\n", ageFile," file not found.");
   exit(255);
}

if (fscanc(infile,"%s", speciesName)!=1)
{
	printf("%s%s%s%s\n", "Error... when reading species ", speciesName, " from ",ageFile);
   exit(255);
}

int curSp=speciesAttrs.current(speciesName);
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc);
sprintf(str,"Species age presentation");
m.rename(str);
m.assignLeg(1,"Water");
m.assignLeg(2,"NonActive");
for (j=1;j<=maxLeg;j++)
   m.assignLeg(j,"");
for (i=2;i<maxLeg-1;i++)
   {
    sprintf(str,"%3d - %3d yr",(i-2)*10+1,(i-2)*10+10);
    m.assignLeg(i,str);
   }
sprintf(str,"     >%3d yr", (i-2)*10);
m.assignLeg(maxLeg-1,str);
for (i=snr;i>=1;i--)
{
	for (j=1;j<=snc;j++)
   {
   	if (sites(i,j)->landUnit->active())
      {
      	m(i,j)=255;       //where species not presents
	      SPECIE *s=sites(i,j)->current(curSp);
         if(s->query())
         	m(i,j)=(unsigned char)(s->oldest()/10+2);
         if (m(i,j)>50)   //currently maximum longevity is 500 years
	         m(i,j)=255;
     	}
	 	else if (sites(i,j)->landUnit->water())
      	m(i,j)=1;
      else if (sites(i,j)->landUnit->lowland())
      	m(i,j)=2;
   	else
	     m(i,j)=0;
   }
}
fclose(infile);
}

void speciesAgeMapDense(MAP8 &m, char *ageFile)
//Same as before for just dense cohorts. JP.

{
FILE *infile;
char str[200],speciesName[10];
int i, j, tmp;//Added tmp. JP.
int snr, snc;

if ((infile=fopen(ageFile,"r"))==NULL)
{
	printf("%s%s\n", ageFile," file not found.");
   exit(255);
}

if (fscanc(infile,"%s", speciesName)!=1)
{
	printf("%s%s%s%s\n", "Error... when reading species ", speciesName, " from ",ageFile);
   exit(255);
}

int curSp=speciesAttrs.current(speciesName);
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc);
sprintf(str,"Species age presentation");
m.rename(str);
m.assignLeg(1,"Water");
m.assignLeg(2,"NonActive");
for (j=1;j<=maxLeg;j++)
   m.assignLeg(j,"");
for (i=2;i<maxLeg-1;i++)
   {
    sprintf(str,"%3d - %3d yr",(i-2)*10+1,(i-2)*10+10);
    m.assignLeg(i,str);
   }
sprintf(str,"     >%3d yr", (i-2)*10);
m.assignLeg(maxLeg-1,str);
for (i=snr;i>=1;i--)
{
	for (j=1;j<=snc;j++)
   {
   	if (sites(i,j)->landUnit->active())
      {
      	m(i,j)=255;       //where species not presents
	      SPECIE *s=sites(i,j)->current(curSp);
        tmp=s->oldestDense();      //JP
         if (tmp)
         	m(i,j)=(unsigned char)(tmp/10+2);
         if (m(i,j)>50)   //currently maximum longevity is 500 years
	         m(i,j)=255;
     	}
	 	else if (sites(i,j)->landUnit->water())
      	m(i,j)=1;
      else if (sites(i,j)->landUnit->lowland())
      	m(i,j)=2;
   	else
	     m(i,j)=0;
   }
}
fclose(infile);
}
