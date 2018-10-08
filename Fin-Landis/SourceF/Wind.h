#ifndef WIND_H
#define WIND_H

#include "defines.h"
#include "point.h"
#include "map8.h"

class WIND

{
public:

long minWindSize,    //Mininimum windthrow size.
	  maxWindSize,    //Maximum windthrow size.
	  meanWindSize,   //Mean windthrow size.
	  meanReturn;     //Mean windthrow return interval.

int   meanLower,	 	//pseudomean lower boundary in percentage
		meanUpper,		//pseudomean upper boundary in percentage
		strong,			//pseudostrong boundary in percentage
		light;			//pseudolight boundary in percentage

char yearlyFn[FNSIZE],
	  finalFn[FNSIZE],
	  logFn[FNSIZE];

void read(FILE*);    //This will read windthrow parameters from a wind file.
void write(FILE*);   //This will write windthrow parameters to a wind file.
void dump();         //This will write windthrow parameters to the CRT screen.
void activate(int, int freq[]); //Nim: added int freq[]
                     //This will activate windthrow for a single Landis iter.
							//The input value is the iteration number.
void initColors(int*, int*, int*); //Sets red, green, and blue palette
											  //information.
void initLogFile();  //This will initialize the windthrow log file.


private:
int damage(FL_POINT, int&);
int disturb(int, int, int, MAP8&, FILE*,double);

int *red, *green, *blue;    //Pointer to color pallete.

class MAP8 m,        //Map of damage for iteration.
           cummMap;  //Map of damage for model run.
};

#endif
