#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "error.h"
#include "specatt.h"
#include "specatts.h"
#include "specie.h"
#include "species.h"

int SPECIES::numSpec=0;
SPECIESATTRS *SPECIES::speciesAttrs=NULL;

//////////////////////////////////////////////////////////////////////////////
//			    SPECIES CLASS CODE				    //
//////////////////////////////////////////////////////////////////////////////


SPECIES::SPECIES()
//Constructor.  Should be used on all instance constructions other than the
//first.  This may be used on the first instance of construction but should
//be followed by a call to setNumber before the second construction instance.

{
if (numSpec==0)
   {
    species=NULL;
    currentSpec=0;
   }
else
   {
    species=new SPECIE[numSpec];
    currentSpec=0;
    for (int i=0;i<numSpec;i++)
        species[i].attach((*speciesAttrs)(i+1));   //JP
   }
}

SPECIES::~SPECIES()
//Destructor.

{
if(species)
	delete [] species; //Nim: added []
}

void SPECIES::setNumber(int n)
//Sets the number of species.  Must be called after the first instance of
//construction and before the second.

{
if (numSpec==0)
    numSpec=n;
else
    errorSys("SPECIES::setNumber()-> Number of species already set.",STOP);
}

void SPECIES::reset()
//If all instances of class SPECIES are destructed then the reset function
//may be called.  After it has been called instances may be constructed with
//a different number of species present.

{
numSpec=0;
}

void SPECIES::read(FILE *infile)
//Read a set of species from a file.

{
for (int i=0;i<numSpec;i++)
    {
     species[i].read(infile);
    }
}

void SPECIES::write(FILE *outfile)
//Write a set of species to a file.

{
for (int i=0;i<numSpec;i++)
    {
     species[i].write(outfile);
     fprintf(outfile,"\n");
    }
}

void SPECIES::dump()
//Write a set of species to the CRT.

{
if (speciesAttrs==NULL)
    errorSys("SPECIES::dump()-> Species attributes not attached to SPECIES.",
              STOP);

for (int i=0;i<numSpec;i++)
    {
     //printf("%s\t",(*speciesAttrs)(i+1)->name); Redundant, because attrs now attached to species. JP
     species[i].dump();
    }
}

SPECIE* SPECIES::operator () (int n)
//Referrences a species by number.

{
if (n>numSpec || n<0)
    return NULL;
else
   {
    currentSpec=(unsigned char)(n-1);
    return &species[n-1];
   }
}

SPECIE* SPECIES::first()
//Referrence first species.

{
currentSpec=0;
return &species[currentSpec];
}

SPECIE* SPECIES::current(int n)
{
if (n>numSpec || n<0)
    return NULL;
else
   {
    currentSpec=(unsigned char)n;
    return &species[n];
   }
}

SPECIE* SPECIES::next()
//Referrence next species.

{
currentSpec++;
if (currentSpec>=numSpec)
   return NULL;
else
   return &species[currentSpec];
}

SPECIESATTR* SPECIES::specAtt()
//Referrence the current species attribute.

{
if (speciesAttrs==NULL)
    errorSys("SPECIES::specAtt()-> Species attributes not attached to\
              SPECIES.",STOP);

if (currentSpec>=numSpec)
     return NULL;
else
     return (*speciesAttrs)(currentSpec+1);
}

void SPECIES::attach(SPECIESATTRS *s)
//This attaches a set of species attributes to all instances of species.

{
speciesAttrs=s;
}

int SPECIES::number()
//This returns the number of species.

{
return numSpec;
}

int SPECIES::youngest()
//This returns the youngest age.

{
int y=10000;

for (int i=0;i<numSpec;i++)
    {
     y=min(y,species[i].youngest());
    }
return y;
}

int SPECIES::oldestYngst()
//This returns the oldest age among the youngest.

{
int o=0;

for (int i=0;i<numSpec;i++)
    {
     o=max(o,species[i].youngest());
    }
return o;
}

int SPECIES::oldestDense()
//This returns the age of the oldest dense cohort of the site.

{
int o=0;
for (int i=0;i<numSpec;i++)
    {
     o=max(o,species[i].oldestDense());
    }
return o;
}

float SPECIES::oldestNamed(char* spName) //Landif
{
int s=speciesAttrs->current(spName);
if (!s) return 0;
float rAge=(float)species[s].oldestDense()/(*speciesAttrs)(s+1)->longevity;
return rAge;
}

SPECIES& SPECIES::operator=(const SPECIES& s)

{
for (int i=0;i<number();i++)
   {
    species[i]=s.species[i];
   }
currentSpec=0;
return *this;
}
