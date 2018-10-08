#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include "error.h"
#include "landu.h"
#include "specatt.h"
#include "specie.h"
#include "landis.h"
#include "optim.h"

OPTIM::OPTIM()
//Constructor.

{
specNum=0;
rows=0;
cols=0;
active=NULL;
mature=NULL;
}


OPTIM::~OPTIM()
//Destructor.

{
if(active)
	delete [] active; //Nim: added []
if(mature)
	delete [] mature; //Nim: added []
}

void OPTIM::dim(int sn, int r, int c)
//Constructor.  First Parameters are number of
//species, number of rows and columns.

{
specNum=sn;
rows=r;
cols=c;
if (active)
    delete [] active; //JP just in case
if (mature)
    delete [] mature; //JP just in case

active=new unsigned char[specNum];
mature=new unsigned char[specNum*rows*cols];
}

void OPTIM::initialize()
//This will initialize or reinitialize the map.


{
int i, j, snr, snc, specAtNumb;
specAtNumb=speciesAttrs.number();

for (i=1;i<=specAtNumb;i++)
    if (speciesAttrs(i)->effectiveD<0)       //JP
        active[i-1]=true;     //Species with effectiveD -1 are always active.
    else                      //(Used to be Popu always active.)
        active[i-1]=false;


snr=sites.numRows();
snc=sites.numColumns();

for (i=1;i<=snr;i++)
    for (j=1;j<=snc;j++)
        {
         //Define Land unit.
         LANDUNIT *l=sites(i,j)->landUnit;
         if (l!=NULL && l->active())
              {
              SPECIE *s=sites(i,j)->first();
              SPECIESATTR *sa=sites(i,j)->specAtt();
              int speciesNum=1;

              //Inner loop; Loops through all species on a site.
              while (s!=NULL)
              {
                 if (s->query() || s->vegPropagules || s->disPropagules) //JP
                   {
                    active[speciesNum-1]=true;
                   }
                 if (s->oldest()>=sa->maturity || s->disPropagules) //JP
                  mature[((speciesNum-1)*rows*cols)+((i-1)*cols)+(j-1)]=true;
                 else
                  mature[((speciesNum-1)*rows*cols)+((i-1)*cols)+(j-1)]=false;
                 // Loop to next species and species attribute.
                 s=sites(i,j)->next();
                 sa=sites(i,j)->specAtt();
                 speciesNum++;
              }
            }
          else
            {
             for (int speciesNum=1;speciesNum<=specAtNumb;speciesNum++)
                   mature[((speciesNum-1)*rows*cols)+((i-1)*cols)+(j-1)]=false;
            }
       }
}

int OPTIM::speciesActive(int sn)
//This will return true if a species referrenced
//by number is active.
   /// 28JUL94_CSH
{
if (sn>0 && sn<=speciesAttrs.number())
    return active[sn-1];
else
{
   errorSys("ERROR:  Illegal species number OPTIM::speciesActive",STOP);
   return -999;
}
}

unsigned char* OPTIM::operator()(int sn, int r,int c)
//This will return a referrence to the appropriate species, row, column
//cordinate in the maturity array.
{
int ref;

if (sn<1 || sn>speciesAttrs.number() ||
    r<1 || r>rows ||
    c<1 || c>cols)
    errorSys("OPTIM bounds error",STOP);

sn--;
r--;
c--;
ref=(sn*rows*cols)+(r*cols)+c;
return &mature[ref];
}

void OPTIM::dump()
//This dumps some simple statistics on seed availability.

{
int i,j,k;
int p=0;

for (i=0;i<18;i++)
  {
   int number=0;
   for (j=0;j<rows;j++)
       for (k=0;k<cols;k++)
           if (mature[p++])
	       number++;
   printf("Species=%d, Number=%d\n",i+1,number);
  }
}

