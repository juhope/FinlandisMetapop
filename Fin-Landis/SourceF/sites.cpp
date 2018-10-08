#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "error.h"
#include "species.h"
#include "landus.h"
#include "site.h"
#include "sites.h"

///////////////////////////////////////////////////////////////////////////////
//				SITES CLASS CODE			     //
///////////////////////////////////////////////////////////////////////////////

SITES::SITES()
//Constructor.  The maximum number of species will be set to species.
//The map dimensions will can be set by a call to SITES::dim after
//construction.

{
//SPECIES();
map=NULL;
rows=0;
columns=0;
}

SITES::~SITES()
//Destructor.

{
if(map)
	delete [] map; //Nim: added []
}


void SITES::dim(int species, int i, int j)
//This will dimension the size of the map to i rows and j columns, and it
//will initialize the SPECIES class to species..

{
if (map!=NULL)
   delete [] map; //Nim: added []
SITE temp;
temp.setNumber(species);
map=new SITE[i*j];
rows=i;
columns=j;
}

SITE* SITES::operator() (int i, int j)
//This will referrence a site map by row i and column j.

{
int x;

#ifdef BOUNDSCHECK
if (i<=0 || i>rows || j<=0 || j>columns)
    {
     char err[80];
     sprintf(err,"SITES::operator() (int,int)-> (%d, %d) are illegal map\
                  coordinates",i,j);
     errorSys(err,STOP);
    }
#endif
x=(i-1)*columns;
x=x+j-1;
return &map[x];
}

void SITES::read(FILE* infile)
//This will read in a set of sites from infile.

{
int i;
SITE *p;

p=map;
for (i=1;i<=rows*columns;i++)
   {
    if (feof(infile))
       errorSys("SITES::read(FILE*)-> Premature end of file.",STOP);
    p->read(infile);
    p++;
   }
}

void SITES::write(FILE* outfile)
//This will write out a set of sites to outfile.

{
int i;
SITE *p;

p=map;
for (i=1;i<=rows*columns;i++)
   {
    p->write(outfile);
    fprintf(outfile,"\n");
    p++;
   }
}


void SITES::dump()
//This will dump a set of sites to the CRT screen.

{
    int i,j;
    for (i=rows; i>=1; i--) {
        for (j=1; j<=columns; j++) {
            printf("site(%d,%d) =\n", i, j);
            (*this)(i,j)->dump();
        } 
    }
}

void SITES::attach(LANDUNITS *lu)
//This will attach a set of land units to all sites.

{
if (rows<=0 || columns<=0)
   errorSys("SITES::attach(LANDUNITS*)-> Can't attach to NULL map.",STOP);
map->attach(lu);
}

int SITES::numRows()
//This will return the number of rows in the sites structure.

{
return rows;
}

int SITES::numColumns()
//This will return the number of columns in the sites structure.

{
return columns;
}

int SITES::number()
//This will return the total number of sites.

{
return numRows()*numColumns();
}

int SITES::numActive()
//This will return the number of sites with active land units on the map.

{
int count=0;
for (int i=0;i<=numRows()*numColumns()-1;i++)
    if ((map[i]).landUnit->active())
        count++;            
return count;
}

void SITES::setHeader(unsigned long dest[])
{
//sets the sites header info.
for (int i = 0; i<32; i++)
	header[i] = dest[i];
}

unsigned long* SITES::getHeader()
{
//copy the header info.
return &header[0];
}
