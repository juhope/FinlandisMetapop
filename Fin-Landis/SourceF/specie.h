#ifndef SPECIE_H
#define SPECIE_H

#include <stdio.h>
#include "agelist.h"
#include "specatt.h"

class SPECIE : public AGELIST

{

//whole function not used anymore. Added public getters to AGELIST. JP.

friend class SPECIES; //Landif

public:

bool            vegPropagules,      //Vegetative propagules present?
                disPropagules;      //Seed available?
                //vegPropChecked;     //Is creation of vegetative propagules
                            //already checked during the iteration? JP
SPECIE();                   //Constructor.
~SPECIE();                  //Destructor. JP.

void clear();               //Clear all values.

void attach(SPECIESATTR*);           //Attach species attributes of species i. JP

// Kill functions also enable vegetative propagation when suitable.
void kill(int);             //Kill trees in a given age class. JP.
void kill(int, int);        //Kill trees in a given age range.
void kill(int, int, AGELIST);  //Kill trees in a given range if and only if the
                            //ages are present in a mask were the mask is
                            //represented by type AGELIST.
void thin(int);              //Sets cohorts of given age thinned.
void read(FILE*);           //Read a specie from a file.
void write(FILE*);          //Write a specie to a file.
void dump();                //Dump specie data to the CRT.

void birth(float, bool);                //Adds trees to age ten.   JP.
void grow();                //Grows trees ten years.    JP.
int  oldestDense();        //Returns the age of oldest non-thin cohort. JP. Landif.

bool shading();             //Returns true if the species is shading. JP.
void killSeeds();						//E.g. fires may destroy disPropagules through this. JP
void seedCheck();

private:

SPECIESATTR*    speciesAttr;//Enables species to access its species
                            //attributes. JP.
AGELIST         thinCohorts;//Lists those cohorts that are too thin to
                            //cause shading. JP.
};

#endif
