
#ifndef SPECIES_H
#define SPECIES_H

#include <stdio.h>
#include "specie.h"
#include "specatt.h"
#include "specatts.h"

class SPECIES

{
public:

SPECIES(int n);                   //Constructor.  This constructor can only
                                  //be used on the first creation instance of
                                  //class SPECIES.  It sets the number of
                                  //different varieties of species in the
                                  //model.
SPECIES();                        //Constructor.  Should be used on all
                                  //instance constructions other than the
                                  //first.  This may be used on the first
                                  //instance of construction but should be
                                  //followed by a call to setNumber before
                                  //the second construction instance.
~SPECIES();                       //Destructor.
static void setNumber(int);       //Sets the number of species.  Must be
                                  //called after the first instance of
                                  //construction and before the second.
static void reset();              //If all instances of class SPECIES are
                                  //destructed then the reset function may
                                  //be called.  After it has been called
                                  //instances may be constructed with a
                                  //different number of species present.
void read(FILE*);                 //Read in all species data.
void write(FILE*);                //Write out all species data.
void dump();                      //Dump set of species data to the CRT.
SPECIE* operator() (int);         //Referrence a species by species number.
SPECIE* first();                  //Referrence first species attribute.
SPECIE* next();                   //Referrence next species attribute.
SPECIE* current(int);             //Reference by current species number. hsh
SPECIESATTR* specAtt();           //Referrence the current species attribute.
static void attach(SPECIESATTRS*);//This attaches a set of species attributes
                                  //to all instances of species.
static int number();              //Returns number of species.
int youngest();                   //Return the youngest age.
int oldestYngst();                //Return the oldest age.
int oldestDense();                //Return the age of the oldest dense cohort
                                  //on the site.
float oldestNamed(char*);         //Return age of oldest dense cohort
                                  //of species named in argument.JP

SPECIES& operator=(const SPECIES&);

private:

SPECIE *species;                  //Array holding all species information.
unsigned char currentSpec;        //Current species number being pointed to
                                  //by first and next access functions.
static int numSpec;               //Number of species.
static SPECIESATTRS *speciesAttrs;//Pointer to an attached set of species
                                  //Attributes.
};

#endif
