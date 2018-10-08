
// This is a version specifically modified for the Teeri-Lososuo studies by Pennanen et al. 2003 and further
// by Fabritius et al. 2016-18.

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream> //HF: For debugging purposes...
#include <math.h>
#include "defines.h"
#include "system1.h"
#include "error.h"
#include "timerpoint.h"
#include "timerpile.h" //Required for intensity-dependent rate-of-spread.
#include "landu.h"
#include "specatt.h"
#include "specie.h"
#include "map8.h"
#include "landis.h"
#include "FireB.h"
#include <string>
#include <sstream>

//////////////////////////////////////////////////////////////////////////////
//			    ASSOCIATED FIRE FUNCTIONS                  //
//////////////////////////////////////////////////////////////////////////////
static TIMERPILE pile;

int FIRE::susceptibility(float rAge)  //Corrected version of the original.
									  //Determines susceptibility of cohorts.
{
int sc=5;
for (int i=4;i>=0;i--)
    if (rAge>lifespan[i])
		{
        sc = susClass[i];
		break;
        }
return sc;
}

float intProbEffect(int fireCl)       //Effect of intensity on flammability
//This version for uniform flammability.
{
/*float e;
float c=0.25;//Should be new parameter!
e=exp(c*(fireCl-3));
return e;*/
return(1);
}

float standProbEffect(SITE* s)        //Effect of stand structure at 's'
									  //on flammability.

//This version for uniform flammability.
{
/*float sp=s->oldestNamed("piceabie");
float ps=s->oldestNamed("pinusylv");
float e;

if (!ps) return(0.2); //Without pine always low flammability.
e=1;
e = e-1.5*sp; //Spruce age decreases flammability.
e = 2*max(e,0.1); //0.2 <= e <=2.
*/
return(1);
}


float siteIntensityEffect(SITE* s)

/*Simplified, corrected version of the LANDIS design,
enables also the original 'default' mode.
'Site intensity effect' is given for
each land type and time-since-fire in the
landtype file.*/

{
LANDUNIT* l=s->landUnit;
int siteEff=0;
for (int i=4;i>=0;i--)
    {
	if (l->fireCurve[i]<=s->lastFire)
		{
        siteEff=l->fireClass[i];
        break;
		}
    }
if (s->lastFire > s->lastWind)
    for (int i=4;i>=0;i--)
    {
    if (l->windCurve[i]<=s->lastWind)
        {
        siteEff=l->windClass[i];
        break;
        }
    }
return siteEff;
}


float weatherIntensityEffect(double w)     //Effect of weather on fire intensity.
//This could be calculated only once per fire!
{
return 0;
}

float standIntensityEffect(SITE* s)     //Effect of stand structure on 's' on
										//fire intensity.
{
return 0;
}

unsigned char FIRE::fireClass(SITE* s, unsigned char fClass, double w)
//Determine fire class.
//Now a global function, called from
//disturb. Uses other functions defined above.

//Totally different from the original. However able to recover the original
//function too with default parameters.

{
LANDUNIT *l=s->landUnit;
if (!l->active()) return 0;
unsigned char fireClass=0;
float fr;
if (l->fireInterval) fr = (1/(float)l->fireInterval);
	else fr = 0;
int lf=1;
if (parameters.probFlag) lf=s->lastFire;  //If flag is on, using the original
										  //LANDIS age-dependent flammability.
float prob=parameters.PROB2;

if (w<fr*intProbEffect(fClass)*prob*standProbEffect(s)*lf)
	fireClass=(unsigned char)(siteIntensityEffect(s)+weatherIntensityEffect(w)+standIntensityEffect(s)+0.001);

fireClass = min((unsigned char)5,fireClass);
return fireClass;
}

int FIRE::damage(TIMERPOINT p, int fireClass)    //Removed &, JP
//This will cause damage from the fire at POINT p.  It returns the
//number of cohorts killed.

{
int i, j;
int numCohorts=0;  //HF: Moved here before land unit activity check

int specAtNum=speciesAttrs.number();
if (!sites(p.y,p.x)->landUnit->active())
		return numCohorts;

for (i=1;i<=specAtNum;i++)
	{
	SPECIE *s=(*sites(p.y,p.x))(i);
	SPECIESATTR *sa=sites(p.y,p.x)->specAtt();

	for(j=10;j<(sa->longevity);j+=10)
		{
		if (s->query(j))
		  {
		  if (fireClass>(sa->fireTolerance-susceptibility((float)j/sa->longevity)))
			{
			s->kill(j);
			numCohorts++;
			}
		  else if (fireClass>(sa->fireTolerance-susceptibility((float)j/sa->longevity)-2))
			{
			s->thin(j);
			}
		  if (fireClass>sa->serotiny)
			s->killSeeds();
		  }
		}
	}

return numCohorts;
}

int FIRE::disturb(int row, int col, int totSteps, int itr, int k, MAP8 &m, MAP8 &firemap, FILE *logfile,double weather,unsigned char fireCl)//JP
/*
This will activate a fire disturbance at row r and col c. totSteps gives the
maximum duration of the fire event. Weather is determined beforehand for each
fire. fireClass here corresponds to the 'strength' of ignition.

Ignited sites are pushed to timerpile, and 'pull'-function pulls sites randomly
at a rate inversely proportional to the given burn time.

There is no difference between damaged area and burnt area anymore.
Cohorts do not have to be killed to make a site burnt.
*/

{
try
{
TIMERPOINT p1, p2;
MAP8 dist;          //Sites are marked by the intensity class when disturbed.
SITE* s;
int     numSites=0,  //Number of sites burnt.
					 //No difference between burnt & damaged anymore! JP.
		numCohorts=0,//Number of cohorts damaged.
		numLU;		//number of landunits
double   burnTime=0; //Float for small increments with large fire fronts
long    land[30];	//fire on different landunits, maximum 30
char*   NAME; //For the landunit name
int     i,j;
char str[80];          //Character string. HF added for printing fire intensities.

numLU = landUnits.number();
for (int t = 1; t <=numLU; t++) land[t]=0;

pile.reset();
dist.dim(sites.numRows(), sites.numColumns());
dist.fill(0);
if (fireCl<=0) fireCl=0;
// errorSys("Disturb called with non-positive fire class.",STOP);
//HF: Commented out, as fire layers determine burn, not fire class.
//HF: Fire should perhaps just skip inactive cells.

dist(row,col)=fireCl;

// HF: Add here the reading of the fire locations & filling of the pile
for (i=1;i<=sites.numRows();i++) for (j=1;j<=sites.numColumns();j++) if(firemap(i,j)>0)
{
	p1.y=i; //row
	p1.x=j; //col
	p1.timer=max(1,pow(2,(double)(5-fireCl))); //HF: Added double
	fireCl=fireClass(sites(p1.y,p1.x),fireCl,weather);
	dist(p1.y,p1.x)=fireCl;
	pile.push(p1);
}

fprintf(logfile,"%d\t%d\t%d\t",col,row,totSteps);
fflush(logfile);

while (!pile.isEmpty() && burnTime<totSteps) //Note time check. JP.
	{
	p2=pile.pull();
	numSites++;
	burnTime++;  //HF: one site burns at a time
	s=sites(p2.y,p2.x);
	fireCl=dist(p2.y,p2.x);
	numCohorts+=damage(p2, fireCl);
	NAME=s->landUnit->name;
	s->lastFire=0;
	for (int l = 1;l<=numLU;++l)
	  {
	  if(strcmp(landUnits(l)->name,NAME)==0)
		{
		land[l]++;
		break;
		}
	  }

	m(p2.y,p2.x)=(unsigned char)(1+fireCl); //Update fire map for the iteration.

	//HF: Fire spread part removed for the version where fire determined by input maps.

	}           //While fire burns.

//Update log file with fire stats.
fprintf(logfile, "%d\t%d",numSites, numCohorts);
for(int i = 1; i<=numLU;i++)
	fprintf(logfile, "%s %d", "\t",land[i]);
fprintf(logfile, "%s", "\n");
fflush(logfile);

//Write a fire class map output file after a single fire.
std::stringstream distmap;
distmap << parameters.outputDir << "\\fire" << itr*10 << "_" << k; //HF: std::string distmapname = distmap.str();
std::string distmapname = distmap.str();
dist.setCellSize(parameters.cellSize);
dist.setHeader(sites.getHeader());
dist.assignLeg(0,"Act_Sites");
dist.assignLeg(1,"Class 1");
dist.assignLeg(2,"Class 2");
dist.assignLeg(3,"Class 3");
dist.assignLeg(4,"Class 4");
dist.assignLeg(5,"Class 5");
dist.write(distmapname.c_str());


return numSites;
}
catch(...)
  {
  errorSys("Error inside disturb",CONTINUE);
  exit(10);
  return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
//			    EXTERNAL FUNCTIONS				                                   //
///////////////////////////////////////////////////////////////////////////////


void FIRE::activate(int itr, int freq[])  //Nim: added int freq[]
//This will simulate fire for a single Landis iteration.

{
FILE *logfile,*logfile2;  //fire log files.
long cellSize;         //Now cell side length. JP.
char str[80];          //Character string.
int i,j,si,sj,k,z,ign,burn,toBeBurnt,prevBurns,prevIgns,snr,snc,ignNumber;    //Additions. JP.
float siz, runningsiz, f, averageBurn;   //Additions. JP.
unsigned char F; //JP.
double weather, dr; //JP
SITE* s; //JP

if (itr==1) { // this sets counters/loggers for the decade
  burnable=ignTotal=burnTotal=0;
  for (i=0;i<10;i++) {
	burns[i]=0;
	igns[i]=0;
	}
  }

//Fill map.
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc); //Resets the fire map for single iteration.
for (i=1;i<=snr;i++)
	for (j=1;j<=snc;j++)
	{
	//if (sites(i,j)->landUnit->fireInterval) //JP7/02 //HF v.02 commented off
	  //{ HF v02: Now the whole landscape is set burnable.
	  if (itr==1) burnable++;
			m(i,j)=1;
	  //} HF
	//else m(i,j)=0;
	}

if (itr==1)
	cummMap=m;

if (parameters.checkFire)
  errorSys("Error: Fire frequency mode not implemented, sorry.",STOP);

//Setup fire parameters.
cellSize=parameters.cellSize;
sprintf(str,"%s/%s",parameters.outputDir,logFn); //prepare log file
if ((logfile=fopen(str,"a"))==NULL)
	 errorSys("Error opening fire log file",STOP);
sprintf(str,"%s/%s",parameters.outputDir,logFn2);
if ((logfile2=fopen(str,"a"))==NULL)
	 errorSys("Error opening fire freq log file",STOP);

double MFS=meanFireSize/cellSize;
double size;

//fseed(parameters.randSeed+itr*6);

float minSize=(float)minFireSize/(float)cellSize;
float maxSize=(float)maxFireSize/(float)cellSize;

//Define ignNumber
i=0;
do i++;
while (ignnode[i]<10*itr && i<parameters.numberOfIterations); //HF: changed to numberOfIterations

if(ignnode[i]==10*itr){ //HF: Fire takes place if the disturbance file (dist.dat) has fires for the decade

	j=itr*10-ignnode[i-1]; //how many decades ago the last ignnode took place
	k=ignnode[i]-itr*10;   //how many decades ahead is the next ignnode; now irrelevant
	f=(k*ignitions[i-1]+j*ignitions[i])/(j+k); //ignitions are scaled accordingly

	prevBurns=0; //adds the previous iteration's burns & igns to a history log
	prevIgns=0;  //HF: Changed from i to z as i is still needed
	for (z=0;z<10;z++) {
	  prevBurns+=burns[z];
	  prevIgns+=igns[z];
	  }

	//during the transient better not to calculate averageBurns
	if (prevBurns==0 || prevIgns==0) averageBurn=1;
	else averageBurn=(float)prevBurns/prevIgns;

	if (parameters.checkFire) { //f gives AAB
	  toBeBurnt=-f*burnable/10;
	  ignNumber=pseudoPoisson(toBeBurnt/averageBurn);
	  }
	else {             //f gives mean ignitions per iteration
	  //ignNumber=pseudoPoisson(f); //HF: ignNumber read from the file
	  ignNumber=ignitions[i];
	  }

	for (i=0;i<9;i++) { //move fire & ign history log one step backwards
	  burns[i]=burns[i+1];
	  igns[i]=igns[i+1];
	  }
	igns[9]=0;

	k=1;
	ign=burn=0;

	while (k<=ignNumber) {

		//HF: Open a fire layer based on iteration & ignition number
		std::stringstream firelayer_ss;
		if(k==1) firelayer_ss << "..\\FireLayers\\" << itr*10 << ".gis";
		else firelayer_ss << "..\\FireLayers\\" << itr*10 << "_" << k << ".gis";
		std::string firelayername = firelayer_ss.str();

		//HF: Create a fire map from the fire layer
		MAP8 firemap(sites.getHeader());
		firemap.dim(sites.numRows(), sites.numColumns());
		firemap.read(firelayername.c_str());

		weather=drand();  //JP //HF: Moved up, so the weather variable stays the same for one fire.

		size=0;
		si=0;
		sj=0; //HF: Pointers to the first positive map cell (=start of ignition)
		for (i=1;i<=snr;i++) for (j=1;j<=snc;j++) if(firemap(i,j)>0)
		{
			size++;
			if (si==0 && sj==0) {
				si=i;
				sj=j;
			}
		}
		std::cout << "Fire ignition size & point: " << size << " " << si << " " << sj << "\n";
		i=si;
		j=sj;
		s=sites(i,j);
		F=fireClass(s,2,weather);

		ign++;
		//if (F) { //HF: Removed the if clause: this BTW prevents fires of class 0!
		fprintf(logfile,"%d\t",itr*10);
		fflush(logfile);
		burn+=disturb(i,j,size,itr,k,m,firemap,logfile,weather,F); //Note: 'size' is really fire duration now. //HF: Size is real size from the fire layers now, added "itr" for printing fire class maps.
		igns[9]++;
		k++; //} // HF: Removed the if clause

	  } //while loop for ignition trials and fires.

	//Add data to cummMap.
	for (i=1;i<=snr;i++)
	  for (j=1;j<=snc;j++)
		{
		if (m(i,j)>=2)
		  cummMap(i,j)=m(i,j);
		}

	ignTotal+=ign;
	burnTotal+=burn;

	burns[9]=burn;
	//igns[9]=ign;

	prevBurns=0;
	prevIgns=0;
	for (i=0;i<10;i++) {
	  prevBurns+=burns[i];
	  prevIgns+=igns[i];
	  }

	if (igns[9]) siz=(float)burns[9]/igns[9];
	else siz=0;
	if (prevIgns) runningsiz=(float)prevBurns/prevIgns;
	else runningsiz=0;
  }
fprintf(logfile2,"%d\t%d\t%f\t%f\t%f\t%d\t%f\t%f",itr*10,ign,f,(float)burn/burnable*10,(float)prevBurns/burnable,igns[9],siz,runningsiz);
fprintf(logfile2, "%s", "\n");
fflush(logfile2);

if ((freq[1]>0)&&((itr%freq[1])==0)&&(freq[1]<10)||(itr*10==freq[1])) //Nim: added this line
	{                                                                     //Nim: added this line
	//Write map output file.
	sprintf(str,"fire damage assessment for year %d.",itr*10);          //Nim: changed spelling
	m.setHeader(sites.getHeader());
	m.rename(str);
	m.assignLeg(0,"Act_Sites");
	m.assignLeg(1,"Class 1");
	m.assignLeg(2,"Class 2");
	m.assignLeg(3,"Class 3");
	m.assignLeg(4,"Class 4");
	m.assignLeg(5,"Class 5");
	sprintf(str,"%s/%s%d",parameters.outputDir,yearlyFn,itr*10);
	m.setCellSize(parameters.cellSize);
	m.write(str,red,green,blue);
	} //Nim: added this line

fclose(logfile);
fclose(logfile2);

if (itr==parameters.numberOfIterations)
	{
	 //Write cumulative map output file.
	 sprintf(str,"Cumulative fire damage assessment."); //Nim: changed spelling
	 cummMap.setHeader(sites.getHeader());
	 cummMap.rename(str);
	 cummMap.assignLeg(0,"Act_Sites");
	 cummMap.assignLeg(1,"Class 1");
	 cummMap.assignLeg(2,"Class 2");
	 cummMap.assignLeg(3,"Class 3");
	 cummMap.assignLeg(4,"Class 4");
	 cummMap.assignLeg(5,"Class 5");
	 sprintf(str,"%s/%s",parameters.outputDir,finalFn);
	 cummMap.setCellSize(parameters.cellSize);
	 cummMap.write(str,red,green,blue);
	}
}

void FIRE::read(FILE *infile)
//This will read fire parameters into the model.

{
int i,k; //JP
if (fscanc(infile,"%ld",&minFireSize)!=1)
	 errorSys("Error reading in minFireSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&maxFireSize)!=1)
	 errorSys("Error reading in maxFireSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&meanFireSize)!=1)
	 errorSys("Error reading in meanFireSize from disturbance file.",STOP);
if (fscanc(infile,"%d",&meanLower)!=1)
	 errorSys("Error reading in meanLower from disturbance file.",STOP);
if (fscanc(infile,"%d",&meanUpper)!=1)
	 errorSys("Error reading in meanUpper from disturbance file.",STOP);
if (fscanc(infile,"%d",&strong)!=1)
	 errorSys("Error reading in minFireSize from disturbance file.",STOP);
if (fscanc(infile,"%d",&light)!=1)
	 errorSys("Error reading in minFireSize from disturbance file.",STOP);
if (fscanc(infile,"%s",&yearlyFn)!=1)
	 errorSys("Error reading in yearlyFn from disturbance file.",STOP);
if (fscanc(infile,"%s",&finalFn)!=1)
	 errorSys("Error reading in finalFn from disturbance file.",STOP);
if (fscanc(infile,"%s",&logFn)!=1)
	 errorSys("Error reading in logFn from disturbance file.",STOP);
if (fscanc(infile,"%s",&logFn2)!=1)
	 errorSys("Error reading in logFn from disturbance file.",STOP);
for (i=0;i<5;i++) //User-defined lifespan proportions. JP.
		if (fscanc(infile,"%f",&lifespan[i])!=1)
	 errorSys("Error reading in lifespan from disturbance file.",STOP);
for (i=0;i<5;i++) //User-defined corresponding susceptibility classes. JP.
    if (fscanc(infile,"%d",&susClass[i])!=1)
	 errorSys("Error reading in lifespan from disturbance file.",STOP);
k=0;
do {
  if (fscanc(infile,"%d",&ignnode[k])!=1)
  errorSys("Error reading in lifespan from disturbance file.",STOP);
  k++;
  }
while (ignnode[k-1]<10*parameters.numberOfIterations && k<30);

for (i=0;i<k;i++) {
  if (fscanc(infile,"%f",&ignitions[i])!=1)
    errorSys("Error reading in lifespan from disturbance file.",STOP);
  }
}

void FIRE::initColors(int *r, int *g, int *b)
//This will initialize the color scheme.

{
red=r;
green=g;
blue=b;
}

void FIRE::initLogFile()
//This will initialize the fire log file.

{
FILE *logfile,*logfile2;
char str[100];
int numLU; numLU=landUnits.number();
sprintf(str,"%s/%s",parameters.outputDir,logFn);
remove(str);
if ((logfile=fopen(str,"a"))==NULL)
	 errorSys("Error opening fire log file",STOP);
fprintf(logfile,"Year\tx\ty\tTotalAreaSelected\tDamagedArea\tDamagedCohorts");
for (int i =1; i<=numLU;i++)
	fprintf(logfile, "\t%s", landUnits(i)->name);
fprintf(logfile, "%s", "\n");
fclose(logfile);

sprintf(str,"%s/%s",parameters.outputDir,logFn2);
remove(str);
if ((logfile2=fopen(str,"a"))==NULL)
	 errorSys("Error opening fire freq log file",STOP);
fprintf(logfile2,"Year\tIgnNo\tExpected\tAAB\trunning_AAB\tFireNo\tFireSize\trunning_size");
fprintf(logfile2, "%s", "\n");
fclose(logfile2);
}


