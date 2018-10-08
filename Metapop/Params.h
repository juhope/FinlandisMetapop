#ifndef PARAMS_H
#define PARAMS_H

#include <fstream.h>

class PARAMS
  {
  public:
    int     sp1MinAge,  //Min age of sp1 that counts as substrate. -1 -> Aspen not used.
            sp2MinAge, //Min age of sp2 that counts as substrate. -1 -> Willow not used.
            maturity, //How many years after colonization starts to reproduce.
            startyear, //Year of Fin-Landis output to begin with
            simulationlength, //Number of years
            timestep, //Length of iteration in years
            outputfreq,
            coordinatefreq, //Frequency of creating output maps
            curve,
            boundary,
            aggregation,
            exactradius,
            rows,
            cols,
            roffset,
            coffset,
            mrows,
            mcols,
            NWx,
            NWy,
            useCoordinates;

    float   alpha,    //Defines dispersal curve.
            beta,
            c, //HF: Added for 2p exp dispersal kernel
			colonization1, //Defines colonization rate.
            colonization2, //Defines colonization rate.
            exponent1,     //Defines effect of host 1 age on colonisation rate
            exponent2,     //Defines effect of host 2 age on colonisation rate
            mortality,  //Defines mortality rate (apart from deterministic mortality, when substrate removed).
            incidence;  //Proportion of occupied cells at the beginning
//            optimizationerror; //allowed relative error in colonization probability

    char    *outputfilename, //Name of output log file
            *coordinatename,
            *outputmapname,  //Stem of output map name
            *sp1name,   //Stem for species1 map file name
            *sp2name;   //Stem for species2 map file name

	  PARAMS();      //Constructor.
	  ~PARAMS();     //Destructor.
	  void read(ifstream*);  //Read in all parameters from a file.
	  void write(ofstream*); //Write all parameters to a file.
    void nextline(ifstream*); //Read to the start of the next line.
  };

#endif


