#ifndef PILE_H
#define PILE_H

#include "point.h"

#define MAXPILE 25000   //changed from 25000 to 16000

class PILE
//Pile ADT.  Accepts FL_POINT as input and output.

{
public:

PILE();             //Constructor.
int isEmpty();      //This returns true if the pile is empty.
int push(FL_POINT p);  //Pushes p onto the pile.  Returns true if unsucceful,
                    //false otherwise.
FL_POINT pull();       //Pulls p off of the pile.
void reset();       //This will reset the pile to an empty state.

private:

FL_POINT  items[MAXPILE];  //Array of pile items;
int numItems;         //Number of items on pile.
};

#endif
