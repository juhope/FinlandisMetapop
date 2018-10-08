
/****************************************************************************
* LANDIS model design:                                                      *
*        David J. Mladenoff, Joel E. Boeder, Hong S. He, George E. Host     *
* LANDIS 2.x dynamic design :                                               *
*        David J. Mladenoff, Hong S. He                                     *
* Contact :                                                                 *
*        David J. Mladenoff, Forestry Department, University of Wisconsin at*
*        Madison, 1630 Linden Dr., Madison, WI 53706                        *
*        djmladen@facstaff.wisc.edu     Tel:608-262-1992 Fax:608-262-9922   *
*                                                                           *
* History:                                                                  *
*        Name           Date                  General Modification          *
* ----------------  ------------  ----------------------------------------- *
* Jeol E. Boeder       09/93            original coding                     *
* Chris Heim           09/94            upgrating from LANDIS 1.0 to 1.5    *
* Hong S. He         1/95-8/98          upgraing to LANDIS 2.x              *
* ----------------  ------------  ----------------------------------------- *
* Lastest change was made by Hong S. He and Kevin Nimerfro Dec.96-Jan. 1998 *
*                                                                           *
* Version: 2.5a                                                             *
* Description:                                                              *
*        LANDIS is a raster based landscape model simulating forest         *
*        succession, wind and fire disturbances, forest management, and     *
*        the interaction among them over long period of time.               *
* Documentations:                                                           *
*        He, H. S., D. J. Mladenoff, and J. B. Boeder, 1995. LANDIS 2.0     *
*            Users' Guide, University of Wisconsin-Madison Forestry         *
*            Department Report.                                             *
*        He, H. S., D. J. Mladenoff, and J. B. Boeder, 1996. LANDIS 2.0     *
*            Technical Report. University of Wisconsin-Madison Forestry     *
*            Department Repost.                                             *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
*   Juho Pennanen made extensive modifications and extensions for the       *
*   "Fin-Landis" version 1998-2002                                          *
*       - changing reproduction probability during stand succession         *
*       - enabled and corrected vegetative propagation algorithm            *
*       - user-definable mode of fire probability change after last fire    *
*       - numerous other changes and corrections                            *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
*   Henna Fabritius inserted fire layers from Lankia et al. 2012            *
*                                                                           *
*   Updated to compile with Embarcadore C++Builder 10.1 Berlin.             *
*   v01: Fire spread fixed (fireB.cpp) to enable simulations similar to     *
*        the original Snäll et al. (2005) simulations.                      *
*                                                                           *
*   v02: Simulated forest fire are replaced by reading in fire layers       *
*        based on fire history data (Lankia et al. 2012).                   *
*                                                                           *
*                                                                           *
****************************************************************************/


#pragma hdrstop           //Only for C++Builder
#include <condefs.h>      //Only for C++Builder

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "landis.h"
#include "defines.h"
#include "system1.h"
#include "error.h"
#include "params.h"
#include "optim.h"
#include "landu.h"
#include "landus.h"
#include "specatt.h"
#include "specatts.h"
#include "specie.h"
#include "site.h"
#include "sites.h"
#include "FireB.h"
#include "wind.h"
#include "timber.h"
#include "io.h"
#include "map8.h"
#include "reclass3.h"
#include <iostream>

/////////////////////////////////////////////////////////////////////////////
//                      FUNCTION DEFINITIONS                               //
/////////////////////////////////////////////////////////////////////////////

//USEUNIT functions for C++Builder.
//Must be removed for compiling in other platforms.

USEUNIT("SourceF\agelist.cpp");
USEUNIT("SourceF\Error.cpp");
USEUNIT("SourceF\FireB.cpp");
USEUNIT("SourceF\Io.cpp");
USEUNIT("SourceF\Landu.cpp");
USEUNIT("SourceF\Landus.cpp");
USEUNIT("SourceF\Map8.cpp");
USEUNIT("SourceF\Optim.cpp");
USEUNIT("SourceF\Params.cpp");
USEUNIT("SourceF\Pile.cpp");
USEUNIT("SourceF\Point.cpp");
USEUNIT("SourceF\Reclass.cpp");
USEUNIT("SourceF\Reclass2.cpp");
USEUNIT("SourceF\Reclass3.cpp");
USEUNIT("SourceF\site.cpp");
USEUNIT("SourceF\Sites.cpp");
USEUNIT("SourceF\Specatt.cpp");
USEUNIT("SourceF\Specatts.cpp");
USEUNIT("SourceF\specie.cpp");
USEUNIT("SourceF\Species.cpp");
USEUNIT("SourceF\System1.cpp");
USEUNIT("SourceF\Timber.cpp");
USEUNIT("SourceF\Timerpile.cpp");
USEUNIT("SourceF\Timerpoint.cpp");
USEUNIT("SourceF\Wind.cpp");
//---------------------------------------------------------------------------
void singularLandisIteration(int, int, int, int, int, int);
// Changed next functions to bool. JP.
bool seed(SPECIESATTR*, LANDUNIT*, SPECIE *, int, int, int, int, short);
//bool noDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, short); JP
bool uniDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, short);
bool neighDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, int, int, short);
bool effDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, int, int, short);
bool seedDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, int, int, short);
bool intSeedDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, int, int, short);
bool randDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, int, int, short);
bool maxDisp(SPECIESATTR*, LANDUNIT*, SPECIE*, int, int, int, int, short);
void extraSeed(LANDUNIT*, int, int, int, int, short);
void vegProp(SPECIE*, SPECIESATTR*, int, int);
void grow(SPECIE*);
void kill(SPECIE*, SPECIESATTR*);
void thin(SPECIE* s, SPECIESATTR* sa); //JP.
//void birth(SPECIE*); Not used. JP.
void envChange(int,int);
void updateLandtype(FILE*,int);
float probBirth(LANDUNIT*, int, short);
int enoughLight(SPECIESATTR*, int);
int findShade(SITE*);
void findX1X2(int &x1, int &x2, int x, int y, int y1, int r);

/////////////////////////////////////////////////////////////////////////////
//                      GLOBAL VARIABLE DECLARATION                        //
//      THESE GLOBALS MAY BE REFERRENCED IN ALL LANDIS MODULES             //
//          THEY ARE DEFINED IN THE LANDIS HEADER FILE                     //
/////////////////////////////////////////////////////////////////////////////


PARAMETERS    parameters;       //System parameters.           SYSTEM.CPP
SPECIESATTRS  speciesAttrs(MAX_SPECIES); //Species attributes. SPECATT.CPP
LANDUNITS     landUnits(MAX_SPECIES);    //Land units.         LANDU.CPP
SITES         sites;            //Map of all landis sites.     SITES.CPP
WIND          wind;             //Windthrow structure.         WIND.CPP
FIRE          fire;             //Fire structure.              FIRE.CPP
TIMBER        timber;           //Timber management structure. TIMBER.CPP
OPTIM         optim;            //Optimization structure.      OPTIM.CPP

int freq[6];
char reMethods[30][15],ageMaps[30][15];
//HF: int *fire[];

////////////////////////////////////////////////////////////////////////////
//                             MAIN ROUTINE                               //
////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
//Main program.  This contains start and shut down procedures as well as the
//main iteration loop.

{
//			startup procedures

FILE *infile;
char str[100];
int numbOfIter, snr, snc, specAtNum;
int run_extraSeed=0;
int numSitesActive,envOn=0, reclYear=0;
//struct time t;   hsh
time_t t;
randomize();

//Setup program
if	((argc!=2)&&(argc!=3)&&(argc!=4))
{
	argHelp();
   exit(255);
}

if ((strcmp(argv[1],"-h")==0)||(strcmp(argv[1],"-H")==0))
{
	argHelp();
	exit(255);
}
else if ((strcmp(argv[1],"-p")==0)||(strcmp(argv[1],"-P")==0))
{
	help();
	exit(255);
}

if ((strcmp(argv[1],"-e")==0)||(strcmp(argv[1],"-E")==0))
{
   if (argc != 4)
      errorSys("Syntax error. For help enter: landis -h",STOP);
	envOn=(atoi(argv[2]))/10;
   if (!envOn)
   {
		errorSys("Invalid year value",STOP);
   }
   else if ((infile=fopen(argv[3],"r"))==NULL)
   {
		errorSys("Input file not found. For help enter: landis -h",STOP);
	}
}

else if ((strcmp(argv[1],"-r")==0)||(strcmp(argv[1],"-R")==0))
{
   if (argc != 4)
      errorSys("Syntax error. For help enter: landis -h",STOP);
	reclYear=atoi(argv[2]);
   if(!reclYear%10)
		errorSys("Invalid year number",STOP);
   else if ((infile=fopen(argv[3],"r"))==NULL)
   {
		errorSys("Input file not found. For help enter: landis -h",STOP);
	}
}

else if (((infile=fopen(argv[1],"r"))==NULL)
		&&((infile=fopen(argv[2],"r"))==NULL))
	errorSys("Input file not found. For help enter: landis -h",STOP);

for (int x=0; x<5; x++) freq[x]=1;
ioInit();
getInput(infile, wind, fire, timber, freq, reMethods, ageMaps);

if (reclYear)
{
	reclassify(reclYear, ageMaps);
    putOutput(reclYear/10, reclYear/10, freq);
	fclose(infile);
	ioTerm();
	return 0;
}

putOutput(0, 0, freq);
snr=sites.numRows();
snc=sites.numColumns();
numSitesActive=sites.numActive();
specAtNum=speciesAttrs.number();
optim.dim(specAtNum,snr,snc);
numbOfIter=parameters.numberOfIterations;

if	(envOn>numbOfIter)
	errorSys("Invalid year of interpretation",STOP);

//		The main loop

for (int i=1;i<=numbOfIter;i++)
{
	sprintf(str,"\nProcessing Year %d",i*10);
	printMessage(str);

    if((envOn)&&(i<=envOn))
   	    envChange(i,specAtNum);

	singularLandisIteration(i,run_extraSeed,(numSitesActive/1000),
   								snr,snc,specAtNum);
	putOutput(0, i, freq);
}

//		Close program.
fclose(infile);
ioTerm();
return 0;
}

/////////////////////////////////////////////////////////////////////////////
//                      SINGULAR LANDIS ITERATION ROUTINE                  //
/////////////////////////////////////////////////////////////////////////////

void singularLandisIteration(int itr, int run_extraSeed,
						  int numSitesActive, int snr, int snc, int specAtNum)
//This processes a singular Landis iteration.  It loops through each site
//followed by each species.  For every iteration of the loop grow and kill
//are called.  Then seed availability is checked.  If seed is available
//and shade conditions are correct birth is called.

{
char str[100];
int i, j;
LANDUNIT *l;
int shade;
SPECIE *s;
SPECIESATTR *sa;
int speciesNum;
short lastDist;

//Seed is checked before fire to enable serotiny
sprintf(str,"Seed check.");
printMessage(str);

for (i=1;i<=snr;i++)
    for (j=1;j<=snc;j++)
       {
	    //Define Land unit.
 	    l=sites(i,j)->landUnit;
		if (l!=NULL && l->active())
 	        {
           s=sites(i,j)->first();  //Specie being processed.
           while (s!=NULL)   //Inner loop; Loops through all species on a site.
		        {
		        s->seedCheck();
              s=sites(i,j)->next();
			  }
           }
        }


//Process timber management, windthrow, and fire..
if (parameters.timber)
{
	printMessage("Processing timber management.");
	timber.activate(itr, freq); //Nim: added freq
	//fseed(parameters.randSeed+itr*1);
}

if (parameters.wind)
{
	printMessage("Processing windthrow.");
   wind.activate(itr, freq); //Nim: added freq
	//RandomDump (snr, snc);
  // fseed(parameters.randSeed+itr*2);
}

if (parameters.fire)
{
	printMessage("Processing fire.");
   fire.activate(itr, freq); //Nim: added freq
 	//RandomDump (snr, snc);
   //fseed(parameters.randSeed+itr*3);
}

printMessage("Optimizing map.");
optim.initialize();    //Seed will be present if serotinous seeds or mature trees are present

//Outer loop; Loops through sites.
//Loop divided in 2 to correct a serious error. JP.

printMessage("\n");

for (i=1;i<=snr;i++)
    {
    if (i%10==0 || i==1)
        {
	    sprintf(str,"First round, row: %d",i);
	    printMessage(str);
        }
    for (j=1;j<=snc;j++)
        {
	    //Increment number of years since last fire, wind, timber.
 	    sites(i,j)->lastFire+=(short)10;
 	    sites(i,j)->lastWind+=(short)10;
 	    sites(i,j)->lastTimber+=(short)10;
 	    //Define Land unit.
 	    l=sites(i,j)->landUnit;
 	    if (l!=NULL && l->active())
 	        {
		    s=sites(i,j)->first();        //Specie being processed.
		    sa=sites(i,j)->specAtt();     //Associated attributes.
            //Inner loop; Loops through all species on a site.
		    while (s!=NULL)
		        {
		        kill(s,sa);
                thin(s,sa);
		        grow(s);
		        s=sites(i,j)->next();
		        sa=sites(i,j)->specAtt();
                }
            }
        }
    }

printMessage("\n");

for (i=1;i<=snr;i++)
    {
    if (i%10==0 || i==1)
        {
	    sprintf(str,"Second round, row: %d",i);
	    printMessage(str);
        }
    for (j=1;j<=snc;j++)
        {
 	    //Define Land unit.
 	    l=sites(i,j)->landUnit;
 	    if (l!=NULL && l->active())
 	        {
		    shade=findShade(sites(i,j));  //Shade class for site.
		    s=sites(i,j)->first();        //Specie being processed.
		    sa=sites(i,j)->specAtt();     //Associated attributes.
            lastDist=min(sites(i,j)->lastFire,sites(i,j)->lastWind); //JP
		    speciesNum=1;                 //Species referrrence #.
		    //Inner loop; Loops through all species on a site.
		    while (s!=NULL)
		        {
		        if (optim.speciesActive(speciesNum)&&(sa->shadeTolerance!=5 ||
			        sites(i,j)->oldestDense()>=sites(i,j)->landUnit->minShade))
                        if (!seed(sa, l, s, shade, speciesNum, i, j, lastDist)) //lastDist JP
		                    vegProp(s, sa, shade, lastDist);  //corrected vegetative propagation. JP.
		        //Loop to next species and species attribute.
            s->vegPropagules = false;
            s=sites(i,j)->next();
		        sa=sites(i,j)->specAtt();
		        speciesNum++;
		        }
            }
        }
    }

printMessage("\n");

if (run_extraSeed==1) extraSeed(l, numSitesActive, snr, snc, specAtNum, lastDist);
//fseed(parameters.randSeed+itr*6);
}


bool seed(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade, int sn,
			 int row, int col, short ld)
//The following routine selects the appropriate seed dispersal
//routine and switches to it.  The parameters are passed on to the
//individual routines.  They are discussed below.
//ld added to all to get changing establishment coefficients.
//The function returns true if reproduction takes place. JP.
{
switch (parameters.dispRegime) //Added return values. JP.
	  {
		//case NO_DISPERSAL:  return (noDisp(sa, l, s, shade, sn, ld); Redundant. JP.
		case UNIFORM:
            return uniDisp(sa, l, s, shade, sn, ld);
		case NEIGHBORS:
            return neighDisp(sa, l, s, shade, sn, row, col, ld);
    case EFF_DISPERSAL:
            return effDisp(sa, l, s, shade, sn, row, col, ld);
		case DISPERSAL:
            return seedDisp(sa, l, s, shade, sn, row, col, ld);
		case RAND_ASYM:
            return randDisp(sa, l, s, shade, sn, row, col, ld);
		case MAX_DIST:
            return maxDisp(sa, l, s, shade, sn, row, col, ld);
		case SIM_RAND_ASYM:
            return maxDisp(sa, l, s, shade, sn, row, col, ld);
        default:
            return false; //No dispersal. JP.
	  }
}


////////////////////////////////////////////////////////////////////////////
//                      SEED DISPERSAL REGIME SUPPORT ROUTINES            //
////////////////////////////////////////////////////////////////////////////

//All dispersal routines receive the current species attribute (sa), land
//unit type (l), Specie (s), shade value (shade), and species index number
//(sn).  The more complex routines also recieve the row, column map
//coordinates.  Time since last disturbance (ld) is used to get current
//reproduction probabilities of the species [This is added to all
//functions, JP]. Each routine determines if the recruitement of the species
//will occur this year based on seed availability, light and probability
//of birth based on species type and landunit type.  If recruitment
//occurs birth is called and true is returned. Otherwise return value is false. JP.


bool uniDisp(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade,
    int sn, short ld)
//No seed search.  Seed is always present. Dense always born.
//Returns added. JP.
{
if (frand() < (l->probRepro(sn,ld)))//JP
	 if (enoughLight(sa, shade))
	    {
        s->birth(l->probRepro(sn,ld),false);  //JP
        return true;
        }
return false;
}

bool neighDisp(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade, int sn,
			 int row, int col, short ld)
//Seed search occurs on same site and immediate neighbors (including
//diagonals).  Always dense.
//Added returns, removed veg prop. JP.
{
int i, j, minrow, mincol;
if (frand() < (l->probRepro(sn,ld)) ) //JP
	if (enoughLight(sa, shade))
		{
	  minrow=min(row+1,sites.numRows());
	  mincol=min(col+1,sites.numColumns());
	  for (i=max(row-1,1);i<=minrow;i++)
	    for (j=max(col-1,1);j<=mincol;j++)
		{
		  if ((*sites(i,j))(sn)->disPropagules>0)
          {
            s->birth(l->probRepro(sn,ld),0);  //JP
		    return true;
          }
		}
		}
return false;
}

bool effDisp(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade, int sn,
			int row, int col, short ld)
//Seed search occurs on same site and all sites in an uniform ring of the
//immediate site with radius sa->effectD. Always dense.
//Added returns, removed veg prop. JP.

{
int minrow, mincol;
int pixRange=sa->effectiveD/parameters.cellSize+1;
if (frand() < (l->probRepro(sn,ld)) )  //JP
	if (enoughLight(sa, shade))
		 {
	if (sa->effectiveD==-1)
		 {
		  s->birth(l->probRepro(sn,ld),0);  //JP
		  return true;
		 }
	minrow=min(row+pixRange,sites.numRows());
	mincol=min(col+pixRange,sites.numColumns());
	for (int i=max(row-pixRange,1);i<=minrow;i++)
		 for (int j=max(col-pixRange,1);j<=mincol;j++)
			 {
		if ( ((row-i)*(row-i)+(col-j)*(col-j)) < (pixRange*pixRange))
			  if ((*sites(i,j))(sn)->disPropagules>0)
			 {
			  s->birth(l->probRepro(sn,ld),0);  //JP
			  return true;
			 }
			 }
	}
return false;
}

bool seedDisp(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade, int sn,
			int row, int col, short ld)
//Establishment probability is checked first.
//Seed search occurs first on same site and all sites in an uniform ring of the
//radius sa->effectD. In this case dense cohort may be created.
//If no cohort is yet born,
//next search occurs within (effectD+maxD)/2, and a thin cohort may be born.
//This

//Either effectiveD or maxD may be infinite, marked with -1.

//Added returns, removed veg prop. JP.

{
int minrow, mincol, noCols, sliceRange;
int effRange,
    maxRange,
    midRange,
    sqEffRange,
    sqMidRange;
unsigned char *p; //For optimization.
if (enoughLight(sa, shade) && frand() < (l->probRepro(sn,ld)) )  //JP
  {
  if (sa->maxD==0) return false;

  if (sa->effectiveD==-1)
      {
      s->birth(l->probRepro(sn,ld),0);  //JP
      return true;
      }

  if (sa->effectiveD)
    {
    if (*optim(sn,row,col))  //For optimization. JP.
      {
      s->birth(l->probRepro(sn,ld),0);  //JP
      return true;
      }

    effRange=sa->effectiveD/parameters.cellSize;
    sqEffRange=effRange*effRange;

    minrow=min(row+effRange,sites.numRows());
    noCols=sites.numColumns();

    for (int i=max(row-effRange,1);i<=minrow;i++)
		 {
		 sliceRange=int(sqrtf(sqEffRange-(row-i)*(row-i))); //HF: sqrt to int(sqrtf)
         mincol=max(col-sliceRange,1);
         p=optim(sn,i,mincol);
		 for (int j=mincol;j<=min(col+sliceRange,noCols);j++)
            {
            if (*p++)
                  {
                  s->birth(l->probRepro(sn,ld),0);  //JP
                  return true;
                  }

            }
         }
    } //If effective range positive.

    //In the following it is checked if a thin cohort should be born.
    //Tried to optimize. JP.

    if (sa->maxD==-1)//Thin cohorts got always. No reason to search outside effD anymore.
      {
      s->birth(l->probRepro(sn,ld),1);  //JP
      return false;
      }

    maxRange=sa->maxD/parameters.cellSize;
    if (frand()<0.1)
      {
      minrow=min(row+maxRange,sites.numRows());
      for (int i=max(row-maxRange,1);i<=minrow;i++)
          {
		  sliceRange=int(sqrtf(maxRange*maxRange-(row-i)*(row-i))); //HF: sqrt to int(sqrtf)
          mincol=max(col-sliceRange,1);
          p=optim(sn,i,mincol);
          for (int j=mincol;j<=min(col+sliceRange,noCols);j++)
              {
              if (*p++)
                  {
                  s->birth(l->probRepro(sn,ld),1);  //JP
                  return true;
                  }
              }
          }
      }
    else
      {
      sqMidRange=effRange*maxRange;
	  midRange=int(sqrtf(sqMidRange)); //HF: sqrt to int(sqrtf)
	  minrow=min(row+midRange,sites.numRows());
		for (int i=max(row-midRange,1);i<=minrow;i++)
		 {
		 sliceRange=int(sqrtf(sqMidRange-(row-i)*(row-i))); //HF: sqrt to int(sqrtf)
		 mincol=max(col-sliceRange,1);
         p=optim(sn,i,mincol);
         for (int j=mincol;j<=min(col+sliceRange,noCols);j++)
            {
              if (*p++)
                  {
                  s->birth(l->probRepro(sn,ld),1);  //JP
                  return true;
                  }
            }
         }
        }

    }
return false;
}

bool randDisp(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade, int sn,
		 int row, int col, short ld)
//Seed search occurs on all sites on the map so that seeding is
//based on sa->effectD and sa->maxD.  Cells within the radius defined
//by sa->effectD have a 95% chance of seeding to the current cell.  Cells
//within the radius defined by sa->maxD have a 5% chance of seeding to the
//current cells.  All other cells %chance may be calculated by fitting
//them on a asymptopic decay graph that runs through both points.

//Added returns, removed veg prop. Always dense. JP.

{
int i,j; //Nim: added this line
int minrow, mincol;
float tempnumb;

if (frand() < (l->probRepro(sn,ld)) )
	if (enoughLight(sa, shade))
    {
	if (sa->effectiveD==-1)
		 {
		  s->birth(l->probRepro(sn,ld),0);  //JP
		  return true;
		 }
	int nr=sites.numRows();
			int nc=sites.numColumns();
	int sd=parameters.cellSize+1;
	int pixRange=sa->effectiveD/sd;
	minrow=min(row+pixRange,nr);
	mincol=min(col+pixRange,nc);
	for (i=max(row-pixRange,1);i<=minrow;i++) //Nim: changed int i to i
		{
		 for (j=max(col-pixRange,1);j<=mincol;j++) //Nim: changed int j to j
		  {
					if ((*sites(i,j))(sn)->disPropagules>0)
		{
		 tempnumb=((row-i)*(row-i)+(col-j)*(col-j))*sd;
		 tempnumb=sqrt(tempnumb);
					  if (sa->prob(tempnumb)>frand())
			{
			 s->birth(l->probRepro(sn,ld),0);  //JP
			 return true;
			}
        }
		   }
        }
// for all sites in the map check to see if seeding can take place from
// site(row, col)
	 for (i=1;i<=nr;i++) //Nim: changed int i to i
		{
		 for (j=1;j<=nc;j++) //Nim: changed int j to j
		{
		 if ((*sites(i,j))(sn)->disPropagules>0)
			{
			 tempnumb=((row-i)*(row-i)+(col-j)*(col-j))*sd;
			 tempnumb=sqrt(tempnumb);
			 if (sa->prob(tempnumb)>frand())
				{
				 s->birth(l->probRepro(sn,ld),0);  //JP
				 return true;
                }
			}
		}
        }


    }
return false;
}


bool maxDisp(SPECIESATTR *sa, LANDUNIT *l, SPECIE *s, int shade, int sn,
			int row, int col, short ld)
//Seed search occurs on same site and all sites in an uniform ring of the
//immediate site within radius sa->maxD.  It will apply the random asymptopic
//dispersal seeding effect to all sites within this ring.

//Added returns, removed veg prop. Always dense. JP.

{
unsigned char *p;     //Optimization pointer.
int minrow;
float tempnumb;

if (frand() < (l->probRepro(sn,ld)) )
	if (enoughLight(sa, shade))
    {
	if (sa->effectiveD==-1)
		 {
		  s->birth(l->probRepro(sn,ld),0);  //JP
		  return true;
		 }

  if (*optim(sn, row, col))
		 {
		  s->birth(l->probRepro(sn,ld),0);  //JP
		  return true;
		 }

	int sd=parameters.cellSize+1;
	int pixRange=sa->maxD/sd;
	minrow=min(row+pixRange,sites.numRows());
	for (int i=max(row-pixRange,1);i<=minrow;i++)
		{
		 int x1, x2;
		 findX1X2(x1, x2, col, row, i, pixRange);
		 p=optim(sn,i,x1);
		 for (int j=x1;j<=x2;j++)
         {
		 if (*(p++))
			{
			 tempnumb=( (row-i)*(row-i)+(col-j)*(col-j) )*sd;
			 tempnumb=sqrt(tempnumb);
			 if (sa->prob(tempnumb)>frand())
				{
				 s->birth(l->probRepro(sn,ld),0);  //JP
                 return true;
				}
            }
          }
		}
    }
return false;
}

void extraSeed(LANDUNIT* l, int numSitesActive, int snr, int snc, int specAtNum, short ld)
//Under the MAX_DIST seeding regime every site has a 1/1000th chance of
//receiving random seeds.  This implements this chance.

{
int row, col, sn, shade;
SPECIE *s;
SPECIESATTR *sa;
for (int i=1;i<=numSitesActive;i++)
	 {
	  row=irand(1,snr); //Nim: was (rand()%snr)+1
	  col=irand(1,snc); //Nim: was (rand()%snc)+1
	  sn=irand(1,specAtNum); //Nim: was (rand()%specAtNum)+1
	  shade=findShade(sites(row,col));      //Shade class for site.
	  s=(*(sites(row,col)))(sn);            //Specie being processed.
	  sa=sites(row,col)->specAtt();         //Associated attributes.
	  if (optim.speciesActive(sn) && enoughLight(sa, shade))
	  s->birth(l->probRepro(specAtNum,ld),0);  //JP
	 }
}

////////////////////////////////////////////////////////////////////////////
//                      FIRST ORDER ITERATION SUPPORT ROUTINES            //
////////////////////////////////////////////////////////////////////////////

void grow(SPECIE *s)
//This will grow all trees and update propagules as necessary.
//Currently disPropagules are either set at 0 or 10 dependent on mature
//tree presence.  Vegetative propagules raise in number for every 10 years
//of mature tree presence.  They fall in number for every 10 years of
//mature tree absence.

{
  s->grow();
}

void vegProp(SPECIE *s, SPECIESATTR *sa, int shade, int ld)

//Rewritten. JP. vegPropagules has been set true when suitable cohorts have
//been killed or thinned (due to disturbance or age).
//vegProb used immediately after a disturbance and vegProb2 otherwise.
//Birth checks  if thin or dense cohorts should be created.

{
float probRe;
if (s->vegPropagules && enoughLight(sa,shade))
  {
  if (ld<=10) probRe=sa->vegProb;
  else probRe=sa->vegProb2;
  if (frand()<probRe)
    s->birth(probRe,0);  //JP
  }
}

void kill(SPECIE *s, SPECIESATTR *sa)
//This will kill any trees that have reached maximum age.

{
if (s->query())
	{
   int longev=sa->longevity;
 	 s->kill(longev);
   int start=(sa->senescence+5)/10*10;
   int diff=longev-start;
	 for (int i=10;i<=diff-10;i+=10)
       if (s->query(start+i) && frand()<(float(i)/diff)*(float(i)/diff))
         s->kill(start+i);
	}
}

void thin(SPECIE* s, SPECIESATTR* sa)
//This will thin old cohorts. JP

{
float e0=0.000001;
int longev=sa->longevity;
if (s->query())
	{
 	 int numYears=(int)(longev/2.5)/10;
	 float chanceThin=0.1;
	 float chanceMod=0.9/((float)numYears+e0);
	 for (int i=longev-numYears*10;i<=longev;i+=10)
		{
		if (s->query(i) && frand()<chanceThin)
		s->thin(i);
		chanceThin+=chanceMod;
		}
	}
}


float probBirth(LANDUNIT *l, int speciesNum, short ld) //JP
//This will return the probability of birth for a particular species on a
//particular land unit and time since disturbance in ideal conditions.

{
  return l->probRepro(speciesNum, ld);
}

int enoughLight(SPECIESATTR *sa, int shade)
//This wil return true if their is enough light at the forest floor for
//species birth and false otherwise.  Sa is a species attribute and shade is
//a shade class.

{
 int tempshade=sa->shadeTolerance;
 if (tempshade==5)  //Shade class 5 species may grow under a canopy
	return true;
 else if (tempshade==-1)//pseudo-shrub species is able to grow under any canopy
 	return true;
 else
	return tempshade>shade;
}

int findShade(SITE* site)
//This will return the amount of shade for this site.

{
 int shade=-1;        //hsh
 SPECIE *specie=site->first();
 while (specie!=NULL)
	{
	 if (specie->shading())//JP
		  shade=max(site->specAtt()->shadeTolerance, shade);
	 specie=site->next();
	}
 return shade;
}


void findX1X2(int &x1, int &x2, int x, int y, int y1, int r)
//This will find the scan line (x1,x2) of a circle or radius r given that the
//center of the circle is at (x,y) and the scan line runs from (x1,y1) to
//(x2,y1).  It will also make sure that the entire scan line lies within the
//map boundaries.

{
int u, v;
int snc=sites.numColumns();

u=abs(y1-y);
v=r*r-u*u;
v=(int)(sqrt((float)v)+.999);
if ((x-v)>1) x1=(x-v);
 else x1=1;
if ((x+v)<snc) x2=(x+v);
 else x2=snc;
}

void envChange(int i,int specAtNum)
{
char str[50],iterString[5];
FILE *landTypeFile;
strcpy(str,parameters.landUnitFile);
itoa(i*10,iterString,10);
strcat(str,iterString);
if ((landTypeFile=fopen(str,"r"))==NULL)
	errorSys("Landtype input file not found.",STOP);
	updateLandtype(landTypeFile,specAtNum);
}

void updateLandtype(FILE* infile,int specAtNum)
//Read a land unit from a file.

//Non-functioning unit. Thus error message. Should be replaced by landu member
//functions ... JP.

{
errorSys("Non-functioning updateLandtype called.",STOP);

int i; //Nim: added this line
LANDUNIT *l=landUnits.first();
char instring[25];
int tempInt,tempClass[5];
int numberOfLandtype=0;

float* probReproDist=new float[specAtNum];  //JP
float* probReproFinal=new float[specAtNum]; //JP


while(l)
{
    if (fscanc(infile,"%s",instring)!=1)
   	errorSys("Error reading in name from landtype file.",STOP);
	if (fscanc(infile,"%d",&tempInt)!=1)
   	errorSys("Error reading in minShade from landtype file.",STOP);
	if (fscanc(infile,"%d",&tempInt)!=1)
   	errorSys("Error reading in fireInterval from landtype file.",STOP);
	for (i=0;i<specAtNum;i++) //Nim: changed int i to i
    {
   	    if (fscanc(infile,"%f",&probReproDist[i])!=1)
          errorSys("Error reading in probReproDist from landtype file.",STOP);
        if (fscanc(infile,"%f",&probReproFinal[i])!=1)
          errorSys("Error reading in probReproFinal from landtype file.",STOP);
    }
	for (i=0;i<5;i++)  //Nim: changed int i to i
	    if (fscanc(infile,"%d",&tempClass[i])!=1)
	       errorSys("Error reading in fireCurve from landtype file.",STOP);
	for (i=0;i<5;i++)  //Nim: changed int i to i
   	 if (fscanc(infile,"%d",&tempClass[i])!=1)
      	 errorSys("Error reading in fireCurve from landtype file.",STOP);
	for (i=0;i<5;i++)  //Nim: changed int i to i
	    if (fscanc(infile,"%d",&tempClass[i])!=1)
   	    errorSys("Error reading in windCurve from landtype file.",STOP);
	for (i=0;i<5;i++)  //Nim: changed int i to i
    	if (fscanc(infile,"%d",&tempClass[i])!=1)
      	 errorSys("Error reading in windCurve from landtype file.",STOP);
   numberOfLandtype++;
	l=landUnits.next();
}
delete[] probReproDist;     //JP
delete[] probReproFinal;    //JP
if (numberOfLandtype!=landUnits.number())
	errorSys("The updating landtype files do not match.",STOP);
}
