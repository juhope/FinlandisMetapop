
#include "params.h"

PARAMS::PARAMS()
//Constructor.

{
outputfilename = new char[100];
coordinatename = new char[100];
outputmapname = new char[100];
sp1name = new char[100];
sp2name = new char[100];
}


PARAMS::~PARAMS()
//Destructor.

{
delete [] outputfilename;
delete [] coordinatename;
delete [] outputmapname;
delete [] sp1name;
delete [] sp2name;
}


void PARAMS::read(ifstream* infile)
//Read in all parameters from an input file stream.

{
*infile >> sp1name; nextline(infile);
*infile >> sp2name; nextline(infile);
*infile >> sp1MinAge; nextline(infile);
*infile >> sp2MinAge; nextline(infile);
*infile >> maturity; nextline(infile);
*infile >> curve; nextline(infile);
*infile >> alpha; nextline(infile);
*infile >> beta; nextline(infile);
*infile >> c; nextline(infile); //HF: Added for 2p exponential disp kernel
*infile >> colonization1; nextline(infile);
*infile >> colonization2; nextline(infile);
*infile >> exponent1; nextline(infile);
*infile >> exponent2; nextline(infile);
*infile >> mortality; nextline(infile);
*infile >> incidence; nextline(infile);
*infile >> boundary; nextline(infile);
*infile >> aggregation; nextline(infile);
*infile >> exactradius; nextline(infile);
*infile >> startyear; nextline(infile);
*infile >> simulationlength; nextline(infile);
*infile >> timestep; nextline(infile);
*infile >> rows; nextline(infile);
*infile >> cols; nextline(infile);
*infile >> roffset; nextline(infile);
*infile >> coffset; nextline(infile);
*infile >> mrows; nextline(infile);
*infile >> mcols; nextline(infile);
*infile >> NWx; nextline(infile);
*infile >> NWy; nextline(infile);
*infile >> useCoordinates; nextline(infile);
*infile >> outputfilename; nextline(infile);
*infile >> outputmapname; nextline(infile);
*infile >> coordinatename; nextline(infile);
*infile >> outputfreq; nextline(infile);
*infile >> coordinatefreq; nextline(infile);

return;
}



void PARAMS::write(ofstream* outfile)
//This shall write the contents of parameters to outfile

{
*outfile << "# " << "Parameters used:" << endl;
*outfile << "# " << sp1name << '\t' << "Minimum age of substrate aspens" << endl;
*outfile << "# " << sp2name << '\t' << "Minimum age of substrate willows" << endl;
*outfile << "# " << sp1MinAge << '\t' << "Minimum age of species 1 suitable as substrate" << endl;
*outfile << "# " << sp2MinAge << '\t' << "Minimum age of species 2 suitable as substrate" << endl;
*outfile << "# " << maturity << '\t' << "Reproductive age of simulated species" << endl;
*outfile << "# " << curve << '\t' << "Dispersal curve: 1-exponential, 2-power, 3-lognorm" << endl;
*outfile << "# " << alpha << '\t' << "'Alpha' for dispersal curve" << endl;
*outfile << "# " << beta << '\t' << "'Beta' for dispersal curve" << endl;
*outfile << "# " << colonization1 << '\t' << "Colonization rate coefficient on host 1" << endl;
*outfile << "# " << colonization2 << '\t' << "Colonization rate coefficient on host 2" << endl;
*outfile << "# " << exponent1 << '\t' << "Exponent of host1 age effect on colonization" << endl;
*outfile << "# " << exponent2 << '\t' << "Exponent of host2 age effect on colonization" << endl;
*outfile << "# " << mortality << '\t' << "Random extinction rate" << endl;
*outfile << "# " << incidence << '\t' << "Initial proportion of substrate cells occupied by focal species" << endl;
*outfile << "# " << boundary << '\t' << "Boundary conditions: 1-absorbing, 2-toroidal" << endl;
*outfile << "# " << aggregation << '\t' << "Aggregation range for optimization" << endl;
*outfile << "# " << exactradius << '\t' << "Radius for exact dispersal calculation" << endl;
*outfile << "# " << startyear << '\t' << "Start year (in Fin-Landis output)" << endl;
*outfile << "# " << simulationlength << '\t' << "Simulation length" << endl;
*outfile << "# " << timestep << '\t' << "Timestep length" << endl;
*outfile << "# " << rows << '\t' << "Rows in simulation map" << endl;
*outfile << "# " << cols << '\t' << "Columns in simulation map" << endl;
*outfile << "# " << roffset << '\t' << "Row offset relative to input map" << endl;
*outfile << "# " << coffset << '\t' << "Col offset" << endl;
*outfile << "# " << mrows << '\t' << "Rows in input maps" << endl;
*outfile << "# " << mcols << '\t' << "Cols in input maps" << endl;
*outfile << "# " << NWx << '\t' << "x coordinate of the center of NW-most cell of input map" << endl;
*outfile << "# " << NWy << '\t' << "y coordinate of the center of NW-most cell of input map" << endl;
*outfile << "# " << useCoordinates << '\t' << "ha-cell coordinate input list in use: 1-yes, 0-no" << endl;
*outfile << "# " << outputfilename << '\t' << "Output log name (this file)" << endl;
*outfile << "# " << outputmapname << '\t' << "Stem for output map names" << endl;
*outfile << "# " << coordinatename << '\t' << "Stem for coordinate output file name" << endl;
*outfile << "# " << outputfreq << '\t' << "Map output frequency" << endl;
*outfile << "# " << coordinatefreq << '\t' << "Coordinate output frequency" << endl;

*outfile << "#" << endl << "#" << endl;
return;
}

void PARAMS::nextline(ifstream* in)
{
char c;
do in->get(c);
while (c!='\n'&&!in->eof());
}
