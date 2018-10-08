#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "defines.h"
#include "system1.h"
#include "error.h"
#include "point.h"
#include "pile.h"
#include "map8.h"
#include "specie.h"
#include "landis.h"
#include "wind.h"

static const int    lsclasses=5;
static const float  lifespan[5]={0.0,.2,.5,.7,.85};
static const float  suscept[5]={0.0,.05,.50,.85,1.0};



//////////////////////////////////////////////////////////////////////////////
//			    ASSOCIATED WINDTHROW FUNCTIONS                  //
//////////////////////////////////////////////////////////////////////////////

int WIND::damage(FL_POINT p, int &cl)
//This will cause damage from the windthrow at FL_POINT p.  It will return the
//actual number of cohorts killed.

{
 int numCohorts=0,k;
float fr=frand();
sites(p.y,p.x)->lastWind=0;

for (int i=1;i<=speciesAttrs.number();i++)
	{
	 SPECIE *s=(*sites(p.y,p.x))(i);
	 int longevity=speciesAttrs(i)->longevity;
	 for (int j=10;j<=longevity;j+=10)
		 {
		  if (s->query(j))
		{
				for (k=lsclasses-1;k>=0;k--)
			  if (lifespan[k]<((float)j/(float)longevity))
				  break;
		 if (k>=0 && fr<suscept[k])
			  {
		 if (j<640)
			 s->kill(j);
		 numCohorts++;
		 cl=k;
		}
		}
		  }
	}
return numCohorts;
}

int WIND::disturb(int row, int col, int totArea, MAP8 &m, FILE *logfile,double x)
//This will activate a windthrow disturbance at row r and col c.  The size of
//the windthrow will be totArea where totArea is in number of pixels.
//The output map is contained in m and the output file is logfile.

{
static PILE pile;
FL_POINT p1, p2;
MAP8 dist;       //This will be marked true if an area is disturbed.
int cl=0,          //Windthrow class.
	 area=0,      //Total area consumed to date.
	 numSites=0,  //Number of sites damaged.
	 numCohorts=0;//Number of cohorts damaged.
double //xx, verified new probability on each site needed no more. JP.
		 siteProb; //probability on the site
int windDirection=irand(1,4); //0=none 1=N 2=E 3=S 4=W

int N, S, E, W;             // north/southing and east/westing
int snr=sites.numRows(), snc=sites.numColumns();

pile.reset();
dist.dim(sites.numRows(), sites.numColumns());
dist.fill(false);
if (row>snr || row==0 || col>snc || col==0)
	 {
	  printf("ILLEGAL INITIAL COORDINATE FOR WIND.\n");
	  exit(0);
	 }

dist(row,col)=true;
p1.x=col;
p1.y=row;
pile.push(p1);

fprintf(logfile,"'(%3.1d,%3.1d)',%d,",col,row,totArea);
fflush(logfile);

while (!pile.isEmpty() && area<totArea)
	{
	 p2=pile.pull();
	 area++;
	 int nCKilled=0; //printf("wind line 116\n");
	 if (sites(p2.y,p2.x)->landUnit->active())
	 {
	 siteProb=parameters.PROB1*(1.0/(double)meanReturn);
	 if(x<=siteProb) //JP
		nCKilled=damage(p2, cl);
	 }
	 else
			nCKilled=0;
	 numCohorts+=nCKilled; //printf("wind line 121\n");
	 if (nCKilled)
		 {
		  numSites++;
		  if (sites(p2.y,p2.x)->landUnit->active() ||
		 sites(p2.y,p2.x)->landUnit->water() ||
		 sites(p2.y,p2.x)->landUnit->lowland())
			  m(p2.y,p2.x)=(unsigned char)(1+cl);
		 }
	 //windDirection=0;
	 switch (windDirection)
			{
	  //   block formation to the NE SE SW NW
	  case 1: N=2; S=1; E=1; W=1;  break; // North
	  case 2: N=1; S=1; E=2; W=1;  break; // East
	  case 3: N=1; S=2; E=1; W=1;  break; // South
	  case 4: N=1; S=1; E=1; W=2;  break; // West


	 }


	 //// the below 4 ifs establish sites to put in a windthrow pile
	 //// to be drawn at random
	 if ((W!=0) && (p2.x-W>0) && (!dist(p2.y,p2.x-W)))
		  {
	 p1.x=p2.x-W;
	 p1.y=p2.y;
	 dist(p1.y,p1.x)=true;
	 pile.push(p1);
	 if ((W>1) && (!dist(p2.y, p2.x-W+1)))
	  {
		p1.x=p2.x-W+1;
		dist(p1.y,p1.x)=true;
		pile.push(p1); //printf("wind line 149\n");
	  }
	}
	 if ((E!=0) && (p2.x+E<=snc) && (!dist(p2.y,p2.x+E)))
		  {
	 p1.x=p2.x+E;
	 p1.y=p2.y;
	 dist(p1.y,p1.x)=true;
	 pile.push(p1);
	 if ((E>1) && (!dist(p2.y,p2.x+E-1)))
	  {
		p1.x=p2.x+E-1;
		dist(p1.y,p1.x)=true;
		pile.push(p1); //printf("wind line 160\n");
	  }
	}
	 if ((S!=0) && (p2.y-S>0) && (!dist(p2.y-S,p2.x)))
		  {
	 p1.x=p2.x;
	 p1.y=p2.y-S;
	 dist(p1.y,p1.x)=true;
	 pile.push(p1);
	 if ((S>1) && (!dist(p2.y-S+1,p2.x)))
	  {
		p1.y=p2.y-S+1;
		dist(p1.y,p1.x)=true;
		pile.push(p1); //printf("wind line 171\n");
	  }
	}
	 if ((N!=0) && (p2.y+N<=snr) && (!dist(p2.y+N,p2.x)))
		  {
	 p1.x=p2.x;
	 p1.y=p2.y+N;
	 dist(p1.y,p1.x)=true;
	 pile.push(p1);
	 if ((N>1) && (!dist(p2.y+N-1,p2.x)))
	  {
		p1.y=p2.y+N-1;
		dist(p1.y,p1.x)=true;
		pile.push(p1); //printf("wind line 182\n");
	  }
	}
	}

fprintf(logfile,"%d, %d\n",numSites, numCohorts);
fflush(logfile);
return numSites;
}

///////////////////////////////////////////////////////////////////////////////
//			    EXTERNAL FUNCTIONS				     //
///////////////////////////////////////////////////////////////////////////////

void WIND::activate(int itr, int freq[]) //Nim: added int freq[]
//This will simulate windthrow for a singular Landis iteration.
{}


void WIND::read(FILE *infile)
//This will read windthrow parameters into the model.

{

if (fscanc(infile,"%ld",&minWindSize)!=1)
	 errorSys("Error reading in minWindSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&maxWindSize)!=1)
	 errorSys("Error reading in maxWindSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&meanWindSize)!=1)
	 errorSys("Error reading in meanWindSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&meanReturn)!=1)
	 errorSys("Error reading in meanReturn from disturbance file.",STOP);
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
}

void WIND::write(FILE *outfile)
//This will write windthrow parameters out of the model.

{
fprintf(outfile,"%ld\n",minWindSize);
fprintf(outfile,"%ld\n",maxWindSize);
fprintf(outfile,"%ld\n",meanWindSize);
fprintf(outfile,"%ld\n",meanReturn);
fprintf(outfile,"%s\n",yearlyFn);
fprintf(outfile,"%s\n",finalFn);
fprintf(outfile,"%s\n",logFn);
}

void WIND::dump()
//This will dump windthrow parameters to the CRT screen.

{
printf("%ld\n",minWindSize);
printf("%ld\n",maxWindSize);
printf("%ld\n",meanWindSize);
printf("%ld\n",meanReturn);
printf("%s\n",yearlyFn);
printf("%s\n",finalFn);
printf("%s\n",logFn);
}

void WIND::initColors(int *r, int *g, int *b)

{
red=r;
green=g;
blue=b;
}

void WIND::initLogFile()
//This will initialize the windthrow logfile.

{
FILE *logfile;
char str[100];

sprintf(str,"%s/%s",parameters.outputDir,logFn);
remove(str);
if ((logfile=fopen(str,"a"))==NULL)
	 errorSys("Output directory not found or error opening wind log file",STOP);
fprintf(logfile,"Year, \"(x, y)\", Total_Area, Damag_Area, Damag_Cohorts\n");
fflush(logfile);
fclose(logfile);
}
