#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "error.h"
#include "landu.h"
#include "specatts.h"
#include "landus.h"

LANDUNITS::LANDUNITS(int n)
//Constructor.  N is equal to maximum array size for number of species.

{
numLU=0;
landUnits=new LANDUNIT[n];
currentLU=0;
maxLU=n;
}

LANDUNITS::~LANDUNITS()
//Destructor.

{
if (landUnits)
   delete[] landUnits;
}

void LANDUNITS::read(FILE* infile)
//Read set of land units from a file.

{
while (!eof(infile))
{
     if (numLU<=maxLU)
        landUnits[numLU++].read(infile);
     else
        errorSys("LANDUNITS::read(FILE*)-> Array bounds error.",STOP);
}
}

void LANDUNITS::write(FILE* outfile)
//Write set of land units to a file.

{
for (int i=0;i<numLU;i++)
{
    landUnits[i].write(outfile);
}
}

void LANDUNITS::dump()
//Dump set of land units to the CRT.

{
for (int i=0;i<numLU;i++)
{
    landUnits[i].dump();
    printf("===================================\n");
}
}

void LANDUNITS::attach(SPECIESATTRS *s)
//Attaches a set of species attributes to every land unit.  This must be
//performed following construction.

{
for (int i=0;i<maxLU;i++)
{
    landUnits[i].attach(s);
}
}

LANDUNIT* LANDUNITS::operator () (char *name)
//Referrence a land unit by land unit name.

{
for (int i=0;i<numLU;i++)
    if (strcmp(name,landUnits[i].name)==0)
       return &landUnits[i];

return NULL;
}

LANDUNIT* LANDUNITS::operator () (int n)
//Referrence a land unit by land unit number.

{
if (n>numLU || n<=0)
	return NULL;
else
	return &landUnits[n-1];
}

LANDUNIT* LANDUNITS::first()
//Referrence first land unit attribute.

{
currentLU=0;
if (numLU==0)
	 return NULL;
else
	 return &landUnits[currentLU];
}

LANDUNIT* LANDUNITS::next()
//Referrence next land unit attribute.

{
currentLU++;
if (currentLU>=numLU)
	 return NULL;
else
	 return &landUnits[currentLU];
}

int LANDUNITS::number()
//Returns number of land units.

{
return numLU;
}

