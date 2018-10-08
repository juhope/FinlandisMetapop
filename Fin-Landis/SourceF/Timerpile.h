#ifndef TIMERPILE_H
#define TIMERPILE_H

#include "timerpoint.h"

#define MAXPILE 25000   //changed from 25000 to 16000

class TIMERPILE
//Timerpile ADT.  Accepts TIMERPOINT as input and output.

{
public:

TIMERPILE();             //Constructor.
int isEmpty();      //This returns true if the pile is empty.
int push(TIMERPOINT p);  //Pushes p onto the pile.  Returns true if unsucceful,
                    //false otherwise.
int getCount() {return count;};        //Gets count.
int getFrontSize() {return numItems;}; //Gets fire front size.
TIMERPOINT pull();       //Pulls p off of the pile.
void reset();       //This will reset the pile to an empty state.

private:

TIMERPOINT  items[MAXPILE];  //Array of pile items;
int numItems;         //Number of items on pile.
int count;      //Time taken by last pull.
};

#endif