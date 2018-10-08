
/******************************************************************
Epiphyte metapopulation simulator.
Operates on Fin-Landis species age output maps.

(c) 2002 Juho Pennanen
(c) 2016 Henna Fabritius
******************************************************************/

#include <condefs.h>
#include <stdlib.h>
#include <math.h>
#include <sstream> //HF: Updated from strstream.h
#include <list> //HF: Updated from list.h
#include <random> //HF: Added
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <float.h>

#pragma hdrstop

#include "erdas.h"
#include "params.h"

struct Cell {
  Cell(int x,int y) : xm(x), ym(y) {};
  int xm;
  int ym;
  };

typedef list<Cell> Cells;
typedef list<Cell>::iterator CellIterator;

USEUNIT("Erdas.cpp");
USEUNIT("Params.cpp");
USE("Metapop1.todo", ToDo);
//---------------------------------------------------------------------------
float connect(const float, const PARAMS&);

float celldistance(const int, const int, const int, const int, const PARAMS&);

//---------------------------------------------------------------------------
int main(int argc, char **argv)
{

ErdasFile *outputmap;

//Create list for ha-cells for ha-level output log
Cells cells;

stringstream s; //HF: Modified from ostrstream
const double x_max = -log(DBL_EPSILON); //HF: Added

int n, mini, maxi, pm, qm, pM, qM,
	occurrence=0,
	abundance,
	abundance1,
	abundance2,
	landisyear=-10;

char c;

PARAMS params;

ifstream        paramfile,
				coordinateinfile,
				hostmap;

ofstream  outputfile,
		  halogfile,
		  coordinatefile;

randomize();

float connectivity;

//Get parameter file name.
if (argc!=2) {
  cout << "Error: None or too many arguments." << endl;
  cout << "Exactly one argument (parameter file name) required." << endl;
  exit(255);
  }

//Open parameter file
paramfile.open(argv[1]);
if (!paramfile.good()) {
  cout << "Error: Could not open parameter file '" << argv[1] << "'." << endl;
  exit(255);
  }

//Read in parameters

params.read(&paramfile);
paramfile.close();

const int         rows=params.rows,  //size of simulated map
				  cols=params.cols,
				  roffset=params.roffset, //location of simulated map in landis map
				  coffset=params.coffset,
				  mrows=params.mrows, //Size of finlandis input maps
				  mcols=params.mcols,
				  noPix=rows*cols,
				  NWx=params.NWx,     //x-coordinate for the center of the NW-most cell of input map
				  NWy=params.NWy;     //y-coordinate for the center of the NW-most cell of input map

int fs, ls;
int minage[2];
minage[0]=params.sp1MinAge;
minage[1]=params.sp2MinAge;

if (params.useCoordinates) {
  //Create ha-level output log and print headers
  halogfile.open("hapopu.log");
  if (!halogfile.good()) {
	cout << "Error: Could not create ha-cell output log 'hapopu.log'." << endl;
	exit(255);
	}
  halogfile << "#No of occupied ha-cells for host species and epiphyte." << endl;
  halogfile << "#Only host trees old enough for substrate are counted." << endl;
  halogfile << "year\thost1\thost2\tsome.host\tepiphyte" << endl;
  //Open file for a list of cells to consider in ha-level output
  coordinateinfile.open("hacelldata.txt");
  if (!coordinateinfile.good()) {
	cout << "Error: Could not open coordinate list file 'hacelldata.txt'." << endl;
	exit(255);
	}
  //Skip header
  char buffer[200];
  coordinateinfile.getline(buffer,200);
  //Read in list of coordinates and close file
  while (!coordinateinfile.eof())   {
	int id,xm,ym;
	//Read in coordinates one at a time
	coordinateinfile >> id >> xm >> ym;
	coordinateinfile.ignore(200,'\n');
	cells.push_front(Cell(xm,ym));
	}
  coordinateinfile.close();
  }

if (params.exactradius>rows/2-1||params.exactradius>cols/2-1) {
  cout << "Error: radius for exact dispersal calculation set too high." << endl;
  cout << "Use 0 for exact calculations without optimization." << endl;
  exit(255);
  }

int *population=new int[noPix];   //Epiphyte population on aspen, 0=empty; 1,2,... = age
double *col_prob=new double[noPix];   //HF: Added
double *pers_prob=new double[noPix];  //HF: Added
int *sp[2];
if (params.sp1MinAge>=0) fs=0;
else fs=1;
if (params.sp2MinAge>=0) ls=1;
else ls=0;
if (fs>ls) {
  cout << "Error: Min age parameter = -1 for both species" << endl;
  exit(255);
  }

for (int i=0;i<=1;i++) sp[i] = new int[noPix]; //Aspen occurrence, 0=empty; 1,2,... = age

int acols = (cols-1)/params.aggregation+1;
int arows = (rows-1)/params.aggregation+1;
if (params.exactradius<=0) acols=arows=1;
int apix = acols*arows;

int *aggregate = new int[apix];
int *aggri = new int[apix];
int *aggrj = new int[apix];

int *pmin,*pmax,*qmin,*qmax,
	*apmin,*apmax,*aqmin,*aqmax;

if (params.exactradius>0) {
  pmin = new int[noPix];  //These define the boundaries of the
  pmax = new int[noPix];  //smallest rectangular area around each cell
  qmin = new int[noPix];  //which follow the boundaries of cell aggregates
  qmax = new int[noPix];  //and include cells within exactradius
  apmin = new int[noPix];  //These define boundaries in terms of 'aggregate coordinates'
  apmax = new int[noPix];  //of the same rectangular area
  aqmin = new int[noPix];
  aqmax = new int[noPix];
  for (int i=0;i<rows;i++)
	for (int j=0;j<cols;j++) {
	  if (i-params.exactradius>=0) pmin[i*cols+j]=(i-params.exactradius)/params.aggregation*params.aggregation;
		else if (params.boundary==1) pmin[i*cols+j] = 0;
		  else pmin[i*cols+j]=(rows+i-params.exactradius)/params.aggregation*params.aggregation-rows;
	  if (i+params.exactradius<=rows) pmax[i*cols+j]=(i+params.exactradius)/params.aggregation*params.aggregation+params.aggregation-1;
		else if (params.boundary==1) pmax[i*cols+j]= rows-1;
		  else pmax[i*cols+j]=(i+params.exactradius-rows)/params.aggregation*params.aggregation+params.aggregation-1+rows;
	  if (j-params.exactradius>=0) qmin[i*cols+j]=(j-params.exactradius)/params.aggregation*params.aggregation;
		else if (params.boundary==1) qmin[i*cols+j] = 0;
          else qmin[i*cols+j]=(cols+j-params.exactradius)/params.aggregation*params.aggregation-cols;
      if (j+params.exactradius<=cols) qmax[i*cols+j]=(j+params.exactradius)/params.aggregation*params.aggregation+params.aggregation-1;
        else if (params.boundary==1) qmax[i*cols+j]= cols-1;
          else qmax[i*cols+j]=(j+params.exactradius-cols)/params.aggregation*params.aggregation+params.aggregation-1+cols;
      if (i-params.exactradius>=0) apmin[i*cols+j]=(i-params.exactradius)/params.aggregation;
        else if (params.boundary==1) apmin[i*cols+j] = 0;
		  else apmin[i*cols+j]=(rows+i-params.exactradius)/params.aggregation-arows;
      if (i+params.exactradius<=rows) apmax[i*cols+j]=(i+params.exactradius)/params.aggregation;
        else if (params.boundary==1) apmax[i*cols+j]= arows-1;
          else apmax[i*cols+j]=(i+params.exactradius-rows)/params.aggregation+arows;
      if (j-params.exactradius>=0) aqmin[i*cols+j]=(j-params.exactradius)/params.aggregation;
        else if (params.boundary==1) aqmin[i*cols+j] = 0;
          else aqmin[i*cols+j]=(cols+j-params.exactradius)/params.aggregation-acols;
      if (j+params.exactradius<=cols) aqmax[i*cols+j]=(j+params.exactradius)/params.aggregation;
        else if (params.boundary==1) aqmax[i*cols+j]= acols-1;
          else aqmax[i*cols+j]=(j+params.exactradius-cols)/params.aggregation+acols;
      }
  }

//calculate i and j coordinates for each aggregate's centerpoint
for (int i=0;i<arows;i++)  //Loop through aggregates
  for (int j=0;j<acols;j++) {
    mini=params.aggregation*i;
    maxi=min(params.aggregation*(i+1),rows)-1;
    aggri[i*acols+j]=(mini+maxi)/2;
    mini=params.aggregation*j;
    maxi=min(params.aggregation*(j+1),cols)-1;
    aggrj[i*acols+j]=(mini+maxi)/2;
    }


//Open output log
outputfile.open(params.outputfilename);
if (!paramfile.good()) {
  cout << "Error: Could not create or open output log file '" << params.outputfilename << "'." << endl;
  exit(255);
  }

//print parameters and headers to output log
params.write(&outputfile);
outputfile << "#Note: Only cells with host trees old enough for substrate are counted." << endl;
outputfile << "Year\tHost1\tHost2\tHost\tOccupied" << endl;

cout << endl;

for (int i=0;i<rows;i++)
  for (int j=0;j<cols;j++)
    population[i*cols+j]=0;

for (int k=0;k<=1;k++)
  for (int i=0;i<rows;i++)
	for (int j=0;j<cols;j++)
	  sp[k][i*cols+j]=0;


//---------------------------------------------------------------------------
//  ITERATION MAIN LOOP
//---------------------------------------------------------------------------

for (int y=params.startyear;y<=params.startyear+params.simulationlength;y+=params.timestep) {

  cout << "Year: " << y << endl;

  for (int i=0;i<arows;i++)
	for (int j=0;j<acols;j++) {
	  aggregate[i*acols+j]=0;
	  }

  for (int i=0;i<rows;i++) //HF: Added
	for (int j=0;j<cols;j++)
	  col_prob[i*cols+j]=0;

  for (int i=0;i<rows;i++) //HF: Added
	for (int j=0;j<cols;j++)
	  pers_prob[i*cols+j]=0;

  //Increase age of epiphyte occurrences -----------------------------
  for (int i=0;i<rows;i++)
    for (int j=0;j<cols;j++)
      if (population[i*cols+j]>0) population[i*cols+j]+=params.timestep; //increase age of occurrences


  //Read in new habitat maps --------------------------------
  if (y>=landisyear+10) { //If new substrate map is available proceed to use it
	landisyear = y/10 * 10; //Assumes fin-landis output every 10 years
	for (int k=fs;k<=ls;k++) {
	  s.str(""); //HF: This didn't work anymore: s.seekp(0,ios_base::beg);
	  if (k==0) s << params.sp1name;
	  else s << params.sp2name;
	  s << landisyear << ".gis"; //HF: Removed "<< ends"
	  cout << s.str() << "\n";

	  const std::string tmp = s.str(); //HF: Modified to solve stringstream - const char* inconsistency
	  hostmap.open(tmp.c_str(),ios::binary); //HF: Modified from hostmap.open(s.str(),ios::binary);
	  if (!hostmap.good()) {
		  cout << "Error: Could not open map file '" << s.str() << "'." << endl;
		  exit(255);
          }
	  hostmap.seekg(128,ios_base::beg);
      //read hostmap and update landscapes here
      for (int i=0;i<mrows;i++)
        for (int j=0;j<mcols;j++) {
          hostmap.get(c);
          if (i>=roffset && i<roffset+rows && j>=coffset && j<coffset+cols) {
            if (c<50 && c>=3) sp[k][(i-roffset)*cols+j-coffset]=10*(c-2);
            else sp[k][(i-roffset)*cols+j-coffset]=0;
            }
          }
      hostmap.close();
      }
    }  //end updating maps

  //Populate map on first iteration -------------------------------------------

  if (y==params.startyear)
    for (int i=0;i<rows;i++)
	  for (int j=0;j<cols;j++)
        if ((sp[0][i*cols+j]>=minage[0] && minage[0]>-1 || sp[1][i*cols+j]>=minage[1] && minage[1]>-1) && rand()<params.incidence*RAND_MAX)
          population[i*cols+j]=params.maturity;

  //Deterministic extinction
  for (int i=0;i<rows;i++)
	for (int j=0;j<cols;j++)
	  if (sp[0][i*cols+j]<minage[0] && sp[1][i*cols+j]<minage[1]) population[i*cols+j]=0; //HF: Mature hosts but no lobaria

  //Random extinction (= persistence probability for cells with host trees & Lobaria. HF: Note that we now use params.mortality for persistence probability.)
  for (int i=0;i<rows;i++)
    for (int j=0;j<cols;j++)
	  //if (population[i*cols+j]>0 && rand()<params.mortality*RAND_MAX) //HF: Slightly modified to remove the randomization from here
	  if ((population[i*cols+j]>0) && ((sp[0][i*cols+j]>=minage[0] && minage[0]>-1) || (sp[1][i*cols+j]>=minage[1] && minage[1]>-1))){ //HF: Host trees and Lobaria
		if(params.mortality<=x_max && population[i*cols+j]>0) pers_prob[i*cols+j]=exp(params.mortality)/(exp(params.mortality)+1); //HF: Set persistence probability
		if(params.mortality> x_max && population[i*cols+j]>0) pers_prob[i*cols+j]=1.0; //HF: Overflow handling
	  }

  cout << "Progress:  0%";
  //Optimized colonization
  if (params.exactradius>0) {
    //Calculate aggregate sources
    for (int i=0;i<arows;i++)  //Loop through aggregates
      for (int j=0;j<acols;j++) {
        pm=params.aggregation*i;
        pM=min(params.aggregation*(i+1),rows)-1;
        qm=params.aggregation*j;
        qM=min(params.aggregation*(j+1),cols)-1;
        for (int p=pm;p<=pM;p++) //Loop through cells within the aggregate
          for (int q=qm;q<=qM;q++)
            if (population[p*cols+q]>params.maturity) aggregate[i*acols+j]+=1;
		}

    //Loop through potential colonizations on both hosts. HF: Estimate colonization probability for cells with mature hosts
      for (int i=0;i<rows;i++) {
        cout << "\b\b\b";
        n = (int)(100.0*i/rows);
        if (n<10) cout << ' ';
        cout << n << '%';
        for (int j=0;j<cols;j++)
		  //if ((sp[0][i*cols+j]>=max(minage[0],1) && minage[0]>-1 || sp[1][i*cols+j]>=max(minage[1],1) && minage[1]>-1) && population[i*cols+j]==0) {
		  if ((population[i*cols+j]==0) && ((sp[0][i*cols+j]>=minage[0] && minage[0]>-1) || (sp[1][i*cols+j]>=minage[1] && minage[1]>-1))){ //HF: Mature hosts but no lobaria
			//Calculate input from cells inside aggregate cells that are (partially) within exactradius
			connectivity=0;
			pm=pmin[i*cols+j];
			pM=pmax[i*cols+j];
			qm=qmin[i*cols+j];
			qM=qmax[i*cols+j];
			  for (int p=pm;p<=pM;p++)
				for (int q=qm;q<=qM;q++)
				  if (population[(p+rows)%rows*cols+(q+cols)%cols]>params.maturity)
					connectivity+=connect(20*celldistance(j,i,q,p,params),params);
			//Add input from aggregate sources
			for (int p=0;p<arows;p++)  //Loop through aggregates
			  for (int q=0;q<acols;q++)
				connectivity+=aggregate[p*acols+q]*connect(20*celldistance(j,i,aggrj[p*acols+q],aggri[p*acols+q],params),params);
			//Subtract aggregates whose cells where accounted for in detail
			for (int p=apmin[i*cols+j];p<=apmax[i*cols+j];p++)  //Loop through aggregates
			  for (int q=aqmin[i*cols+j];q<=aqmax[i*cols+j];q++)
				connectivity-=aggregate[(p+arows)%arows*acols+(q+acols)%acols]*connect(20*celldistance(j,i,aggrj[(p+arows)%arows*acols+(q+acols)%acols],aggri[(p+arows)%arows*acols+(q+acols)%acols],params),params);
			//float probability1,probability2,probability=0; //HF: Removed this version doesn't have species-specific col. rates
			//int flag=0;
			if(connectivity>0) col_prob[i*cols+j]=1-exp(-exp(params.colonization1 + log(connectivity))); //HF: Col. probability for both species
			}
		}
	}   //end of optimized algorithm

  //Brute force version of colonization -------------
  else { //exactradius <=0 i.e. no optimization
	  for (int i=0;i<rows;i++) {
		cout << "\b\b\b";
		n = (int)(100.0*i/rows);
		if (n<10) cout << ' ';
		cout << n << '%';
		for (int j=0;j<cols;j++)
		  //if ((sp[0][i*cols+j]>=max(minage[0],1) && minage[0]>-1) || (sp[1][i*cols+j]>=max(minage[1],1) && minage[1]>-1) && population[i*cols+j]==0) {
		  if ((population[i*cols+j]==0) && ((sp[0][i*cols+j]>=minage[0] && minage[0]>-1) || (sp[1][i*cols+j]>=minage[1] && minage[1]>-1))){ //HF: Mature hosts but no lobaria
		    connectivity=0;
			for (int p=0;p<rows;p++)
			  for (int q=0;q<cols;q++)
				if (population[p*cols+q]>params.maturity)
				  connectivity+=connect(20*celldistance(j,i,q,p,params),params);
			if(connectivity>0) col_prob[i*cols+j]=1-exp(-exp(params.colonization1 + log(connectivity))); //HF: Col. probability for both species
		  }
		} //row loop
   } //else

   //HF: Estimate occupancy probability based on extinction & colonization probabilities
   for (int i=0;i<rows;i++){
	 for (int j=0;j<cols;j++){
	  if ((population[i*cols+j]>0) && ((sp[0][i*cols+j]>=minage[0] && minage[0]>-1) || (sp[1][i*cols+j]>=minage[1] && minage[1]>-1))) //HF: Mature hosts and Lobaria
		if (rand()>=(pers_prob[i*cols+j]*RAND_MAX)) population[i*cols+j]=0; //HF: Stochastic extinction
	  if ((population[i*cols+j]==0) && ((sp[0][i*cols+j]>=minage[0] && minage[0]>-1) || (sp[1][i*cols+j]>=minage[1] && minage[1]>-1))) //HF: Mature hosts but no lobaria
		if (rand()<(col_prob[i*cols+j]*RAND_MAX)) population[i*cols+j]=1; //Establish if lucky enough
	 }
   }

   //------------------------------------------------------------------------

  cout << endl;

  //Create output ----------------------------------------------------------

  occurrence = abundance = abundance1 = abundance2 = 0;

  if (!params.useCoordinates) //Calculate abundances etc. for all simulated cells
    for (int i=0;i<rows;i++)          //Loop through cells
      for (int j=0;j<cols;j++) {
        if (params.sp1MinAge>=0 && sp[0][i*cols+j]>=params.sp1MinAge) abundance1++;
        if (params.sp2MinAge>=0 && sp[1][i*cols+j]>=params.sp2MinAge) abundance2++;
        if (population[i*cols+j]>0) occurrence++;
		if ((params.sp1MinAge>=0 && sp[0][i*cols+j]>=params.sp1MinAge) || (params.sp2MinAge>=0 && sp[1][i*cols+j]>=params.sp2MinAge)) abundance++;
        }



  else  //Calculate abundances etc on 20-m level for given list of cells only
    for (CellIterator i=cells.begin(), end=cells.end();i!=end;i++)  {
      int y,x,ym,xm;
      //Find corresponding cell coordinate
      y=(NWy-20*roffset-(*i).ym)/20;
      x=((*i).xm-NWx-20*coffset)/20;
      if (x<0||x>=cols||y<0||y>=rows) {
        cout << "Error: Coordinates out of range:\t" << x << "\t" << y << endl;
        exit(255);
        }
      //Loop through the 25 20-m cells that belong to the 1-ha cell
      for (int j=-2;j<3;j++) for (int k=-2;k<3;k++) {
        //  For each 20-m cell, check if substrate trees present and add to count
        xm=x+j;
        ym=y+k;
        if (xm>=0&&xm<cols&&ym>=0&&ym<rows) {
          xm=xm+cols*ym;
          if(params.sp1MinAge>=0 && sp[0][xm]>=params.sp1MinAge) abundance1++;
          if(params.sp2MinAge>=0 && sp[1][xm]>=params.sp2MinAge) abundance2++;
          if(population[xm]>0) occurrence++;
          if((params.sp1MinAge>=0 && sp[0][xm]>=params.sp1MinAge) || (params.sp2MinAge>=0 && sp[1][xm]>=params.sp2MinAge)) abundance++;
          }
        }
      }

  //Inform the precious customer
  cout << "Species occurrence: " << occurrence << endl;

  //row to output log: year, # of substrate cells, # of occupied cells
  outputfile << y << '\t' << abundance1 << '\t' << abundance2 << '\t' << abundance << '\t' << occurrence << endl;

  if (params.useCoordinates) { //Calculate ha-level abundances for listed cells
    int host1Count=0,host2Count=0,hostsCount=0,epiCount=0;
    for (CellIterator i=cells.begin(), end=cells.end();i!=end;i++)  {
      int y,x,ym,xm,c1=0,c2=0,ce=0;
      //Find corresponding cell coordinate
      y=(NWy-20*roffset-(*i).ym)/20;
      x=((*i).xm-NWx-20*coffset)/20;
      if (x<0||x>=cols||y<0||y>=rows) {
        cout << "Error: Coordinates out of range:\t" << x << "\t" << y << endl;
        exit(255);
		}
      //Loop through the 25 20-m cells that belong to the 1-ha cell
      for (int j=-2;j<3;j++) for (int k=-2;k<3;k++) {
        //  For each 20-m cell, check if substrate trees present and add to count
        xm=x+j;
        ym=y+k;
        if (xm>=0&&xm<cols&&ym>=0&&ym<rows) {
          xm=xm+cols*ym;
          if(params.sp1MinAge>=0 && sp[0][xm]>=params.sp1MinAge) c1++;
          if(params.sp2MinAge>=0 && sp[1][xm]>=params.sp2MinAge) c2++;
		  if(population[xm]>0) ce++;
          }
        }
      if (c1>0) host1Count++;
      if (c2>0) host2Count++;
      if (c1>0 || c2>0) hostsCount++;
      if (ce>0) epiCount++;
      }
    halogfile << y << '\t' << host1Count << '\t' << host2Count << '\t'
            << hostsCount << '\t' << epiCount << endl;

    }

  //Output map in erdas format: empty, substrate, age of subpopulation
  if (params.outputfreq==1 || (params.outputfreq>0 && y%params.outputfreq==0) ||
        (params.outputfreq==0&&y==params.startyear+params.simulationlength-params.simulationlength%params.timestep))
    {
    s.clear();
    s.seekp(0,ios_base::beg);
	s << params.outputmapname << y << ends;
	const std::string tmp2 = s.str(); //HF: Modified to solve stringstream - const char* inconsistency
	outputmap = new ErdasFile(tmp2.c_str(),cols,rows); //HF: Modified from outputmap = new ErdasFile(s.str(),cols,rows);
	for (int i=0;i<rows;i++)          //Loop through cells
	  for (int j=0;j<cols;j++) {
		outputmap->W(min(255,population[i*cols+j])); //If species 'colony' age > 255 years, 255 output anyway
		}
	delete outputmap;
	}

  //Output map as a list of coordinates
  if (params.coordinatefreq==1 || (params.coordinatefreq>0 && y%params.coordinatefreq==0) ||
		(params.coordinatefreq==0&&y==params.startyear+params.simulationlength-params.simulationlength%params.timestep))
	{
	s.seekp(0,ios_base::beg);
	s << params.coordinatename << y << ".txt" << ends;
	const std::string tmp3 = s.str(); //HF: Modified to solve stringstream - const char* inconsistency
	coordinatefile.open(tmp3.c_str()); //HF: Modified from coordinatefile.open(s.str());
    params.write(&coordinatefile);
    coordinatefile << "# Note: Age of oldest host tree and age of epiphyte occurrences given below." << endl;
    coordinatefile << "#" << endl;
    coordinatefile << "id\tx20\ty20\tHost1\tHost2\tPopu" << endl;
    n=1;
    for (int i=rows-1;i>=0;i--)          //Loop through cells
      for (int j=0;j<cols;j++) {
        coordinatefile << n << '\t' << (NWx+20*coffset+20*j) << '\t' << (NWy-20*roffset-20*i);
        for (int k=0;k<=1;k++)
		  coordinatefile << '\t' << (sp[k][i*cols+j]);
        coordinatefile << '\t' << (population[i*cols+j]);
        coordinatefile << endl;
        n++;
        }
    coordinatefile.close();
    }

  }

//END OF ITERATION LOOP
//-----------------------------------------------------------------------------

//Close files and clean up
outputfile.close();

if (params.useCoordinates) halogfile.close();

for (int k=fs;k<=ls;k++)
  delete[] sp[k];

delete[] aggregate;
delete[] population;

return 0;
}

float connect(const float distance, const PARAMS& params)
//Calculates connectivity contributions depending on dispersal curve
{
switch (params.curve) {
  case 1 :
	if (distance<20) return 0;
	else return exp(-distance*params.alpha);
  case 2 :
	if (distance<20) return 0;
	else return pow(distance,(-1)*params.alpha);
  case 3 :
	if (distance<20) return 0;
	else return exp(-params.alpha*(log(distance)-params.beta)*(log(distance)-params.beta));
  case 4 :
	if (distance<20) return 0;
	else return exp(-pow((distance*params.alpha),exp(params.c)));
  case 5 :
	if (distance<20) return 0;
	else return pow((1+(1/params.alpha)*pow((distance/params.c),2)),(-(params.alpha+1)/2));
  default :
	cout << "Error: Illegal curve parameter: " << params.curve << endl;
	exit(255);
  }
return 0; //Avoids compiler warning.
}

float celldistance(const int x1, const int y1, const int x2, const int y2, const PARAMS& params)
//Calculates cell distances depending on boundary conditions
{
int dx,dy;
switch (params.boundary) {
  case 1 :    //Absorbing boundaries
	return sqrt (float ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))); //HF: Added float
  case 2 :    //Toroidal boundaries
	dx = min(abs(x1-x2),params.cols-abs(x1-x2));
	dy = min(abs(y1-y2),params.rows-abs(y1-y2));
	return sqrt (float (dx*dx+dy*dy));   //HF: Added float
  default :
    cout << "Error: Illegal boundary parameter: " << params.boundary << endl;
    exit(255);
  }
return 0; //Avoids compiler warning.
}
