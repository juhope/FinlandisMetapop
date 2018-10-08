#ifndef SPECIESATTR_H
#define SPECIESATTR_H

class SPECIESATTR

{
public:

char* name;           //Species Name.
int   longevity,      //Maximum age.
      senescence,     //Age when cohorts start disappearing
      maturity,       //Sexual Maturity.
      shadeTolerance, //Shade Tolerance.
      fireTolerance,  //Fire Tolerance.
      serotiny,       //Highest fire intensity the seeds on trees survive
      effectiveD,     //Effective seeding distance.
      maxD;           //Maximum seeding distance.
float vegProb,        //Probability of vegetative seeding after a fire
      vegProb2;       //... when lastFire > 0
int   maxSproutAge;   //Maximum sprouting age.
float reclassCoef;    //Reclassification coefficient.

SPECIESATTR();        //Constructor.
~SPECIESATTR();       //Destructor.
void read(FILE*);     //Read species attributes from a file.
void write(FILE*);    //Write species attributes to a file.
void dump();          //Dump species attributes to the CRT.
int distance(float);  //Given a probability this will interpolate actual
                      //and effective distance and return the interpolated
                      //distance.  Assume there is a 95% probability of
                      //seeding within the actual distance and 5% probability
                      //of seeding within the maximum distance.
float prob(float x);
//Given a distance this will return a seeding probability for a species.

};

#endif

