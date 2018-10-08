#ifndef TIMBER_H
#define TIMBER_H

#include "defines.h"
#include "point.h"
#include "map8.h"

class TIMBER

{
public:

long minTimberSize,    //Mininimum timber size.
     maxTimberSize,    //Maximum timber size.
     meanTimberSize,   //Mean timber size.
     meanReturn;     //Mean timber return interval.

char harvestSpecies[15],
     yearlyFn[FNSIZE],
     finalFn[FNSIZE],
     logFn[FNSIZE];     
   
void read(FILE*);    //This will read timber parameters from a wind file.
void write(FILE*);   //This will write timber parameters to a wind file.
void dump();         //This will write timber parameters to the CRT screen.
void activate(int, int freq[]); //Nim: added int freq[]
                     //This will activate timber for a single Landis iter.
                     //The input value is the iteration number.
void initColors(int*, int*, int*); //Sets red, green, and blue palette
                                   //information.		     
void initLogFile();  //This will initialize the timber log file.

private:
int damage(FL_POINT, int&);
int disturb(int, int, int, MAP8&, FILE*);

int *red, *green, *blue;    //Pointer to color pallete.

class MAP8 m,        //Map of damage for iteration.
           cummMap;  //Map of damage for model run.
};

#endif
