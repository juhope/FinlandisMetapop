#ifndef FIRE_H
#define FIRE_H

#include "defines.h"
#include "point.h"
#include "map8.h"


class FIRE

{
public:

long minFireSize,    //Mininimum fire size.
	  maxFireSize,    //Maximum fire size.
	  meanFireSize;   //Mean fire size.

int   meanLower,	 	//pseudomean lower boundary in percentage
		meanUpper,		//pseudomean upper boundary in percentage
		strong,			//pseudostrong boundary in percentage
		light;			//pseudolight boundary in percentage

char yearlyFn[FNSIZE],
	  finalFn[FNSIZE],
	  logFn[FNSIZE],
	  logFn2[FNSIZE];

float lifespan[5];      //lifespan proportions for fire susceptibility
int   susClass[5];  //fire susceptibilities corresponding to lifespan. JP.
int ignnode[30];  //years to define ignition frequency
float ignitions[30]; //ignition numbers or AAB's for node years

void read(FILE*);    //This will read fire parameters from a fire file.
void write(FILE*);   //This will write fire parameters to a fire file.
void dump();         //This will write fire parameters to the CRT screen.*/
void activate(int, int freq[]); //Nim: added int freq[]
					 //This will activate fire for a single Landis iter.
							//The input value is the iteration number.
void initColors(int*, int*, int*); //Sets red, green, and blue palette
											  //information.
void initLogFile();  //This will initialize the fire log file.

private:
int susceptibility(float);  //JP
unsigned char fireClass(SITE*,unsigned char,double);  //JP
int damage(TIMERPOINT, int);  //JP
int disturb(int, int, int, int, int, class MAP8&, class MAP8&, FILE*, double, unsigned char); //JP //HF: added one int

int burnable;
int burns[10];
int igns[10];
int ignTotal;
int burnTotal;

int *red, *green, *blue;    //Pointer to color pallete.

class MAP8  m,                  //Map of damage for iteration.
			cummMap;            //Map of damage for model run.
};

#endif

