#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "error.h"
#include "species.h"
#include "landus.h"
#include "site.h"

LANDUNITS *SITE::landUnits=NULL;

///////////////////////////////////////////////////////////////////////////////
//				SITE CLASS CODE				     //
///////////////////////////////////////////////////////////////////////////////


SITE::SITE() : SPECIES()
//Constructor.


{
lastFire=0;
lastWind=0;
lastTimber=0;
landUnit=NULL; //JP
}

SITE::~SITE()

{
if(landUnit)
	landUnit=NULL;
}

void SITE::attach(LANDUNITS *l)

{
landUnits=l;
}

void SITE::read(FILE *infile)
//Read in site data.

{
char temp[80];
int t1, t2;
fscanc(infile,"%s %d %d",temp,&t1,&t2);
lastWind=(short)t1;
lastFire=(short)t2;
if ((landUnit=(*landUnits)(temp))==NULL)
     errorSys("Illegal landtype encountered in map attribute file.",STOP);
SPECIES::read(infile);
}

void SITE::write(FILE *outfile)
//Write out site data.

{
fprintf(outfile,"%s %d %d %d\n",landUnit->name,lastFire, lastWind, lastTimber);
SPECIES::write(outfile);
printf("\n\n");
}

void SITE::dump()
//Dump site data to the CRT screen.

{
printf("%s %d %d %d\n",landUnit->name,lastFire, lastWind, lastTimber);
SPECIES::dump();
printf("\n\n");
}
