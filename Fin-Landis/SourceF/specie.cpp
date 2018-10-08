#include <stdio.h>
#include "system1.h"
#include "agelist.h"
#include "specie.h"


//////////////////////////////////////////////////////////////////////////////
//			     SPECIE CLASS CODE				    //
//////////////////////////////////////////////////////////////////////////////

static const unsigned long lowMask=0x00000001;
static const unsigned long highMask=0x80000000;
static const unsigned long fullMask=0XFFFFFFFF;


SPECIE::SPECIE()
//Constructor.

{
vegPropagules=0;
disPropagules=0;
//vegPropChecked=0; //JP
AGELIST::clear();
speciesAttr=NULL; //JP
thinCohorts.young=0; //JP
thinCohorts.old=0; //JP
disPropagules=false;//JP
}

SPECIE::~SPECIE()
//Destructor

{
}

void SPECIE::clear()
//Clear all specie values.

{
vegPropagules=0;
disPropagules=0;
//vegPropChecked=0; //JP
AGELIST::clear();
thinCohorts.clear();//JP
disPropagules=false;//JP
}

void SPECIE::attach(SPECIESATTR* a)
//Attach attributes of a species. JP.
{
speciesAttr=a;
}


void SPECIE::kill(int a)
//Kill all trees of age a. JP.
//Added setting veg prop. JP.
{

    if (this->query(a))
        {
        if (a<speciesAttr->maxSproutAge)
                vegPropagules = true;
        reset(a);
        thinCohorts.reset(a);
        }
    //vegPropChecked=true;
    return;
}

void SPECIE::kill(int l, int h)
//Kill all trees bounded by l and h.
//Added setting veg prop. JP.
//Note: vegProb checked only once! JP.
{
if (l<speciesAttr->maxSproutAge &&
            (this->query(l,speciesAttr->maxSproutAge)))
            vegPropagules = true;

for (int i=l;i<=h;i+=10)
    {
    reset(i);
    thinCohorts.reset(i);
    }

//vegPropChecked=true;
return;
}

void SPECIE::kill(int l, int h, AGELIST a)
//Kill all trees bounded by l and h if and only if the age is present in a.
//Added setting veg prop. JP.
//Note: vegProb checked only once! JP.
{
if (l<(speciesAttr->maxSproutAge))
    for (int i=l;i<=(speciesAttr->maxSproutAge);i+=10)
        if ((this->query(i)) && (a.query(i)))
            //if (frand()<speciesAttr->vegProb) Checked in main function.
                {
                vegPropagules = true;
                break;
                }

for (int i=l;i<=h;i+=10)
    if (a.query(i))
        {
        reset(i);
        thinCohorts.reset(i);
        }

//vegPropChecked=true;
return;
}

void SPECIE::killSeeds()
{
disPropagules=false;
}

void SPECIE::seedCheck()
{
disPropagules=false;
if ((this->oldest())>=(this->speciesAttr->maturity))
  disPropagules=true;
return;
}

void SPECIE::birth(float prob, bool thin)
{
set(10);
if (thin || prob<frand()) thinCohorts.set(10);
}

void SPECIE::grow()
{
shiftOld();
thinCohorts.shiftOld();
}

void SPECIE::thin(int age)
{
//Note: at thinning also possibility of vegetative reproduction
    if (this->query(age) && !thinCohorts.query(age))
        {
        if (age<speciesAttr->maxSproutAge)
                vegPropagules = true;
        thinCohorts.set(age);
        }
//vegPropChecked=true;
return;
}

int SPECIE::oldestDense()
{
int i;
unsigned long y, o;


o=old&~thinCohorts.old;
for (i=0;i<32;i++)
   {
    if (o&highMask)
       return (64-i)*10;
    o<<=1;
   }

y=young&~thinCohorts.young;
for (i=0;i<32;i++)
   {
    if (y&highMask)
       return (32-i)*10;
    y<<=1;
   }
return 0;
}

bool SPECIE::shading()
{
for (int i=speciesAttr->maturity/30+10;i<=speciesAttr->longevity;i+=10)
    if (query(i) && (!thinCohorts.query(i))) return true;
return false;
}


void SPECIE::read(FILE *infile)
//Read a specie from a file.

{
int a;
//////
fscanc(infile,"%d ",&a);
//////// its the above line
////fputs("thru bug fscanc/n", stdout);
vegPropagules=(bool)a;
////fputs("vegPropagules=a/n", stdout);
AGELIST::read(infile);
thinCohorts.read(infile);
}

void SPECIE::write(FILE *outfile)
//Write a specie to a file.

{
fprintf(outfile,"%d",vegPropagules);
AGELIST::write(outfile);
thinCohorts.write(outfile);
speciesAttr->write(outfile); //JP
}

void SPECIE::dump()
//Dump a specie to the CRT.

{
printf("%d\t",vegPropagules);
AGELIST::dump();
thinCohorts.dump();
speciesAttr->dump(); //JP
}
