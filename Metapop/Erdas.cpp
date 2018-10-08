#include "erdas.h"

ErdasFile::ErdasFile (const char* Filename, int mapSideC, int mapSideR, float cs, float xul, float yul, int noClasses)
{
int i;
length = 0; 						//alustetaan kartan koko
prSize = mapSideC*mapSideR;
float ca=cs*cs/10000;
stringstream wholeName; //HF: Modified from ostrstream
wholeName << Filename << ".gis" << ends;

const std::string tmp4 = wholeName.str(); //HF: Modified to solve stringstream - const char* inconsistency
prStream=new ofstream(tmp4.c_str(), ios::binary);  //HF: prStream=new ofstream(wholeName.str(), ios::binary);
if (prStream->fail()) cerr << "Opening output file for Erdas failed" << endl;
else
	{
    *prStream << "HEAD74";
	out(0); out(0); out(1); out(0);
	for (i = 0; i < 6; i++) out(0);
	out(mapSideC % 256); out(mapSideC/256); out(0); out (0);
	out(mapSideR % 256); out(mapSideR/256); out(0); out (0);
    for (i = 0; i < 64; i++) out(0);
	out(17); out(0); out(noClasses); out (0);
    for (i = 0; i < 14; i++) out(0);
    out (2); out (0);
    for (int i=0;i<4;i++) out (((char*)&ca)[i]);
    for (int i=0;i<4;i++) out (((char*)&xul)[i]);
    for (int i=0;i<4;i++) out (((char*)&yul)[i]);
    for (int i=0;i<4;i++) out (((char*)&cs)[i]);
    for (int i=0;i<4;i++) out (((char*)&cs)[i]);
	prStream->flush();
	if (!prStream->good()) cerr << "Writing map header failed" << endl;
	}
}

void ErdasFile::out(int classNo)     //Only for the constructor!
{
	*prStream << (char)classNo;
}

ErdasFile& ErdasFile::operator<< (const char clas)
{
    *prStream << clas;
    if (prStream->fail())
        {
        cout << "Output failure (in Erdas module)." << endl;
        exit(255);
        }
    else length++;
    return *this;
}

int ErdasFile::W (int pixelClass)
{
	if (pixelClass < 0 || pixelClass > 255)
	{
		cerr << "Illegal pixelClass: not 8 bit integer" << endl;
		return 1;
	}
	if (length >= prSize)
        {
        cerr << "Map full already." << endl;
        return 1;
        }
	*prStream << (char)pixelClass;
  	if (prStream->fail())
        {
        cerr << "Writing to map failed." << endl;
        return 1;
        }
	else length++;
	return 0;
}


int ErdasFile::MapFull() const
{
	if (length >= prSize) return 1;
	else return 0;
}

ErdasFile::~ErdasFile ()
{
prStream->close();
delete prStream;
}
