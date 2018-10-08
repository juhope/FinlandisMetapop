#ifndef PARAMETERS_H
#define PARAMETERS_H

#define NO_DISPERSAL  0     //No seed dispersal.
#define UNIFORM       1     //Uniform seed dispersal.
#define NEIGHBORS     2     //Seed to immediate neighbors.
#define EFF_DISPERSAL 3     //Seed within effective distance.
#define DISPERSAL     4     //Seed within effective distance and some within maximum.
#define RAND_ASYM     5     //Seed using interpolated chaotic distances.
#define MAX_DIST      6     //RAND_ASYM up to maximum distances.
#define SIM_RAND_ASYM 7     //RAND_ASYM up to maximum distances, Simulated
									//random asymptopic after maximum distance.
#include <stdio.h>

class  PARAMETERS
  {
	public:

   char
     *specAttrFile,//Species Attributes input file name.
	  *landUnitFile,     //Land unit input file name.
 	  *siteInFile,       //Site input file name.
	  *reclassInFile,  	//Reclassification input file name.
	  *reclassOutFile, 	//Reclassification output file name.
	  *outputDir,    	  	//Output directory.
	  *disturbance,	   //Disturbance regime file name.
	  *default_plt,      //For map output
	  *freq_out_put,     //Output map frequency file
     *ageIndexFile;     //species age output index file
#ifdef __HARVEST__
   char
	*harvestFile,		//Harvest event input file name.
	*standMapFile,      //Stand identifier map file name.
	*mgtAreaMapFile,    //Management area map file name.
	*harvestOutputFile1,//Harvest output file name for stand report.
	*harvestOutputFile2;//Harvest output file name for management area report.
#endif
	int
	  wind,              //Turn wind disturbances on/off.
	  fire,              //Turn fire disturbances on/off.
	  timber,            //Turn timber disturbances on/off.
      reproFlag,         //changing establishment coeffs on/off
      probFlag,          //mode of fire probability change
#ifdef __HARVEST__
     harvest,           //Turn harvest events on/off.
     standAdjacencyFlag,//Turn stand adjacency flag on/off.
     harvestDecadeSpan, //Sites harvested within this time span are considered
                        //"recently harvested".
#endif
	  numberOfIterations,//Number of iterations in model.
	  numberOfReplicates,//Number of replicates in model.
	  randSeed,          //Random number seed.
	  cellSize,          //Length of side of cell in meters.
	  dispRegime,        //Seeding regime: NO_DISPERSAL, UNIFORM, NEIGHBORS,
			      			//DISPERSAL, or INTERPOLATE.
	  PROB1, 				//Wind disturbance probability coefficient
	  SIZE1, 				//Wind disturbance size coefficient
	  PROB2,             //Fire disturbance probability coefficient
	  SIZE2,             //Fire disturbance size coefficient
     suppression;       //First year of fire suppression

#ifdef __HARVEST__
   float  harvestThreshold;  //If the number of "recently harvested" sites on a
                        //divided by the number of active sites on a stand
                        //is greater than or equal to this value, consider
                        //the stand "recently harvested".
#endif

   float
     checkFire,  			//ignition ratio (checking 1/checkPct of the points)
     checkWind;      	//breakout ratio (checking 1/checkPct of the points)

	  PARAMETERS();      //Constructor.
	  ~PARAMETERS();     //Destructor.
	  void read(FILE*);  //Read in all parameters from a file.
	  void write(FILE*); //Write all parameters to a file.
	  void dump();       //Dump all parameters to the CRT.
  };

#endif

