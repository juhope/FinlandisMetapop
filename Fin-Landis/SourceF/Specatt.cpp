#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include "mem.h"
#include "system1.h"
#include "error.h"
#include "landis.h"
#include "specatt.h"

////////////////////////////////////////////////////////////////////////////
//				SPECIESATTR CLASS			  //
////////////////////////////////////////////////////////////////////////////

SPECIESATTR::SPECIESATTR()
//Constructor.

{
name=new char[25]; //Moved here. JP.
longevity=0;
senescence=0;
maturity=0;
shadeTolerance=0;
fireTolerance=0;
serotiny=0;
effectiveD=0;
maxD=0;
vegProb=0.0;
vegProb2=0.0;
maxSproutAge=0;
reclassCoef=0.0;
}

SPECIESATTR::~SPECIESATTR()
//Destructor.

{
//free(name);
// Nim: add the next line if SPECIESATTR owns name
if (name)
    delete [] name;
}

void SPECIESATTR::read(FILE* infile)
//Read species attributes from a file.

{
char instring[25];

if (fscanc(infile,"%s",instring)!=1)
	 errorSys("Read error in spec. attr. file.",STOP);
//Nim: deleted next two lines
//if ((name=(char*)malloc((strlen(instring)+1)*sizeof(char)))==NULL)
//	 errorSys("SPECIESATTR::read(FILE*)-> Out of memory error.",STOP);

strcpy(name,instring);

if (fscanc(infile,"%d",&longevity)!=1)
	 errorSys("Error reading in longevity from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&senescence)!=1)
	 errorSys("Error reading in senescence from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&maturity)!=1)
	 errorSys("Error reading in maturity from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&shadeTolerance)!=1)
	 errorSys("Error reading in shadeTolerance from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&fireTolerance)!=1)
	 errorSys("Error reading in fireTolerance from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&serotiny)!=1)
	 errorSys("Error reading in serotiny from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&effectiveD)!=1)
	 errorSys("Error reading in effectiveD from spec. attr. file.",STOP);
//if (effectiveD!=-1)
//	effectiveD = effectiveD * parameters.cellSize / 30;   Certainly unnecessary. JP.
if (fscanc(infile,"%d",&maxD)!=1)
    errorSys("Error reading in maxD from spec. attr. file.",STOP);
if (maxD<effectiveD && maxD != -1)
    errorSys("Error: maxD less than effectiveD.",STOP);
if (fscanc(infile,"%f",&vegProb)!=1)
    errorSys("Error reading in vegProb from spec. attr. file.",STOP);
if (fscanc(infile,"%f",&vegProb2)!=1)
    errorSys("Error reading in vegProb2 from spec. attr. file.",STOP);
if (fscanc(infile,"%d",&maxSproutAge)!=1)
    errorSys("Error reading in maxSproutAge from spec. attr. file.",STOP);
if (fscanc(infile,"%f",&reclassCoef)!=1)
    errorSys("Error reading in reclassCoef from spec. attr. file.",STOP);
}

void SPECIESATTR::write(FILE* outfile)
//Write species attributes to a file.

{
fprintf(outfile,"%s\t",name);
fprintf(outfile,"%d\t",longevity);
fprintf(outfile,"%d\t",senescence);
fprintf(outfile,"%d\t",maturity);
fprintf(outfile,"%d\t",shadeTolerance);
fprintf(outfile,"%d\t",fireTolerance);
fprintf(outfile,"%d\t",serotiny);
fprintf(outfile,"%d\t",effectiveD);
fprintf(outfile,"%d\t",maxD);
fprintf(outfile,"%f\t",vegProb);
fprintf(outfile,"%f\t",vegProb2);
fprintf(outfile,"%d\t",maxSproutAge);
fprintf(outfile,"%f\n",reclassCoef);
}

void SPECIESATTR::dump()
//Dump species attributes to CRT.

{
printf("name:           %s\n",name);
printf("longevity:      %d\n",longevity);
printf("senescence:      %d\n",senescence);
printf("maturity:       %d\n",maturity);
printf("shadeTolerance: %d\n",shadeTolerance);
printf("fireTolerance:  %d\n",fireTolerance);
printf("serotiny:  			%d\n",serotiny);
printf("effectiveD:     %d\n",effectiveD);
printf("maxD:           %d\n",maxD);
printf("vegProb:        %f\n",vegProb);
printf("vegProb2:        %f\n",vegProb2);
printf("maxSproutAge:   %d\n",maxSproutAge);
printf("reclassCoef:    %f\n",reclassCoef);
}

float SPECIESATTR::prob(float x)
//Given a distance this will return a seeding probability for a species.

{
double a=.95;
double b;  //b=0.5 ttt
double m=maxD;

if (m<=100)
   {
    b=2.0;
   }
else if (m<=500)
   {
    b=5.0;
   }
else
   {
    b=15.0;
   }

if (x<=effectiveD)
   {
    return 1.0;
   }
else if (x<=m)
   {
    float prob=(float)(a*exp(-b*(x/m)));
    return prob;
   }
else
    return 0.0;
}
