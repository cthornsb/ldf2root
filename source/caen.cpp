#include <fstream>
#include <iostream>

#include "caen.h"

#include "TTree.h"

bool CaenGeneral::SetupTree(TTree *tree_){
	if(!tree_) return false;
	
	tree = tree_;
	
	tree->Branch("caen", &structure);
	
	return true;
}

bool CaenGeneral::ReadEvent(unsigned int *data, unsigned int &index, bool verbose){
	unsigned int type;
	//unsigned int geo;

	unsigned int datum = data[index++];
	if(datum == 0xffffffff){
		while(datum == 0xffffffff)
			datum = data[index++];
	}

	index += 1;
	datum = data[index++];
	
	while(datum != 0xffffffff){
		unsigned short halfWord[2];
		halfWord[0] = 0x0000ffff & datum;
		halfWord[1] = datum >> 16;

		// Reverse the byte order.
		datum = (halfWord[0] << 16) | (halfWord[1]);
		
		type = (datum & ALLH_TYPEMASK) >> ALLH_TYPESHIFT;
		//geo = (datum & ALLH_GEOMASK) >> ALLH_GEOSHIFT;
	
		if (type == DATA) {
			chan = (datum & DATAH_CHANMASK) >> DATAH_CHANSHIFT;
			adc  = (datum & DATAL_DATAMASK);
			overflow  = (datum & DATAL_OVBIT) != 0;
			underflow = (datum & DATAL_UNBIT) != 0;
			valid     = (datum& DATAL_VBIT)   != 0; //Only defined for V775
			structure.Append(chan, adc, overflow, underflow, valid);
		}
		
		if(index + 1 >= 8194) return false;
		
		datum = data[index++];
	}
	
	tree->Fill();
	structure.Zero();
	
	return true;
}	
