#ifndef OPTIM_H
#define OPTIM_H

class OPTIM

{
public:

OPTIM();                   //Constructor.
~OPTIM();                  //Destructor.

void dim(int, int, int);   //Dimension map size.  Parameters are number of
                           //species, number of rows and columns.

void initialize();         //This will initialize or reinitialize the map.

int speciesActive(int);    //This will return true if a species referrenced
                           //by number is active.

unsigned char* OPTIM::operator()(int, int, int);
                           //This will return a referrence to the appropriate
                           //species, row, column, coordinate in the maturity
                           //array. JP removed.
void dump();		   //This dumps some simple statistics on seed
                           //availability.

private:

unsigned char *active;     //List of active species.
unsigned char *mature;     //Three dimensional array referrenced by SPECIES,
                           //Row, and column.
                           //Not used anymore. JP.
int specNum,               //Number of species.
    rows, cols;            //number of rows and columns.
};

#endif

