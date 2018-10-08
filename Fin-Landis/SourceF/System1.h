#ifndef SYSTEM1_H
#define SYSTEM1_H

#include <stdio.h>

#undef min
#undef max

int fscanc(FILE *fp, char *fmt, ...);
// fscanc: fscanf function with one more feature: jump the comment delimited
// by # pairs. It'll return TRUE, if success, otherwise FALSE.

int eof(FILE*);
//This will return TRUE if eof is encountered and FALSE otherwise.  It is
//different from the feof library function in that it skips all white space
//before detecting the end of file condition.

int min(int, int);       //Returns the minimum of two integers.
float min(float, float); //Returns the minimum of two floats.
int max(int, int);       //Returns the maximum of two integers.
float max(float, float); //Returns the maximum of two floats.

void skipblanks(FILE*);
//This will skip all white space in a file.

// all random number calls automaticaly call fseed

void fseed(int);
//This will seed the random number generator.

//Nim: commented out the next line
//int intrand();
//returns an integer random number

float frand();
//This returns a random number.

double drand();
//This returns a random number.

int irand(int a, int b);
//This will return a random integer between a and b inclusive.

float frandNorm();
//This returns a random number normally distributed around 0.

float franCos();

//float franII();

void swap(float&, float&);
//This will swap two floating point values.

int pseudoPoisson(float);
//This returns a Poisson-distributed random variable with a given mean.

#endif


