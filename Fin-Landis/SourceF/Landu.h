#ifndef LANDUNIT_H
#define LANDUNIT_H

#include "specatt.h"
#include "specatts.h"

#define PASSIVE 0
#define ACTIVE  1
#define WATER   2
#define WETLAND 3
#define BOG     4
#define LOWLAND 5
#define NONFOREST 6

class LANDUNIT
{
public:

char*  name;               //Land unit name.
int    minShade,           //This is the minimum number of shade years that
                           //must be present before shade 5 species may seed
                           //in on this site.
       fireInterval,       //Mean interval between fires measured in years.

       fireCurve[5],       //Elements 0-4 correpsond to fire class [1-5]. Each
                           //element stores the number of years fuel must
                           //accumulate for the LANDUNIT to accomodate a fire
                           //of a given severity.
       fireClass[5],       //This holds fire classes in conjunction with fire
                           //curve.
       windCurve[5],       //Same as fireCurve and fireClass but used for fire
       windClass[5];       //fuel after windthrow.

LANDUNIT();                //Constructor.
~LANDUNIT();               //Destructor.
void read(FILE*);          //Read a land unit from a file.
void write(FILE*);         //Write a land unit to a file.
void dump();               //Dump contents of land unit to the CRT.
void attach(SPECIESATTRS*);//Attaches a set of species attributes to the
                           //land unit.  Must be performed following
                           //construction.
//--------------------------------------------------------------------------
//Added time since last disturbance to probRepro functions. JP.
float probRepro(int, short);      //Returns the probability of reproduction of the
                           //given species on the land unit. Second parameter
                           //is the time since last disturbance. Species is
                           //referrenced by number.
float probRepro(char*, short);//Returns the probability of reproduction of the
                           //given species on the land unit.  Species is
                           //referrenced by name.
float probRepro(SPECIESATTR*, short);//Returns the probability of reproduction of
                           //the given species on the land unit.  Species is
                           //referrenced by species attribute class.
//--------------------------------------------------------------------------
int active();              //A landunit type is either active or inactive.
                           //Inactive land units are not processed.
int water();               //Returns true if a land unit type is water, false
                           //otherwise.
int lowland();             //Returns true if a land unit type is lowland,
                           //false otherwise

private:

SPECIESATTRS *speciesAttrs;//Pointer to an attached set of species attributes.
                           //Allows species attributes to be referrenced by
                           //name.
int     status;            //Either ACTIVE, PASSIVE, or WATER.
//---------------------------------------------------------------------------
int     recovery;          //Time of humus and ground vegetation recovery after
                           //disturbance. Reproduction probability changes
                           //linearly until that point and then stays constant.
                           //Must be no larger than 100. JP.
float*  probReproDist;     //Set of probabilities of reproduction for each
                           //species immediately after a disturbance given that
                           //seeds are present and there is no competition. JP.
float*  probReproFinal;    //Same probabilities after the recovery of
                           //humus and ground vegetation. JP.
float*  probReproduction[10]; //Array of sets of reproduction
                           //probabilities. Each set corresponds
                           //to a species, and the array describes
                           //the change in reproduction probabilities
                           //as a function of time since last
                           //disturbance. Elements are derived from
                           //recovery above and probReproDist, probReproFinal
                           //below. Array is used for execution speed. JP.
//---------------------------------------------------------------------------
};

#endif

