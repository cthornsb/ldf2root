#include <iostream>

#include "TH2I.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TCanvas.h"

#include "Structures.h"

int main(){
	TFile *f = new TFile("~/data/ldf/intensity_test02.root", "READ");
	if(!f || !f->IsOpen()) return 1;
	
	TTree *t = (TTree*)f->Get("data");
	if(!t) return 1;
	
	CaenV775EventStructure *str;
	TBranch *str_b = NULL;
	
	t->SetBranchAddress("caen", &str, &str_b);
	if(!str_b) return 1;
	
	TH2I *h = new TH2I("h", "dE vs. E", 4096, 0, 4095, 4096, 0, 4095);

	const unsigned short channel1 = 17;
	const unsigned short channel2 = 18;
	
	unsigned short p1, p2;
	for(unsigned int i = 0; i < 1; i++){
		t->GetEntry(i);
		
		p1 = 0;
		p2 = 0;
		for(size_t j = 0; j < str->chan.size(); j++){
			if(str->chan[j] == channel1) p1 = str->adc[j];
			else if(str->chan[j] == channel2) p2 = str->adc[j];
		}
		
		if(p1 != 0 && p2 != 0){
			h->Fill(p1, p2);
		}
	}

	TCanvas *can = new TCanvas("can", "Canvas");
	can->cd();
	h->Draw("COLZ");
	can->WaitPrimitive();

	can->Close();
	f->Close();
	delete can;
	delete f;
	delete h;

	return 0;
}
