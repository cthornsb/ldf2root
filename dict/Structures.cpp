#include "Structures.h"

Trace::Trace(const std::string &name_/*=""*/){
	name = name_;
	mult = 0;
}

void Trace::Zero(){
	wave.clear();
	mult = 0;
}

Trace &Trace::Set(const Trace &other_){
	wave = other_.wave;
	return *this;
}

Trace &Trace::Set(Trace *other_){
	wave = other_->wave;
	return *this;
}

void Trace::Append(unsigned short *arr_, const size_t &size_){
	wave.reserve(wave.size()+size_);
	for(size_t i = 0; i < size_; i++){
		wave.push_back(arr_[i]);
	}
	mult++;
}

///////////////////////////////////////////////////////////
// CaenV775EventStructure
///////////////////////////////////////////////////////////
CaenV775EventStructure::CaenV775EventStructure() : Structure("CaenV775EventStructure") {
}

CaenV775EventStructure::CaenV775EventStructure(const CaenV775EventStructure &other_) : Structure("CaenV775EventStructure") {
	Set(other_);
}

void CaenV775EventStructure::Append(const unsigned short &chan_, const unsigned short &adc_, const bool &over_, const bool &under_, const bool &valid_){
	chan.push_back(chan_);
	adc.push_back(adc_);
	over.push_back(over_);
	under.push_back(under_);
	valid.push_back(valid_);
}

void CaenV775EventStructure::Zero(){
	chan.clear();
	adc.clear();
	over.clear();
	under.clear();
	valid.clear();
}

CaenV775EventStructure &CaenV775EventStructure::operator = (const CaenV775EventStructure &other_){
	return Set(other_);
}

CaenV775EventStructure &CaenV775EventStructure::Set(const CaenV775EventStructure &other_){
	chan = other_.chan;
	adc = other_.adc;
	over = other_.over;
	under = other_.under;
	valid = other_.valid;
	return *this;
}

CaenV775EventStructure &CaenV775EventStructure::Set(CaenV775EventStructure *other_){
	chan = other_->chan;
	adc = other_->adc;
	over = other_->over;
	under = other_->under;
	valid = other_->valid;
	return *this;
}
