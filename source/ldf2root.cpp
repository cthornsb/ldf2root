#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjString.h"

#include "caen.h"

#define BUFFER_LENGTH 8194
#define BUFFER_LENGTHB 32776

bool process(const char *filename, bool debug=false){
	std::ifstream file(filename, std::ios::binary);

	if(!file.is_open()){
		std::cout << "ERROR: Failed to open input file \"" << filename << "\"\n";
		return false;
	}

	std::string outputFilename = std::string(filename);
	size_t finalPeriod = outputFilename.find_last_of('.');
	if(finalPeriod != std::string::npos){
		std::string extension = outputFilename.substr(finalPeriod);
		if(extension != ".ldf"){
			std::cout << "ERROR: Input file has invalid extension (" << extension << "). ABORT\n";
			return false;
		}
		outputFilename = outputFilename.substr(0, finalPeriod);
	}
	outputFilename += ".root";

	std::cout << " Opening output root file \"" << outputFilename << "\"\n";

	TFile *outfile = new TFile(outputFilename.c_str(), "RECREATE");
	TTree *outtree = new TTree("data", "Caen V775 Output Tree");

	if(!outfile->IsOpen()){
		std::cout << "ERROR: Failed to open output file \"" << outputFilename << "\"\n";
		file.close();
		return false;
	}

	CaenGeneral caen;
	caen.SetupTree(outtree);

	unsigned int numBuffersRead = 0;
	unsigned int numBuffersSkipped = 0;
	unsigned int buffer[BUFFER_LENGTH];
	while(true){
		file.read((char*)buffer, BUFFER_LENGTHB);
		
		if(file.eof()) break;
		
		if(buffer[0] == 0x20464f45){ // EOF buffer
			numBuffersSkipped++;
			file.read((char*)buffer, BUFFER_LENGTHB);
			if(file.eof()) break;
			else if(buffer[0] == 0x20464f45){
				numBuffersSkipped++;
				if(debug) std::cout << "DEBUG: Encountered double EOF buffer\n";
				break;
			}
		}
		else if(buffer[0] == 0x41544144){ // DATA buffer
			numBuffersRead++;
			unsigned int index = 2;
			while(caen.ReadEvent(buffer, index, outtree)){
			}
		}
		else if(buffer[0] == 0x44414544){ // DEAD buffer
			numBuffersSkipped++;
			if(debug) std::cout << "DEBUG: Reading " << buffer[1]*4 << " B of deadtime information\n";
			char *deadtime = new char[buffer[1]*4+1];
			memcpy(deadtime, (char*)&buffer[2], buffer[1]*4);
			deadtime[buffer[1]*4] = '\0';
			std::cout << " 0x44414544 (DEAD): \"" << deadtime << "\"\n";
			TObjString str(deadtime);
			outfile->cd();
			str.Write();
			delete deadtime;
		}
		else{
			numBuffersSkipped++;
			if(debug){
				char header[4];
				memcpy(header, (char*)&buffer[0], 4);
				std::cout << "DEBUG: Skipping buffer 0x" << std::hex << buffer[0] << std::dec << " (" << header << ")\n";
			}
		}
	}
	
	file.close();
	
	std::cout << " Done! Wrote " << outtree->GetEntries() << " entries to output root file.\n";
	std::cout << "  Read " << numBuffersRead << " of " << numBuffersRead+numBuffersSkipped << " buffers from input .ldf file.\n";
	
	outfile->cd();
	outtree->Write();
	outfile->Close();
	
	delete outfile;
	
	return true;
}

int main(int argc, char *argv[]){
	if(argc < 2){
		std::cout << " ERROR: No input filename specified.\n";
		std::cout << "  Syntax: " << argv[0] << " <file1> [file2] ...\n";
		return 1;
	}

	for(int i = 1; i < argc; i++){
		if(i < 10) std::cout << "[0" << i << "] Processing input file \"" << argv[i] << "\"...\n";
		else std::cout << "[" << i << "] Processing input file \"" << argv[i] << "\"...\n";
		process(argv[i], true);
		std::cout << std::endl;
	}

	return 0;
}
