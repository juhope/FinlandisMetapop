#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include "defines.h"
#include "system1.h"
#include "error.h"
#include "species.h"
#include "site.h"
#include "map8.h"
#include "reclass.h"
#include "reclass2.h"
#include "fireB.h"
#include "wind.h"
#include "timber.h"
#include "landis.h"
#include "io.h"
#ifdef __BORLANDC__
#include <dir.h>
#endif
#ifdef __UNIX__
#include <sys/stat.h>
#endif

//////////////////////////////////////////////////////////////////////////////
//			NON INTERACTIVE VERSION IMPLEMENTATION		    //
//////////////////////////////////////////////////////////////////////////////

int reclassMethods=0,numAgeMaps=0;
static int red[maxLeg]={  0,  0,100,150,200,  0,  0,  0,150,  0,150,255, 80, 150, 255};           //Nim: changed 22 to 256
static int green[maxLeg]={  0,  0,  0,  0,  0,100,150,255,  0,150,150,255, 80, 150, 255};         //Nim: changed 22 to 256
static int blue[maxLeg]={  0,150,  0,  0,  0,  0,  0,  0,150,150,  0,  0, 80, 150, 255};          //Nim: changed 22 to 256

static int red2[maxLeg]={    0, 70,  0,  0,  0,  0,  0,  0,200,100,255,150,200, 200, 255, 255};   //Nim: changed 22 to 256
static int green2[maxLeg]={  0, 70,  0,  0,  0,150,200,255, 30,200, 50, 50,200,   0, 255};        //Nim: changed 22 to 256
static int blue2[maxLeg]={   0, 70,125,200,255,  0,100,  0, 30, 50, 50,  0,   0, 255};            //Nim: changed 22 to 256

static int red3[maxLeg]={    0, 70,  0,  0,  0,  0,  0,  0,200,100,255,150,200, 200, 255, 255};   //Nim: changed 22 to 256
static int green3[maxLeg]={  0, 70,  0,  0,  0,150,200,255, 30,200, 50, 50,200,   0, 255};        //Nim: changed 22 to 256
static int blue3[maxLeg]={   0, 70,125,200,255,  0,100,  0, 30, 50, 50,  0,   0, 255};            //Nim: changed 22 to 256

static int red4[maxLeg]={    0, 70,  0,  0,  0,  0,  0,  0,200,100,255,150,200, 200, 255, 255};   //Nim: changed 22 to 256
static int green4[maxLeg]={  0, 70,  0,  0,  0,150,200,255, 30,200, 50, 50,200,   0, 255};        //Nim: changed 22 to 256
static int blue4[maxLeg]={   0, 70,125,200,255,  0,100,  0, 30, 50, 50,  0,   0, 255};            //Nim: changed 22 to 256

void HSV_to_RGB(float h, float s, float v,
                int &red, int &green, int &blue)
//This will convert HSV colors to RGB.

{
float f, p, q, t, r, g, b;
int i;

if (h==360.0)
   h=0.0;
h=h/60.0;
i=(int)h;
f=h-i;
p=v*(1-s);
q=v*(1-(s*f));
t=v*(1-(s*(1-f)));
switch (i)
   {
    case 0:   r=v; g=t; b=p; break;
    case 1:   r=q; g=v; b=p; break;
    case 2:   r=p; g=v; b=t; break;
    case 3:   r=p; g=q; b=v; break;
    case 4:   r=t; g=p; b=v; break;
    case 5:   r=v; g=p; b=q; break;
    default:  exit(0);
   }
red=(int)(r*255.0);   //Nim: cast to (int)
green=(int)(g*255.0); //Nim: cast to (int)
blue=(int)(b*255.0);  //Nim: cast to (int)
}

void printMessage
(const char *mssg)
//This will print a message on the CRT screen.

{
printf("%s\n",mssg);
fflush(stdout);
}

void ioInit()
//This will call any computer dependent io calls (i.e. set up graphics
//system) at start up.

{
printMessage("Beginning Landis Run.\n");
}

void putOutput(int rep, int itr, int freq[])
//This will write out a Landis description for this iteration and replicate.

//Changed so that freq[0] is for age reclass also and freq[4] for
//species age reclass. JP.

{
int i;  //Nim: added this line
char str[100];
MAP8 m(sites.getHeader());
//extern freq[];

printMessage("Writing output.");

if (freq[0]&&(((itr%freq[0])==0)&&(freq[0]<10)||(itr*10==freq[0])))
{
	for (i=0;i<reclassMethods;i++) //Nim: changed int i to i
   {
	  printf("reclass %s\n",reMethods[i]);
	  sprintf(str,"%s.rcs",reMethods[i]);
	  fileReclass(m,str);
	  sprintf(str,"%s/%s%d",parameters.outputDir,reMethods[i],itr*10);
	  m.setCellSize(parameters.cellSize);
	  m.write(str,red,green,blue);
   }
	ageReclass(m);
  sprintf(str,"%s/age%d",parameters.outputDir,itr*10);
  m.setCellSize(parameters.cellSize);
  m.write(str,red2,green2,blue2);
  //Added next. JP.
  ageReclassDense(m);
  sprintf(str,"%s/ageD%d",parameters.outputDir,itr*10);
  m.setCellSize(parameters.cellSize);
  m.write(str,red2,green2,blue2);
}

if (freq[4]&&(((itr%freq[4])==0)&&(freq[4]<10)||(itr*10==freq[4])&&(!rep)))
  {
    for (i=0;((i<numAgeMaps)&&(!rep));i++) //Nim: changed int i to i
	  {
	  //printf("reclass\n");

		  sprintf(str,"%s.age",ageMaps[i]);
      speciesAgeMap(m,str);
      sprintf(str,"%s/%s%d",parameters.outputDir,ageMaps[i],itr*10);
      printf("creating %s %s\n",ageMaps[i],"age map");
      m.setCellSize(parameters.cellSize);
      m.write(str,red2,green2,blue2);
    }
  }
if (freq[5]&&(((itr%freq[5])==0)&&(freq[5]<10)||(itr*10==freq[5])&&(!rep)))
  {
    for (i=0;((i<numAgeMaps)&&(!rep));i++) //Nim: changed int i to i
	  {
	  //printf("reclass\n");
      sprintf(str,"%s.age",ageMaps[i]);
      speciesAgeMapDense(m,str);
      sprintf(str,"%s/%sD%d",parameters.outputDir,ageMaps[i],itr*10);
      printf("creating %s %s\n",ageMaps[i],"age map");
      m.setCellSize(parameters.cellSize);
      m.write(str,red2,green2,blue2);
    }
  }

}

void ioTerm()
//This will call any computer dependent io calls (i.e. terminate graphics
//system) at the end of the model.

{
printMessage("Ending Landis Run.\n");
}

#ifdef __UNIX__
#define swap4bytes(srcptr, destptr) *((char*)destptr) = *(((char*)srcptr)+3); \
			  *(((char*)destptr)+1) = *(((char*)srcptr)+2); \
              *(((char*)destptr)+2) = *(((char*)srcptr)+1); \
              *(((char*)destptr)+3) = *((char*)srcptr)
#define ERDi4_c(srcptr, dest)  swap4bytes(srcptr, &dest)

#define swap2bytes(srcptr, destptr) *((char*)destptr) = *(((char*)srcptr)+1);\
              *(((char*)destptr)+1) = *((char*)srcptr)
#define ERDi2_i(srcptr, dest)  dest = 0; swap2bytes(srcptr, (((char*)&dest)+2))
#endif

void inputBin8(FILE* classFile, FILE* mapFile, int yDim, int xDim)
//This will read in the global variable sites from class file and map file.
//Class file is a file containing site descriptions for a set of class values.
//Map file contains the corresponding map.  Thus every value in mapFile
//is represented by the cooresponding class descritption represented in
//classFile.  mapFile is an Erdas 8 bit gis file.  The file pointer is
//placed on the first map element.  yDim and xDim are the (x,y) dimensions
//of the Erdas map.

{
int coverType, numRead, numCovers=0;
unsigned char c;
SITE *s;
s=new SITE[256];
while (!eof(classFile))
{
	s[numCovers].read(classFile);
   numCovers++;
}

for (int i=yDim;i>0;i--)
{
	for (int j=1;j<=xDim;j++)
   {
   	numRead=fread((char*)(&c),1,1,mapFile);
   	coverType=(int)c;
      if (numRead>0)
      {
      	if ((coverType)<numCovers && coverType>0)
             *(sites(i,j))=s[coverType];
         else
             *(sites(i,j))=s[0];
      }
   }
}
delete [] s;
}

void inputBin16(FILE* classFile, FILE* mapFile, int yDim, int xDim)
//This will read in the global variable sites from class file and map file.
//Class file is a file containing site descriptions for a set of class values.
//Map file contains the corresponding map.  Thus every value in mapFile
//is represented by the cooresponding class descritption represented in
//classFile.  mapFile is an Erdas 16 bit gis file.  The file pointer is
//placed on the first map element.  yDim and xDim are the (x,y) dimensions
//of the Erdas map.

{
int coverType, numRead, numCovers=0;
unsigned short c;
SITE *s;

s=new SITE[256];

while (!eof(classFile))
   {
    s[numCovers].read(classFile);
    numCovers++;
   }

for (int i=yDim;i>0;i--)
    {
     for (int j=1;j<=xDim;j++)
         {
          numRead=fread((char*)(&c),2,1,mapFile);
          #ifdef __UNIX__
	     ERDi2_i(&c, coverType);
	  #else
 	     coverType=(int)c;
	  #endif
          if (coverType>31)
             exit(0);
          if (numRead>0)
              {
                if ((coverType)<numCovers && coverType>0)
                   *(sites(i,j))=s[coverType];
	              else
                   *(sites(i,j))=s[0];
              }
         }
    }
delete [] s;  //Added []. JP.
}

void getInput(FILE *infile, WIND &wind, FIRE &fire, TIMBER &timber,
              int freq[], char reMethods[30][15], char ageMaps[30][15])
//This will read in all LANDIS global variables.
//ageMaps instead of ageMap which is not used. JP.


{
int colNum; //Nim: added this line
FILE *saFile, *luFile, *siFile, *rcFile, *roFile, *diFile, *palleteFile,
	  *freqOfOutput,*ageFile;
int i, nRows, nCols;
unsigned long dest[64];
char str[100];
//MAP8 m;
printMessage("Reading input.");
parameters.read(infile);
if ((saFile=fopen(parameters.specAttrFile,"r"))==NULL)
	  errorSys("Species attribute file not found.",STOP);
if ((luFile=fopen(parameters.landUnitFile,"r"))==NULL)
	  errorSys("Landtype input file not found.",STOP);
if ((siFile=fopen(parameters.siteInFile,"rb"))==NULL)
	  errorSys("GIS map input file not found.",STOP);
if ((rcFile=fopen(parameters.reclassInFile,"r"))==NULL)
	  errorSys("Map attribute input file not found.",STOP);
if ((roFile=fopen(parameters.reclassOutFile,"r"))==NULL)
	  errorSys("Map index file not found.",STOP);
if ((diFile=fopen(parameters.disturbance,"r"))==NULL)
	  errorSys("Disturbance input file not found.",STOP);
if ((ageFile=fopen(parameters.ageIndexFile,"r"))==NULL)
	  errorSys("Species age index file not found.",STOP);
speciesAttrs.read(saFile);
landUnits.attach(&speciesAttrs);
landUnits.read(luFile);
SPECIES::attach(&speciesAttrs);

fread((char*)dest,4,32,siFile);
#ifdef __UNIX__
int packType=dest[1]&0xFFFF; //moved here, only for UNIX. JP.
ERDi4_c(&dest[4],nCols);
ERDi4_c(&dest[5],nRows);
#else
nCols=dest[4];
nRows=dest[5];


#endif
sites.setHeader(dest);
sites.dim(speciesAttrs.number(), nRows, nCols);
sites.attach(&landUnits);
#ifdef __UNIX__
if (packType==512)
	 inputBin16(rcFile, siFile, nRows, nCols);
else
	 inputBin8(rcFile, siFile, nRows, nCols);
#else
   inputBin8(rcFile, siFile, nRows, nCols);
#endif
while (!eof(roFile))
	fscanf(roFile,"%s",reMethods[reclassMethods++]);
while (!eof(ageFile))
	fscanf(ageFile,"%s",ageMaps[numAgeMaps++]);
fclose(saFile);
fclose(luFile);
fclose(siFile);
fclose(rcFile);
fclose(roFile);
fclose(ageFile);

if ((freqOfOutput=fopen(parameters.freq_out_put,"r"))!=NULL)
	{
	 int c=0;
	 char temp[80];
	 while (!eof(freqOfOutput))
	  {
		fscanc(freqOfOutput, "%s",temp);
		freq[c]=atoi(temp);
      if((freq[c]>10)&&(freq[c]%10))
      	errorSys("Illegal frequency value encountered.",STOP); //Nim: changed spelling
		c++;
	  }
	 printf("\n");

   //freq[4] now for species age maps. JP.

    if (freq[0]) {
      if(freq[0]<10)
      {
    	printf("%s%i%s\n","Species maps output every------->",freq[0]*10," years.");
      printf("%s%i%s\n","Age maps output every----------->",freq[0]*10," years.");
      }
      else
      {
    	printf("%s%i%s\n","Species map outputs for year---->",freq[0],".");
    	printf("%s%i%s\n","Age map outputs for year-------->",freq[0],".");
      }
     }
    if (freq[4]) {
      if(freq[4]<10)
    	  printf("%s%i%s\n","Species age maps output every----->",freq[4]*10," years.");
      else
     	  printf("%s%i%s\n","Species age map outputs for year-->",freq[4],".");
    printf("\n");
      }
    if (freq[5]) {
      if(freq[5]<10)
    	  printf("%s%i%s\n","Species age maps (dense) output every----->",freq[5]*10," years.");
      else
     	  printf("%s%i%s\n","Species age map (dense) outputs for year-->",freq[5],".");
      }
    printf("\n");
	 fclose(freqOfOutput);
	}
  else
	{
	 freq[0]=1; freq[1]=1; freq[2]=1; freq[3]=1; freq[4]=1;
	 printf("file <FREQ_OUT.PUT> not found\n");
	 printf("creating file\n");
	 printf("frequency of output unknown assuming every iteration\n");
	 if ((freqOfOutput=fopen("freq_out.put","w"))!=NULL)
		{
		 fprintf(freqOfOutput,"#This file establishes the number of years for output for reclass methods#\n");
		 fprintf(freqOfOutput,"#fire, wind, timber, and age class. A one (1) in any field will produce maps#\n");
		 fprintf(freqOfOutput,"#every iteration#\n");
		 fprintf(freqOfOutput,"#output maps for reclass#  1\n");   //JP.
		 fprintf(freqOfOutput,"#output maps for fire# 1\n");
		 fprintf(freqOfOutput,"#output maps for wind# 1\n");
		 fprintf(freqOfOutput,"#output maps for timber# 1\n");
		 fprintf(freqOfOutput,"#output maps for age class# 1\n");      //JP.
		 fclose(freqOfOutput);
		}

	}

if ((palleteFile=fopen("default.plt","r"))!=NULL)
	{
	 for (colNum=0;colNum<=15;colNum++) //Nim: changed int colNum to colNum
		 fscanf(palleteFile,"%d %d %d", &red[colNum], &green[colNum], &blue[colNum]);
	 for (colNum=0;colNum<=15;colNum++) //Nim: changed int colNum to colNum
		 fscanf(palleteFile,"%d %d %d", &red2[colNum], &green2[colNum], &blue2[colNum]);
	 for (colNum=0;colNum<=15;colNum++) //Nim: changed int colNum to colNum
		 fscanf(palleteFile,"%d %d %d", &red3[colNum], &green3[colNum], &blue3[colNum]);
	 for (colNum=0;colNum<=15;colNum++) //Nim: changed int colNum to colNum
		 fscanf(palleteFile,"%d %d %d", &red4[colNum], &green4[colNum], &blue4[colNum]);
	 fclose(palleteFile);
	}
	else
	 {
	  printf("file <DEFAULT.PLT> not found ....  Continuing\n");
	 }

wind.initColors(red3, green3, blue3);
fire.initColors(red4, green4, blue4);
timber.initColors(red2, green2, blue2);

//Set age colors to a spectrum.
for (i=2;i<16;i++)
	{
	 HSV_to_RGB((float)(i-2)/14.0*360.0,1.0,1.0,red2[i],green2[i],blue2[i]);
	}

#ifdef __UNIX__
mkdir(parameters.outputDir, 493);
#else
mkdir(parameters.outputDir);
#endif

//Write landtype map
MAP8 m(sites.getHeader());
luReclass(m);
sprintf(str,"%s/lu",parameters.outputDir);
m.setCellSize(parameters.cellSize);
m.write(str,red,green,blue);

//Process disturbance input file.
while (!eof(diFile))
	 {
	  fscanc(diFile,"%s",str);
	  if (strcmp(str,"[WIND]")==0)
		  {
	 wind.read(diFile);
	 wind.initLogFile();
	}
	  if (strcmp(str,"[FIRE]")==0)
		  {
	 fire.read(diFile);
	 fire.initLogFile();
	}
	  if (strcmp(str,"[HARVEST]")==0)
		  {
	 timber.read(diFile);
	 timber.initLogFile();
	}
	  }
fclose(diFile);
}

void help()
//This will print the landis parameter file format on CRT.
{
printMessage("-------------------------LANDIS PARAMETER INPUT FILE--------------------------");
printMessage("Species Attribute File   #Species vital attribute file#");
printMessage("Landtype Attribute File  #Attributes of each landtype class#");
printMessage("Map File                 #Overlaid map in Erdas GIS format#");
printMessage("Map Attribute File       #Attributes of each map elements#");
printMessage("Maps Indexes File        #Species name indexes for map output#");
printMessage("Age Index File           #Species name indexes for age map output#");
printMessage("Output Directory         #Path for Landis output#");
printMessage("Disturbance File         #Disturbance Input File#");
printMessage("default.plt              #Required in the parameter file directory#");
printMessage("freq_out.put             #Setting for alternative output#");
printMessage("Iteration Number         #Number of iterations of 10 year step#");
printMessage("Random Number Seed       #0: real time seed, others: fixed seed#");
printMessage("Cell Size                #Cell size in meters#");
printMessage("Seed Dispersal Method    #The name of the seed dispersal routine#");
printMessage("Wind Switch              #0-no, 1-standard, 2-mean, 3-strong, 4-light#");
printMessage("Fire Switch              #0-no, 1-standard, 2-mean, 3-strong, 4-light#");
printMessage("Harvesting Switch        #0-harvesting off, 1-harvesting on#");
printMessage("Wind Prob. Coefficient   #Disturbance probability coefficient#");
printMessage("Wind Size Coefficient    #Disturbance size coefficient#");
printMessage("Fire Prob. Coefficient   #Disturbance probability coefficient#");
printMessage("Fire Size Coefficient    #Disturbance size coefficient#");
printMessage("Wind Checking Percentage #The percentage of pixels checked for wind breakout");
printMessage("Fire Checking Percentage #The percentage of pixels checked for fire ignition");
}

void argHelp()
//This will print the valid argument list on CRT
{
printMessage("Usage: LANDIS [-argument [...]] <input file> ");
printMessage(" ");
printMessage("       argument:");
printMessage("       -e: the years environmental change interpreted");
printMessage("       -h: this help menu");
printMessage("       -p: input file help");
printMessage("       -r: the year for reclassification");
}
