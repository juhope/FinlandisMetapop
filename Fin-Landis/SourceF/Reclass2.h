#ifndef RECLASS2_H
#define RECLASS2_H

#include "map8.h"

extern char reMethods[30][15],ageMaps[30][15];
void fileReclass(MAP8&, const char*);
//This will reclass the Landis map and return the result m.  The map will
//be reclassified using a method defined in a an output reclassification
//text file.

#endif

