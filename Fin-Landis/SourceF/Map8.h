#ifndef MAP8_H
#define MAP8_H

#include <stdio.h>
//#include "point.h" Useless. JP

#define maxLeg 50

class MAP8
{
public:

MAP8();                     //Constructor.  No dimensions.
MAP8(unsigned long*);		 //Constructor.  Initializes map coordinates
MAP8(int, int);             //Constructor.  Dimensions given.
~MAP8();                    //Destructor.

int read(const char*);      //Read map from a file.
                            //Input file is an ERDAS GIS File with noP
                            //extension.

int write(const char*, unsigned char*, unsigned char*, unsigned char*);
									//Write map to file.  There is
                           //no extension on the file name.  Three arrays
                           //of colors cooresponding to the palette (RGB)
                           //are also included.

int write(const char*, int*, int*, int*);  //Write map to file.  There is no
									//extension on the file name.  Three arrays
                           //of colors cooresponding to the palette (RGB)
                           //are also included.

int write(const char*);    //Write map to a file.  The color array is left
                           //unchanged from input.

int readLegend(char*);     //Read legend from a file.  First line of the
                           //file is a title.  All remaining lines are
                           //legends for up to fifteen classes.

void copy(const MAP8&);    //This will copy the contents of another map into
                           //the current map.  This includes legends, titles,
                           //and current display dimensions.

MAP8& MAP8::operator=(const MAP8&);    //Copy operator.

unsigned char& operator()(int, int); //Returns a single map element.

void dim(int, int);         //This will dimension or redimension the map size.
int cols();                 //Returns number of columns in map.
int rows();                 //Returns number of rows in map.
void rename(char*);         //This will rename the map.
void setHeader(unsigned long[]);  //This will set map coordinates
void assignLeg(int,char*);  //This will assign a new name to a legend element.
void fill(const unsigned char);    //This will fill a map with a single value.
void setCellSize(int);      //This will set the cell size of a pixel.
int high();                 //This will return the highest cell value in map.
float sizeOfCells();        //This will return the cell size of a map.
char* legendItem(int);      //This will return the contents of a legend item.

private:

int numCols, numRows;      //Map dimensions.
unsigned char *data;       //Pointer to data array.
char title[80];            //Title.
char legend[maxLeg][32];   //Legend nodes.
unsigned char largeCell;   //Largest cell value.
float cellSize;            //Cell size in meters.
unsigned char 	red[maxLeg],	//Default red, white and blue color values.
      			green[maxLeg],
      			blue[maxLeg];
unsigned long header[32];  //Default file header.
};

#endif
