#ifndef IO_H
#define IO_H

#include "fireB.h"
#include "wind.h"
#include "timber.h"

void ioInit();
//Initialize io package.

void printMessage(const char*);
//Print message on CRT screen.

void putOutput(int, int, int freq[]);
//This will write out a Landis description for the current replicate and
//iteration.

void ioTerm();
//Uninitialize io package.

void getInput(FILE*, class WIND&, class FIRE&, class TIMBER&,
              int freq[], char reMethods[30][15], char ageMaps[30][15]);
//This will read in all Landis variables.

void help();

void argHelp();

#endif
