#include <string.h>
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
#include "timber.h"

static const int    lsclasses=5;



/////////////////////////////////////////////////////////////////////////////
//			    ASSOCIATED TIMBER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

int TIMBER::damage(FL_POINT p, int &cl)
//This will cause damage from the TIMBER at FL_POINT p.  It will return the
//actual number of cohorts killed.

{
 int i, numCohorts=0;

 sites(p.y,p.x)->lastTimber=0;
 for (int index=1;index<=speciesAttrs.number();index++)
    {
     if (strcmp(harvestSpecies, speciesAttrs(index)->name)==0)
      i=index;
    }

    SPECIE *s=(*sites(p.y,p.x))(i);
    int longevity=speciesAttrs(i)->longevity;
    for (int j=10;j<=longevity;j+=10)
       {
        if (s->query(j))
	   {

		 if (j<640)
		    s->kill(j);
		 numCohorts++;
		 cl=3;
	   }
        }
return numCohorts;
}

int TIMBER::disturb(int row, int col, int totArea, MAP8 &m, FILE *logfile)
//This will activate a timber disturbance at row r and col c.  The size of
//the timber harvest will be totArea where totArea is in number of pixels.
//The output map is contained in m and the output file is logfile.

{
static PILE pile;
FL_POINT p1, p2;
MAP8 dist;       //This will be marked true if an area is disturbed.
int cl,          //Timber class.
    area=0,      //Total area consumed to date.
    numSites=0,  //Number of sites damaged.
    numCohorts=0;//Number of cohorts damaged.
   int N, S, E, W;             // north/southing and east/westing
int snr=sites.numRows(), snc=sites.numColumns();
pile.reset();
dist.dim(sites.numRows(), sites.numColumns());
dist.fill(false);
if (row>snr || row==0 || col>snc || col==0)
    {
     printf("ILLEGAL INITIAL COORDINATE FOR TIMBER.\n");
     exit(0);
    }

dist(row,col)=true;
p1.x=col;
p1.y=row;
pile.push(p1);



while (!pile.isEmpty() && area<totArea)
   {
    p2=pile.pull();
    area++;
    int nCKilled; //printf("timber line 116\n");
    if (sites(p2.y,p2.x)->landUnit->active())
	 nCKilled=damage(p2, cl);
    else
         nCKilled=0;
    numCohorts+=nCKilled; //printf("timber line 121\n");
    if (nCKilled)
       {
        numSites++;
        if (sites(p2.y,p2.x)->landUnit->active() ||
	    sites(p2.y,p2.x)->landUnit->water() ||
	    sites(p2.y,p2.x)->landUnit->lowland())
   	     m(p2.y,p2.x)=(unsigned char)(1+cl);
       }
    int timberDirection=1;
    switch (timberDirection)
         {
	  //   block formation to the NE SE SW NW
	  case 1: N=1; S=1; E=1; W=1;  break; // North
	  case 2: N=1; S=1; E=2; W=1;  break; // East
	  case 3: N=1; S=2; E=1; W=1;  break; // South
	  case 4: N=1; S=1; E=1; W=2;  break; // West

	 }


    //// the below 4 ifs establish sites to put in a timber pile
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
	   pile.push(p1); //printf("timber line 149\n");
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
	   pile.push(p1); //printf("timber line 160\n");
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
	   pile.push(p1); //printf("timber line 171\n");
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
	   pile.push(p1); //printf("timber line 182\n");
	  }
	}
   }
if (numSites>0)
{
 fprintf(logfile,"'(%3.1d,%3.1d)',%d,",col,row,totArea);
 // fflush(logfile);
 fprintf(logfile,"%d, %d\n",numSites, numCohorts);
 // fflush(logfile);
}
return numSites;
}

/////////////////////////////////////////////////////////////////////////////
//			    EXTERNAL FUNCTIONS                                            //
/////////////////////////////////////////////////////////////////////////////

void TIMBER::activate(int itr, int freq[]) //Nim: added int freq[]
//This will simulate timber harvest for a singular Landis iteration.

{
FILE *logfile;         //timber log file.
int  numSitesAct,      //Number of sites.
     siteSize;         //Square meters on a singular site.
char str[80];          //Character string.
float size, arealeft, amtdamaged=0;
int i,j;
int max_sites_examined=0;
int snr, snc;
double MWS;
snr=sites.numRows();
snc=sites.numColumns();
m.dim(snr,snc);
for (i=1;i<=snr;i++)
   for (j=1;j<=snc;j++)
       {
        if (sites(i,j)->landUnit->active())
           m(i,j)=1;
        else
           m(i,j)=0;
       }
if (itr==1)
   cummMap=m;

//Setup timber parameters.
numSitesAct=(sites.numActive())/200;
siteSize=parameters.cellSize*parameters.cellSize;
MWS=(double)meanTimberSize/siteSize;
size=(float)(pow((float)10.0,(float)frandNorm())*MWS)*1.4; //HF: Added two latter floats
arealeft=size;
sprintf(str,"%s/%s",parameters.outputDir,logFn);
if ((logfile=fopen(str,"a"))==NULL)
    errorSys("Directory not found or error opening timber log file",STOP);


//Perform timber harvest.
 while ((max_sites_examined<numSitesAct)&&(amtdamaged<arealeft)&&(arealeft>0))
        {
	 i=irand(1,snr); //Nim: was (rand()%snr)+1
	 j=irand(1,snc); //Nim: was (rand()%snc)+1
	 max_sites_examined++;
         if ((sites(i,j)->landUnit->active())) // && (x<probPerSite))
	    {
	     fprintf(logfile,"%d,",itr*10);
	     amtdamaged=(float)disturb(i,j,(int)size,m,logfile);
	     if (amtdamaged>0)
	      {
	       fflush(logfile);
	       arealeft=arealeft-amtdamaged;
	      }
	      else
	      {
	       fprintf(logfile,"   \n");
	      }

	    }
	}

//Add data to cummMap.
for (i=1;i<=snr;i++)
   for (j=1;j<=snc;j++)
       {
        if (m(i,j)>=2)
           cummMap(i,j)=m(i,j);
       }

if (((itr%freq[3])==0)&&(freq[3]<10)||(itr*10==freq[3])&&(freq[3]>=0)) //Nim: added this line
 {                                                                     //Nim: added this line
    //Write map output file. CSH
    sprintf(str," timber harvest assessment for year %d.",itr*10);
    m.setHeader(sites.getHeader());
    m.rename(str);
    m.assignLeg(0,"Act_Sites");
    m.assignLeg(1,"");
    m.assignLeg(2,"");
    m.assignLeg(3,harvestSpecies);
    m.assignLeg(4,"");
    m.assignLeg(5,"");
    sprintf(str,"%s/%s%d",parameters.outputDir,yearlyFn,itr*10);
    m.setCellSize(parameters.cellSize);
    m.write(str,red,green,blue);
} //Nim: added this line

fclose(logfile);

if (itr==parameters.numberOfIterations)
   {
    //Write cumulative map output file.
    sprintf(str,"Cumulative timber harvest assessment."); //Nim: changed spelling
    cummMap.setHeader(sites.getHeader());
    cummMap.rename(str);
    cummMap.assignLeg(0,"Act_Sites");
    cummMap.assignLeg(1,"");
    cummMap.assignLeg(2,"");
    cummMap.assignLeg(3,harvestSpecies);
    cummMap.assignLeg(4,"");
    cummMap.assignLeg(5,"");
    sprintf(str,"%s/%s",parameters.outputDir,finalFn);
    cummMap.setCellSize(parameters.cellSize);
    cummMap.write(str,red,green,blue);
   }
}

void TIMBER::read(FILE *infile)
//This will read timber parameters into the model.

{

if (fscanc(infile,"%ld",&minTimberSize)!=1)
    errorSys("Error reading in mintimberSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&maxTimberSize)!=1)
    errorSys("Error reading in maxtimberSize from disturbance file.",STOP);
if (fscanc(infile,"%ld",&meanTimberSize)!=1)
    errorSys("Error reading in meantimberSize from disturbance file.",STOP);
if (fscanc(infile,"%s",&harvestSpecies)!=1)
    errorSys("Error reading in meanReturn from disturbance file.",STOP);
if (fscanc(infile,"%s",&yearlyFn)!=1)
    errorSys("Error reading in yearlyFn from disturbance file.",STOP);
if (fscanc(infile,"%s",&finalFn)!=1)
    errorSys("Error reading in finalFn from disturbance file.",STOP);
if (fscanc(infile,"%s",&logFn)!=1)
    errorSys("Error reading in logFn from disturbance file.",STOP);
}

void TIMBER::write(FILE *outfile)
//This will write timber parameters out of the model.

{
fprintf(outfile,"%ld\n",minTimberSize);
fprintf(outfile,"%ld\n",maxTimberSize);
fprintf(outfile,"%ld\n",meanTimberSize);
fprintf(outfile,"%ld\n",meanReturn);
fprintf(outfile,"%s\n",yearlyFn);
fprintf(outfile,"%s\n",finalFn);
fprintf(outfile,"%s\n",logFn);
}

void TIMBER::dump()
//This will dump timber parameters to the CRT screen.

{
printf("%ld\n",minTimberSize);
printf("%ld\n",maxTimberSize);
printf("%ld\n",meanTimberSize);
printf("%ld\n",meanReturn);
printf("%s\n",yearlyFn);
printf("%s\n",finalFn);
printf("%s\n",logFn);
}

void TIMBER::initColors(int *r, int *g, int *b)

{
red=r;
green=g;
blue=b;
}

void TIMBER::initLogFile()
//This will initialize the timber logfile.

{
FILE *logfile;
char str[100];

sprintf(str,"%s/%s",parameters.outputDir,logFn);
remove(str);
if ((logfile=fopen(str,"a"))==NULL)
    errorSys("Error opening timber log file",STOP);
fprintf(logfile,"Year, (x, y), Total_Area, Damag_Area, Damag_Cohorts\n");
// fflush(logfile); CSH
fclose(logfile);
}
