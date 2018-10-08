#ifndef AGELIST_H
#define AGELIST_H

#include <stdio.h>


class AGELIST

{

friend class SPECIE; //JP

public:

AGELIST();              //Constructor.
AGELIST(long, long);    //Constructor. JP.
void clear();           //Clear age list.
void set(int);          //Set an age to true.
void reset(int);        //Resest an age to false.
void shiftOld();        //Shift all ages up one to the next age class.
int query();            //Returns true if any ages are present in age list,
                        //false otherwise.
int query(int);         //Return true if age is in list, false otherwise.
int query(int, int);    //Return true if an age exists in the given range
                        //bounded by two integers.
int youngest();         //Return the youngest age.
int oldest();           //Return the oldest age.
int number();           //Return the number of age clases present.
void read(FILE*);       //This shall read the age list from a file.
void write(FILE*);      //This shall write the age list to a file.
void dump();            //This shall dump the age list to the CRT.
long getYoung(){return young;};           //getters for accessing data. JP.
long getOld(){return old;};             //getters for accessing. JP.

protected:

unsigned long young, old; //The data is held in two long 32 bit integers.  For
                          //transportability this relation must be held.  If
                          //32 bit integers are not available this class must
                          //be reprogrammed.
};

#endif
