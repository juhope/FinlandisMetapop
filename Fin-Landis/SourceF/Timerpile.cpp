#include "defines.h"
#include "system1.h"
#include "error.h"
#include "timerpoint.h"
#include "timerpile.h"

TIMERPILE::TIMERPILE()
//Constructor.

{
numItems=0;
count=0;
}

int TIMERPILE::isEmpty()
//This returns true if the pile is empty, false otherwise.

{
return !numItems;
}

int TIMERPILE::push(TIMERPOINT p)
//Pushes p onto the pile.  Returns true if unsuccesful, false otherwise.

{
if (numItems<(MAXPILE))
   {
    items[numItems++]=p;
    return false;
   }
else
   {
    errorSys("MAXIMUM FIRE FRONT PILE SIZE EXCEEDED!",CONTINUE);
	return true;
   }
}

TIMERPOINT TIMERPILE::pull()
//Pulls p off of the pile.

{
count=0;
TIMERPOINT p;
int pos;

do
  {
  count++;
  pos=(int)(frand()*(float)numItems);
  if (pos>=numItems)
	pos=numItems-1;
  }
while (frand()>1/(float)items[pos].timer);

p=items[pos];
items[pos]=items[numItems-1];
numItems--;
return p;
}

void TIMERPILE::reset()
//This will reset the pile to an empty state.

{
numItems=0;
}