//Luokan konstruktori avaa ERDAS 7.4 8 bit tiedoston kirjoitettavaksi.
//Tiedostonimi annetaan parametrina, samoin kuin rasterikartan sivu
//pikselein‰. Palauttaa 1 jos avaaminen ep‰onnistuu.
//
//Staattinen length laskee kirjoitettujen tavujen m‰‰r‰‰, jos laskee?.
//
//J‰senfunktio w kirjoittaa annetun int-luvun (0-255)
//tiedostoon 8 bitill‰ ja palauttaa 0. Jos kuitenkin length = side^2,
//tai kirjoitus muuten ep‰onnistuu, ei en‰‰ kirjoiteta ja palautetaan 1.
//
//MapFull = 1, jos kartta on jo t‰ysi, eli length = side^2.
//
//

#ifndef _ErdasFile_
#define _ErdasFile_
#include <fstream.h>
#include <sstream> //HF: Updated from strstream.h
#include <iostream.h>
#include <stdlib.h>

class ErdasFile
{
public:
	ErdasFile (const char*, int, int, float cs=20, float xul=0, float yul=0, int noClasses=256);
    int W (int pixelClass);
    ErdasFile& operator<< (const char clas);
	int MapFull () const;
	~ErdasFile ();
private:
	int length;
    int prSize;
    ofstream* prStream;
	void out (int classNo);
};

#endif
