#include <stdlib.h>
#include <stdio.h>
#include "system1.h"
#include "specie.h"
#include "sites.h"
#include "landis.h"
#include "reclass3.h"

void reclassify(int timeStep, char ageMaps[30][15])
//This will read in the global variable sites from class file and map file.
//Class file is a file containing site descriptions for a set of class values.
//Map file contains the corresponding map.  Thus every value in mapFile
//is represented by the cooresponding class descritption represented in
//classFile.  mapFile is an Erdas 8 bit gis file.  The file pointer is
//placed on the first map element.  yDim and xDim are the (x,y) dimensions
//of the Erdas map.

{
int coverType, curSp, specAtNum=speciesAttrs.number();
int yDim=sites.numRows();
int xDim=sites.numColumns();
unsigned char c;
unsigned long dest[64];
char str[200],speciesName[10];
FILE	*inAgeMap, *inAgeIndex;

for (int i=0;i<specAtNum;i++)
{
	//read species name from ageIndex file
	sprintf(str,"%s.age",ageMaps[i]);
   if ((inAgeIndex=fopen(str,"r"))==NULL)
	{
		printf("%s%s\n", str," file not found.");
   	exit(255);
	}
   else if (fscanc(inAgeIndex, "%s", speciesName) != 1)
   {
		printf("%s%s\n", "Error, reading file::", str);
   	exit(255);
	}
   curSp=speciesAttrs.current(speciesName);
   //read age map file from output directory
	sprintf(str,"%s/%s%d.gis",parameters.outputDir,ageMaps[i],timeStep);
   if ((inAgeMap=fopen(str, "rb"))==NULL)
	{
		printf("%s%s\n", str," file not found.");
   	exit(255);
	}
   else
   {
   	fread((char*)dest,4,32,inAgeMap);
   }
   // read inAgeMap
   for (int i=yDim;i>0;i--)
	{
		for (int j=1;j<=xDim;j++)
   	{
   		fread((unsigned char*)(&c),1,1,inAgeMap);
	  		coverType=(unsigned int)c;
         if (coverType==255)          //species absence
         {
         	SPECIE *s=sites(i,j)->current(curSp);
            s->clear();
         }
			else if (//(sites(i,j)->landUnit->active())&&
      	  	(coverType!=255)&&(coverType>=3)) //0-empty 1-water 2-nonforest
         {
         	SPECIE *s=sites(i,j)->current(curSp);
            s->clear();
            s->set((coverType-2)*10);
         }
   	}
	}
   fclose(inAgeMap);
}
}

