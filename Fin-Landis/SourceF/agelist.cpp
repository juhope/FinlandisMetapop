#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> // JP
#include "defines.h"
#include "system1.h"
#include "error.h"
#include "agelist.h"

///////////////////////////////////////////////////////////////////////////////
//			CONSTANT DECLARATIONS				     //
///////////////////////////////////////////////////////////////////////////////

//The following masks are used by the AGELIST class to process bits.  lowMask
//is a mask with the low bit set.  highMask has the high bit set and fullMask
//has all bits sets.  Mask is an array with 32 elements, each having their
//respective bit set.

static const unsigned long lowMask=0x00000001;
static const unsigned long highMask=0x80000000;
static const unsigned long fullMask=0XFFFFFFFF;
static const unsigned long mask[32] =
{
0x00000001, 0x00000002, 0x00000004, 0x00000008,
0x00000010, 0x00000020, 0x00000040, 0x00000080,
0x00000100, 0x00000200, 0x00000400, 0x00000800,
0x00001000, 0x00002000, 0x00004000, 0x00008000,
0x00010000, 0x00020000, 0x00040000, 0x00080000,
0x00100000, 0x00200000, 0x00400000, 0x00800000,
0x01000000, 0x02000000, 0x04000000, 0x08000000,
0x10000000, 0x20000000, 0x40000000, 0x80000000
};


//////////////////////////////////////////////////////////////////////////////
//			    AGELIST CLASS CODE				    //
//////////////////////////////////////////////////////////////////////////////

AGELIST::AGELIST()
//Constructor.

{
young=0;
old=0;
}

AGELIST::AGELIST(long y, long o)
//Constructor

{
young=y;
old=o;
}

void AGELIST::clear()
//Clear agelist.

{
young=0;
old=0;
}

void AGELIST::set(int age)
//Set an age to true.

{
age=age/10-1;
#ifdef BOUNDSCHECK
if (age<0 || age>=64)
   errorSys("AGELIST::set()-> Illegal age.",STOP);
#endif

if (age<=31)
    {
     young |= mask[age];
    }
else
    {
     old |= mask[age-32];
    }
}

void AGELIST::reset(int age)
//Reset an age to false.

{
age=age/10-1;
#ifdef BOUNDSCHECK
if (age<0 || age>=64)
   errorSys("AGELIST::reset()-> Illegal age.",STOP);
#endif

if (age<=31)
    {
     young &= ~mask[age];
    }
else
    {
     old &= ~mask[age-32];
    }
}

void AGELIST::shiftOld()
//Shift all ages up one to the next age class.

{
old <<= 1;
if (young & highMask)
    old |= lowMask;
young <<= 1;
}

int AGELIST::query()
//Returns true if any age clases are present, false otherwise.

{
return young || old;
}

int AGELIST::query(int age)
//Return true if an age exists in the list, false otherwise.

{
age=age/10-1;
#ifdef BOUNDSCHECK
if (age<0 || age>=64)
   errorSys("AGELIST::query(int)-> Illegal age.",STOP);
#endif

if (age<=31)
    {
     return young&mask[age];
    }
else
    {
     return old&mask[age-32];
    }
}

int AGELIST::query(int low, int high)
//Return true if an age exists in the range bounded by low and high,
//false otherwise.

{
int i;

low=low/10-1;
#ifdef BOUNDSCHECK
if (low<0 || low>=64)
   errorSys("AGELIST::query(int,int)-> Illegal lower bound.",STOP);
#endif
high=high/10-1;
#ifdef BOUNDSCHECK
if (high<0 || high>=64)
   errorSys("AGELIST::query(int,int)-> Illegal upper bound.",STOP);
if (low>high)
   errorSys("AGELIST::query(int,int)-> Lower bound is greater than\
             upper bound.",STOP);
#endif

for (i=low;i<=min(high,31);i++)
   if (young & mask[i])
      return true;

for (i=max(low-32,0);i<=high-32;i++)
   if (old & mask[i])
       return true;

return false;
}

int AGELIST::youngest()
//Returns the youngest age present.

{
int i;
unsigned long y, o;

y=young;
o=old;

for (i=0;i<32;i++)
   {
    if (y&lowMask)
       return (i+1)*10;
    y>>=1;
   }

for (i=0;i<32;i++)
   {
    if (o&lowMask)
       return (i+1)*10+320;
    o>>=1;
   }
return 0;
}

int AGELIST::oldest()
//Returns the oldest age present.

{
int i;
unsigned long y, o;

y=young;
o=old;

for (i=0;i<32;i++)
   {
    if (o&highMask)
       return (64-i)*10;
    o<<=1;
   }

for (i=0;i<32;i++)
   {
    if (y&highMask)
       return (32-i)*10;
    y<<=1;
   }
return 0;
}

int AGELIST::number()
//Returns the number of age classes present.

{
int i,c=0;
unsigned long y, o;

y=young;
o=old;

for (i=0;i<32;i++)
   {
    if (y&lowMask)
       c++;
    y>>=1;
   }

for (i=0;i<32;i++)
   {
    if (o&lowMask)
        c++;
    o>>=1;
   }
return c;
}


void AGELIST::read(FILE* infile)
//This shall read an age list from a file.

{
char ch;
int j,numRead=0;
clear();
skipblanks(infile);
for (j=0;j<=63 && ((ch=(char)fgetc(infile))!=EOF) && isdigit(ch);j++)
   {
    numRead++;
    young>>=1;
    if (old & lowMask)
         young=young | highMask;
    old>>=1;
    if (ch=='1')
         old=old | highMask;
   }
for (j=1;j<=(64-numRead);j++)
   {
    young>>=1;
    if (old & lowMask)
       young=young|highMask;
    old>>=1;
   }
}

void AGELIST::write(FILE* outfile)
//This will write an age list to a file.

{
int j;
long y, o;

y=young;
o=old;

for (j=0;j<=31;j++)
   {
    if (y&lowMask)
        fprintf(outfile,"1");
    else
        fprintf(outfile,"0");
    y>>=1;
   }

for (j=0;j<=31;j++)
   {
    if (o&lowMask)
        fprintf(outfile,"1");
    else
        fprintf(outfile,"0");
    o>>=1;
   }
}


void AGELIST::dump()
//This will dump an age list to the CRT.

{
int j;
long y, o;

y=young;
o=old;

for (j=0;j<=31;j++)
   {
    if (y&lowMask)
        printf("1");
    else
        printf("0");
    y>>=1;
   }

for (j=0;j<=31;j++)
   {
    if (o&lowMask)
        printf("1");
    else
        printf("0");
    o>>=1;
   }
printf("\n");
}
