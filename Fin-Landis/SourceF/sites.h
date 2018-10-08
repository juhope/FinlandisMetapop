
#ifndef SITES_H
#define SITES_H

#include <stdio.h>
#include "site.h"
#include "landus.h"

class SITES

{
public:

SITES(int, int, int);             //Constructor.  The first parameter is the
                                  //number of species in the model.  The other
                                  //parameters dimension the number of
                                  //rows and columns on the map.
SITES();                          //Constructor.  Initial values must be
                                  //set later by the dim method.  This is
                                  //useful so that a SITES object may be
                                  //declared statically but set up
                                  //dynamically.
~SITES();                         //Destructor.
void dim(int,int,int);            //This will set the number of species,
                                  //rows and columns in the map.
SITE* operator() (int, int);      //This will return a pointer to a singular
                                  //site.  It is referrenced by row and column
                                  //number.
void read(FILE*);                 //Read in all site data.
void write(FILE*);                //Write out all site data.
void dump();                      //Dump set of site data to the CRT.
void attach(LANDUNITS*);          //This attaches a set of land units to all
                                  //sites.
void setHeader(unsigned long []); //Set the header info for sites
unsigned long* getHeader();		 //Copy the header info of sites.
int number();			  				 //This will return the total number of sites.
int numRows();                    //Returns number of rows of sites.
int numColumns();                 //Returns number of columns of sites.
int numActive();                  //Returns number of active sites on the map.


private:
SITE* map;                        //This is the internal represenation of the
                                  //sites map.
int rows, columns;                //Number of rows and columns contained in the
                                  //whole map.
unsigned long header[32];			 //Upperleft coordinates of sites
};

#endif
