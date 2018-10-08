#ifndef SITE_H
#define SITE_H

#include "species.h"
#include "landu.h"
#include "landus.h"

class SITE : public SPECIES

{
public:

short lastFire,                   //Number of years since last fire.
      lastWind,                   //Number of years since last windthrow.
      lastTimber;                 //Number of years since last timber harvest.
LANDUNIT *landUnit;               //Pointer to the land unit the site is
                                  //present on.
                                  //Points to a member of global landUnits. JP.

SITE(int n);                      //Constructor.      NOTE:  Apply same rules
SITE();                           //Constructor.      as for class SPECIES.

~SITE();                          //Destructor.

static void attach(LANDUNITS*);   //This attaches a set of land units to
                                  //all sites.
void read(FILE*);                 //Read in all site data.
void write(FILE*);                //Write out all site data.
void dump();                      //Dump set of site data to the CRT.
//SITE& operator=(const SITE&);

private:

static LANDUNITS *landUnits;      //A pointer to a set of land units.
};

#endif
