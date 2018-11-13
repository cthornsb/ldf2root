#include <iostream>

#include "TH2I.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TApplication.h"

#include "Structures.h"

int main(int argc, char *argv[]){
	if(argc < 4){
		std::cout << " ERROR: Expected 3 arguments but received only " << argc-1 << ".\n";
		std::cout << "  Syntax: " << argv[0] << " <file> <chan1> <chan2>\n";
		return 1;
	}

	TApplication rootapp("rootapp", 0, NULL);

	unsigned short channel1 = strtoul(argv[2], NULL, 0);
	unsigned short channel2 = strtoul(argv[3], NULL, 0);

	std::cout << " Using chan1=" << channel1 << ", chan2=" << channel2 << std::endl;

	TFile *f = new TFile(argv[1], "READ");
	if(!f || !f->IsOpen()) return 1;
	
	TTree *t = (TTree*)f->Get("data");
	if(!t) return 1;
	
	CaenV775EventStructure *str = NULL;
	TBranch *str_b = NULL;
	
	t->SetBranchAddress("caen", &str, &str_b);
	if(!str_b) return 1;
	
	TH2I *h1 = new TH2I("h", "dE vs. E", 4096, 0, 4095, 4096, 0, 4095);

	unsigned short p1, p2;
	for(unsigned int i = 0; i < t->GetEntries(); i++){
		t->GetEntry(i);
		if(!str) continue;
		
		p1 = 0;
		p2 = 0;
		for(size_t j = 0; j < str->chan.size(); j++){
			if(str->chan[j] == channel1) p1 = str->adc[j];
			else if(str->chan[j] == channel2) p2 = str->adc[j];
		}
		
		if(p1 != 0 && p2 != 0){
			h1->Fill(p1, p2);
		}
	}

	TCanvas *can = new TCanvas("can", "Canvas");
	can->cd();
	h1->Draw("COLZ");
	can->WaitPrimitive();

	can->Close();
	f->Close();
	delete can;
	delete f;

	return 0;
}
