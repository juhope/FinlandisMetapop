#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include "system1.h"
//#include "mem.h"
#include "error.h"
#include "specatt.h"
#include "specatts.h"
#include "landu.h"
#include "landis.h" //parameters accessible. JP.

LANDUNIT::LANDUNIT()
//Constructor.

{
fireInterval=0;
probReproDist=NULL; //JP
probReproFinal=NULL; //JP
name=new char[25]; //JP
for (int i=0;i<10;i++) probReproduction[i]=NULL; //JP
speciesAttrs=NULL;
}

LANDUNIT::~LANDUNIT()
//Destructor.

{
delete [] name; //Nim: changed delete to delete []
for (int i=0;i<10;i++)  //JP
	delete [] probReproduction[i];
delete [] probReproDist; //JP
delete [] probReproFinal; //JP
//if(speciesAttrs)
//	delete speciesAttrs;
}

void LANDUNIT::attach(SPECIESATTRS *s)

{
 speciesAttrs=s;
}

void LANDUNIT::read(FILE* infile)
//Read a land unit from a file.

{
int i,j; //Nim: added this line, JP: added j.
if (speciesAttrs==NULL)
	errorSys("LANDUNIT::read(FILE*)-> No attaced species attributes.",STOP);

int specAtNum=speciesAttrs->number();

if (probReproDist)
    delete [] probReproDist;
if (probReproFinal)
    delete [] probReproFinal;
probReproDist=new float[specAtNum]; //JP
probReproFinal=new float[specAtNum]; //JP

for (i=0;i<10;i++)
    {
    if (probReproduction[i])
        delete [] probReproduction[i];
    probReproduction[i]=new float[specAtNum]; //JP
    }

char instring[25];
if (fscanc(infile,"%s",instring)!=1)
   errorSys("Error reading in name from landtype file.",STOP);


strcpy(name,instring);

if (fscanc(infile,"%d",&minShade)!=1)
   errorSys("Error reading in minShade from landtpye file.",STOP);

if (fscanc(infile,"%d",&fireInterval)!=1)
   errorSys("Error reading in fireInterval from landtype file.",STOP);

//--------------------------------------------------------------------------
//JP changes and additions:
if (fscanc(infile,"%d",&recovery)!=1)
   errorSys("Error reading in recovery from landtype file.",STOP);

for (i=0;i<specAtNum;i++)  //Nim: changed int i to i
    {
    if (fscanc(infile,"%f",&probReproDist[i])!=1)
       errorSys("Error reading in probReproDist from landtype file.",STOP);
    if (fscanc(infile,"%f",&probReproFinal[i])!=1)
       errorSys("Error reading in probReproFinal from landtype file.",STOP);
    }

for (i=0;i<5;i++)  //Nim: changed int i to i
    if (fscanc(infile,"%d",&fireCurve[i])!=1)
       errorSys("Error reading in fireCurve from landtype file.",STOP);

for (i=0;i<5;i++)  //Nim: changed int i to i
    if (fscanc(infile,"%d",&fireClass[i])!=1)
       errorSys("Error reading in fireCurve from landtype file.",STOP);

for (i=0;i<5;i++)  //Nim: changed int i to i
    if (fscanc(infile,"%d",&windCurve[i])!=1)
       errorSys("Error reading in windCurve from landtype file.",STOP);

for (i=0;i<5;i++)  //Nim: changed int i to i
    if (fscanc(infile,"%d",&windClass[i])!=1)
       errorSys("Error reading in windCurve from landtype file.",STOP);

if (parameters.reproFlag && recovery)
    for (i=0;i<10;i++) //Set probReproduction array. JP.
      {
        for (j=0;j<specAtNum;j++)
            probReproduction[i][j]
            =probReproDist[j]+(min((float)1.0,(float)(10.0*i/recovery))*(probReproFinal[j]-probReproDist[j])); //HF: Added (float)
      }
else
    for (i=0;i<10;i++) //Set probReproduction array. JP.
      {
        for (j=0;j<specAtNum;j++) probReproduction[i][j]=probReproDist[j];
      }

if (strcmp(name,"empty")==0)
   status=PASSIVE;
else if (strcmp(name,"water")==0 || strcmp(name,"waterB")==0)     //Buffer type added. JP.
     status=WATER;
else if (strcmp(name,"wetland")==0 || strcmp(name,"wetlandB")==0) //Buffer type added. JP.
     status=WETLAND;
else if (strcmp(name,"bog")==0)
     status=BOG;
else if (strcmp(name,"lowland")==0)
     status=LOWLAND;
else if (strcmp(name,"nonforest")==0)
     status = NONFOREST;
else
     status=ACTIVE;
}

void LANDUNIT::write(FILE* outfile)
//Write a land unit to a file.

{
int i; //Nim: added this line
int specAtNum=speciesAttrs->number();
if (speciesAttrs==NULL)
   errorSys("LANDUNIT::write(FILE*)-> No attached species attributes.",STOP);

fprintf(outfile,"%s ", name); //correction. JP.
fprintf(outfile,"%d %d %d",minShade,fireInterval,recovery);//added recovery, JP.
fprintf(outfile,"\n");
for (i=0;i<specAtNum;i++)
    fprintf(outfile,"%2.2f ",probReproDist[i]);     //JP
fprintf(outfile,"\n");
for (i=0;i<specAtNum;i++)
    fprintf(outfile,"%2.2f ",probReproFinal[i]);    //JP
fprintf(outfile,"\n");
for (i=0;i<5;i++)        //Nim: changed int i to i   Landif
    fprintf(outfile,"%d ",fireCurve[i]);
fprintf(outfile,"\n");
for (i=0;i<5;i++)        //Nim: changed int i to i
    fprintf(outfile,"%d ",fireClass[i]);
fprintf(outfile,"\n");
for (i=0;i<5;i++)        //Nim: changed int i to i   Landif
    fprintf(outfile,"%d ",windCurve[i]);
fprintf(outfile,"\n");
for (i=0;i<5;i++)        //Nim: changed int i to i
    fprintf(outfile,"%d ",windClass[i]);
fprintf(outfile,"\n");
}

void LANDUNIT::dump()
//Dump a land unit to the CRT.

{
int i; //Nim: added this line
int specAtNum=speciesAttrs->number();
if (speciesAttrs==NULL)
   errorSys("LANDUNIT::dump(FILE*)-> No attached species attributes.",STOP);

printf("Name:          %s\n",name);
printf("fireInterval:  %d\n",fireInterval);
printf("recovery time: %d\n",recovery); //JP
for (i=0;i<specAtNum;i++) //Nim: changed int i to i
    printf("%s:  %2.2f  %2.2f\n",(*speciesAttrs)(i+1)->name,probReproDist[i],
        probReproFinal[i]);
for (i=0;i<5;i++)        //Nim: changed int i to i    Landif
    printf("Class %d:  %d\n",fireClass[i],fireCurve[i]);
for (i=0;i<5;i++)        //Nim: changed int i to i    Landif
    printf("Class %d:  %d\n",windClass[i],windCurve[i]);
}

float LANDUNIT::probRepro(int s, short t)
//Returns the probability of reproduction of the given species on the given
//land unit.  Species is referrenced by number. t is time since disturbance.

{
if (s<=speciesAttrs->number() && s>0)
    {
    if (t<=100) return probReproduction[t/10-1][s-1];
        else return probReproFinal[s-1];
    }
errorSys("LANDUNIT::probRepro(int, short)-> Array bounds error.",STOP);
return 0.0;
}

float LANDUNIT::probRepro(char *s, short t)
//Returns the probability of reproduction of the given species on the given
//land unit.  Species is referrenced by name.

{
int specAtNum=speciesAttrs->number();
for (int i=0;i<specAtNum;i++)
{
   if (strcmp((*speciesAttrs)(i+1)->name,s)==0)
        {
        if (t<100) return probReproduction[t/10-1][i];
            else return probReproFinal[i];
        }
}

errorSys("LANDUNIT::probRepro(char*)-> Illegal species name.",STOP);
return 0.0;
}

float LANDUNIT::probRepro(SPECIESATTR *s, short t)
//Returns the probability of reproduction of the given species on the given
//land unit.  Species is referrenced by species record.

{
int specAtNum=speciesAttrs->number();
for (int i=0;i<specAtNum;i++)
{
    if (strcmp((*speciesAttrs)(i+1)->name,s->name)==0)
      {
        if (t<100) return probReproduction[t/10][i];
          else return probReproFinal[i];
      }
}

errorSys("LANDUNIT::probRepro(SPECIESATTR*)-> Illegal spec. attr.",STOP);
return 0.0;
}

int LANDUNIT::active()
//

{
if (status==ACTIVE)
	return true;
else
	return false;
}

int LANDUNIT::water()

{
if (status==WATER)
	return true;
else
	return false;
}


int LANDUNIT::lowland()     //changed

{
if (status==WETLAND || status==BOG || status==LOWLAND || status==NONFOREST)
	return true;
else
	return false;
}

