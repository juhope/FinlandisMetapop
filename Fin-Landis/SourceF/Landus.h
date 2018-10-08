#ifndef LANDUNITS_H
#define LANDUNITS_H

#include "defines.h"
#include "landu.h"
#include "specatts.h"

class LANDUNITS

{
public:

LANDUNITS(int n=MAX_SPECIES); //Constructor, sets upper limit for number of
                              //land units.     
~LANDUNITS();                 //Destructor.
void read(FILE*);             //Read set of land unit attributes from a file.
//void updateLandtype(FILE*);   //Read in new updated species reproduction
                              //coefficient set.
void write(FILE*);            //Write set of land unit attributes to a file.
void dump();                  //Dump set of land unit attributes to the CRT.
void attach(SPECIESATTRS*);   //Attaches a set of species attributes to
                              //every land unit.  Must be performed following
                              //construction.
LANDUNIT* operator() (char*); //Referrence an attribute by species name.
LANDUNIT* operator() (int);   //Referrence an attribute by species number.
LANDUNIT* first();            //Referrence first land unit attribute.
LANDUNIT* next();             //Referrence next land unit attribute.
                              //NOTE: All four referrence functions return 
                              //NULL if attribute referrenced is illeagal or
                              //unavailable.
int number();                 //Returns number of land units.

private:

LANDUNIT *landUnits;          //Array holding all land units.
int numLU;                    //Number of land units.
int currentLU;                //Current land unit being pointed to by first
                              //and next access functions.
int maxLU;                    //Maximum number of land units.  Defined upon 
                              //class construction.
};

#endif
