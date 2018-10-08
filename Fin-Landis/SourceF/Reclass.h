#ifndef RECLASS_H
#define RECLASS_H

#include "map8.h"

void luReclass(MAP8&);
//This will perform a reclassification based on the underlying map 
//land units.  It will perform the reclassification on the global 
//object sites and place the results in the map parameter.

void ageReclass(MAP8 &m);
//This will perform a reclassification based upon the oldest cohort upon
//a landis stand.  The cohorts will be scaled into 16 age classes.

void ageReclassDense(MAP8 &m);
//Same as before, but only dense cohorts accounted. JP.

void speciesAgeMap(MAP8 &m, char *fname);
//This will faciliate age output at 10 year step for species specified in
//species age index file.

void speciesAgeMapDense(MAP8 &m, char *fname);
//Same as before, but only dense cohorts accounted. JP.

#endif

