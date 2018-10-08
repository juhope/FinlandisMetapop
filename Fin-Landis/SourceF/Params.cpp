#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include "error.h"
#include "system1.h"
#include "params.h"


PARAMETERS::PARAMETERS()
//Constructor.

{
//JP: Brought new-allocations here from read function. This is better if
//new parameters were somehow read in during model run.
specAttrFile=new char[FNSIZE];
landUnitFile=new char[FNSIZE];
siteInFile=new char[FNSIZE];
reclassInFile=new char[FNSIZE];
reclassOutFile=new char[FNSIZE];
ageIndexFile=new char[FNSIZE];
outputDir=new char[FNSIZE];
disturbance=new char[FNSIZE];
default_plt=new char[FNSIZE];
freq_out_put=new char[FNSIZE];
numberOfIterations=0;
numberOfReplicates=0;
randSeed=0;
PROB1 = 0;
SIZE1 = 0;
PROB2 = 0;
SIZE2 = 0;
checkFire=0;
checkWind=0;
#ifdef __HARVEST__
harvest = 0;
standAdjacencyFlag=0;//JP
harvestDecadeSpan = 0;
harvestThreshold = 0;
harvestFile=new char[FNSIZE];
standMapFile=new char[FNSIZE];
mgtAreaMapFile=new char[FNSIZE];
harvestOutputFile1=new char[FNSIZE];
harvestOutputFile2=new char[FNSIZE];
#endif
}


PARAMETERS::~PARAMETERS()
//Destructor.

{
delete [] specAttrFile;
delete [] landUnitFile;
delete [] siteInFile;
delete [] reclassInFile;
delete [] reclassOutFile;
delete [] ageIndexFile;
delete [] outputDir;
delete [] disturbance;
delete [] default_plt;
delete [] freq_out_put;
}


void PARAMETERS::read(FILE* infile)
//Read in all parameters from a file.

{
char dispType[40];

if (fscanc(infile,"%s",specAttrFile)!=1)
	 errorSys("Error reading in specAttrFile from parameter file.",STOP);
if (fscanc(infile,"%s",landUnitFile)!=1)
	 errorSys("Error reading in landUnitFile from parameter file.",STOP);
if (fscanc(infile,"%s",siteInFile)!=1)
	 errorSys("Error reading in  GIS File from parameter file.",STOP);
if (fscanc(infile,"%s",reclassInFile)!=1)
	 errorSys("Error reading in reclassInFile from parameter file.",STOP);
if (fscanc(infile,"%s",reclassOutFile)!=1)
	 errorSys("Error reading in reclassOutFile from parameter file.",STOP);
if (fscanc(infile,"%s",ageIndexFile)!=1)
	 errorSys("Error reading in ageIndexFile from parameter file.",STOP);
if (fscanc(infile,"%s",outputDir)!=1)
	 errorSys("Error reading in outputDir from parameter file.",STOP);
if (fscanc(infile,"%s",disturbance)!=1)
	 errorSys("Error reading in distRegFile from parameter file.",STOP);
if (fscanc(infile,"%s",default_plt)!=1)
	 errorSys("Error reading in default.plt from parameter file.",STOP);
if (fscanc(infile,"%s",freq_out_put)!=1)
	 errorSys("Error reading in freq.out.put from parameter file.",STOP);
if (fscanc(infile,"%d",&numberOfIterations)!=1)
	 errorSys("Error reading in numberOfIterations from parameter file.",STOP);
if (fscanc(infile,"%d",&randSeed)!=1)
	 errorSys("Error reading in randSeed from parameter file.",STOP);
if (fscanc(infile,"%d",&cellSize)!=1)
	 errorSys("Error reading in cellSize from parameter file.",STOP);
if (cellSize<=0)
   errorSys("Error: cellSize less than 1 meter.",STOP);
if (fscanc(infile,"%s",dispType)!=1)
	 errorSys("Error reading in dispRegime from parameter file.",STOP);
if (fscanc(infile,"%d",&wind)!=1)
	 errorSys("Error reading in windthrow flag from parameter file.",STOP);
if (fscanc(infile,"%d",&fire)!=1)
	 errorSys("Error reading in fire flag from parameter file.",STOP);
if (fscanc(infile,"%d",&timber)!=1)
	 errorSys("Error reading in timber flag from parameter file.",STOP);
if (fscanc(infile,"%d",&reproFlag)!=1) //JP
	 errorSys("Error reading in establishment flag from parameter file.",STOP);
if (fscanc(infile,"%d",&probFlag)!=1) //JP
	 errorSys("Error reading in fire probability flag from parameter file.",STOP);
if (fscanc(infile,"%d",&PROB1)!=1)
	 errorSys("Error reading in wind probability coefficient from parameter file.",STOP);
if (fscanc(infile,"%d",&SIZE1)!=1)
	 errorSys("Error reading in wind size coefficient from parameter file.",STOP);
if (fscanc(infile,"%d",&PROB2)!=1)
	 errorSys("Error reading in fire probability coefficient from parameter file.",STOP);
if (fscanc(infile,"%d",&SIZE2)!=1)
	 errorSys("Error reading in fire size coefficient from parameter file.",STOP);
if (fscanc(infile,"%f",&checkWind)!=1)
	 errorSys("Error reading in wind checking ratio from parameter file.",STOP); // JP.fire<>wind
if (fscanc(infile,"%f",&checkFire)!=1)
	 errorSys("Error reading in fire checking ratio from parameter file.",STOP);
if (fscanc(infile,"%d",&suppression)!=1)
	 errorSys("Error reading in suppression year from parameter file.",STOP);
#ifdef __HARVEST__
if (fscanc(infile,"%d",&harvest)!=1)
	 errorSys("Error reading in harvest event flag from parameter file.",STOP);
if (fscanc(infile,"%d",&standAdjacencyFlag)!=1)
	 errorSys("Error reading in stand adjacency flag from parameter file.",STOP);
if (fscanc(infile,"%d",&harvestDecadeSpan)!=1)
	 errorSys("Error reading in harvest decade span from parameter file.",STOP);
if (fscanc(infile,"%f",&harvestThreshold)!=1)
	 errorSys("Error reading in harvest threshold from parameter file.",STOP);
if (fscanc(infile,"%s",harvestFile)!=1)
	 errorSys("Error reading in harvestFile from parameter file.",STOP);
if (fscanc(infile,"%s",standMapFile)!=1)
	 errorSys("Error reading in standMapFile from parameter file.",STOP);
if (fscanc(infile,"%s",mgtAreaMapFile)!=1)
	 errorSys("Error reading in mgtAreaMapFile from parameter file.",STOP);
if (fscanc(infile,"%s",harvestOutputFile1)!=1)
	 errorSys("Error reading in harvestOutputFile1 from parameter file.",STOP);
if (fscanc(infile,"%s",harvestOutputFile2)!=1)
	 errorSys("Error reading in harvestOutputFile2 from parameter file.",STOP);
#endif
if (strcmp(dispType,"NO_DISPERSAL")==0)
	 dispRegime=NO_DISPERSAL;
else if (strcmp(dispType,"UNIFORM")==0)
	 dispRegime=UNIFORM;
else if (strcmp(dispType,"NEIGHBORS")==0)
	 dispRegime=NEIGHBORS;
else if (strcmp(dispType,"DISPERSAL")==0)
	 dispRegime=DISPERSAL;
else if (strcmp(dispType,"EFF_DISPERSAL")==0)
	 dispRegime=EFF_DISPERSAL;
else if (strcmp(dispType,"RAND_ASYM")==0)
	 dispRegime=RAND_ASYM;
else if (strcmp(dispType,"MAX_DIST")==0)
	 dispRegime=MAX_DIST;
else if (strcmp(dispType,"SIM_RAND_ASYM")==0)
	 dispRegime=SIM_RAND_ASYM;
else
	 errorSys("Input file error or illegal seed dispersal routine." ,STOP);
}


void PARAMETERS::dump()
//This shall dump the contents of parameters.

{
printf("specAttrFile:       %s\n",specAttrFile);
printf("landUnitFIle:       %s\n",landUnitFile);
printf("reclassInFile:      %s\n",reclassInFile);
printf("reclassOutFile:     %s\n",reclassOutFile);
printf("outputDir:          %s\n",outputDir);
printf("disturbance:        %s\n",disturbance);
printf("default_plt:        %s\n",default_plt);
printf("freq_out_put:		 %s\n",freq_out_put);
printf("numberOfIterations: %d\n",numberOfIterations);
printf("numberOfReplicates: %d\n",numberOfReplicates);
printf("randSeed:           %d\n",randSeed);
printf("cellSize:           %d\n",cellSize);
switch (dispRegime)
	{
	 case NO_DISPERSAL:  printf("dispRegime:         NO_DISPERSAL\n"); break;
	 case UNIFORM:       printf("dispRegime:         UNIFORM\n"); break;
	 case NEIGHBORS:     printf("dispRegime:         NEIGHBORS\n"); break;
	 case DISPERSAL:     printf("dispRegime:         DISPERSAL\n"); break;
	 case RAND_ASYM:     printf("dispRegime:         RAND_ASYM\n"); break;
	 case MAX_DIST:      printf("dispRegime:         MAX_DIST\n"); break;
	 case SIM_RAND_ASYM: printf("dispRegime:         SIM_RAND_ASYM\n"); break;
	 default: errorSys("PARAMETERS::dump()-> Illegal dispRegime",STOP);
	}
printf("wind flag:              %d\n",wind);
printf("fire flag:              %d\n",fire);
printf("timber flag:            %d\n",timber);
printf("establishment flag:     %d\n",reproFlag);   //JP
printf("fire probability flag:  %d\n",probFlag);    //JP
#ifdef __HARVEST__
printf("harvest flag:         %d\n",harvest);
printf("stand adjacency flag: %d\n",standAdjacencyFlag);
printf("harvest decade span:  %d\n",harvestDecadeSpan);
printf("harvest threshold:    %f\n",harvestThreshold);
printf("harvest event file:   %s\n",harvestFile);
printf("stand map file:       %s\n",standMapFile);
printf("m.a. map file:        %s\n",mgtAreaMapFile);
printf("stand report file:    %s\n",harvestOutputFile1);
printf("m.a. report file:     %s\n",harvestOutputFile2);
#endif
}


void PARAMETERS::write(FILE* outfile)
//This shall write the contents of parameters to outfile

{
fprintf(outfile,"%s\n",specAttrFile);
fprintf(outfile,"%s\n",landUnitFile);
fprintf(outfile,"%s\n",reclassInFile);
fprintf(outfile,"%s\n",reclassOutFile);
fprintf(outfile,"%s\n",outputDir);
fprintf(outfile,"%s\n",disturbance);
fprintf(outfile,"%s\n",default_plt);
fprintf(outfile,"%s\n",freq_out_put);
fprintf(outfile,"%d\n",numberOfIterations);
fprintf(outfile,"%d\n",numberOfReplicates);
fprintf(outfile,"%d\n",randSeed);
fprintf(outfile,"%d\n",cellSize);
switch (dispRegime)
	{
	 case NO_DISPERSAL:  fprintf(outfile,"NO_DISPERSAL\n"); break;
	 case UNIFORM:       fprintf(outfile,"UNIFORM\n"); break;
	 case NEIGHBORS:     fprintf(outfile,"NEIGHBORS\n"); break;
	 case DISPERSAL:     fprintf(outfile,"DISPERSAL\n"); break;
	 case RAND_ASYM:     fprintf(outfile,"RAND_ASYM\n"); break;
	 case MAX_DIST:      fprintf(outfile,"MAX_DIST\n"); break;
	 case SIM_RAND_ASYM: fprintf(outfile,"SIM_RAND_ASYM\n"); break;
	 default: errorSys("PARAMETERS::write()-> Illegal dispRegime.",STOP);
	}
fprintf(outfile,"%d\n",wind);
fprintf(outfile,"%d\n",fire);
fprintf(outfile,"%d\n",timber);
fprintf(outfile,"%d\n",reproFlag);                //JP
fprintf(outfile,"%d\n",probFlag);                 //JP
#ifdef __HARVEST__
fprintf(outfile,"%d\n",harvest);
fprintf(outfile,"%d\n",standAdjacencyFlag);
fprintf(outfile,"%d\n",harvestDecadeSpan);
fprintf(outfile,"%f\n",harvestThreshold);
fprintf(outfile,"%s\n",harvestFile);
fprintf(outfile,"%s\n",standMapFile);
fprintf(outfile,"%s\n",mgtAreaMapFile);
fprintf(outfile,"%s\n",harvestOutputFile1);
fprintf(outfile,"%s\n",harvestOutputFile2);
#endif
}

