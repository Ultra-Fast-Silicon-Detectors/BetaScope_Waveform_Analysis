#include "CENPAAna.h"

// user include files
#include "General/WaveformAna/include/Waveform_Analysis.hpp"
#include "General/WaveformAna/include/general.hpp"

#include <ROOT/TProcessExecutor.hxx>
#include <TROOT.h>
#include <TThread.h>
#include <boost/thread.hpp>
#include <mutex>
#include <stdlib.h>
#include <thread>
#include <TTreeReaderValue.h>
//---------------

void loopHelper(WaveformAnalysis *WaveAna, std::vector<float>  *w, std::vector<float> *t, float *pmax, float *tmax, 
    int *max_indexing, float *negPmax, float *negTmax, float *pulseArea, float *riseTime, float *fallTime, float *fwhm, float assistThreshold) {
  WaveAna->Correct_Baseline2(*w, 0.30);

  std::pair<float, unsigned int> pmax_pair = WaveAna->Find_Signal_Maximum(*w, *t, 0, 0);
  *pmax = pmax_pair.first;
  *tmax = t->at(pmax_pair.second);
  *max_indexing = pmax_pair.second;

//neg pmax and tmax

  std::pair<float, unsigned int> negPmax_pair = WaveAna->Find_Negative_Signal_Maximum(*w, *t, 0, 0);
  *negPmax = negPmax_pair.first;
  *negTmax = t->at(negPmax_pair.second);

// other

  *pulseArea = WaveAna->Find_Pulse_Area(*w,*t,pmax_pair,0);
  float underShoot = WaveAna->Find_Udershoot_Area(*w,*t,pmax_pair);
  *pulseArea = *pulseArea + underShoot;

// timing information
  *riseTime = WaveAna->Find_Rise_Time(*w, *t, pmax_pair, 0.1, 0.9); 
  *fallTime = WaveAna->Find_Fall_Time(*w, *t, pmax_pair, 0.9, 0.1);
  *fwhm =     WaveAna->Find_FWHM(*w, *t, pmax_pair);

  return;
}

//---------------

void CENPAAna::Initialize() {
  this->beta_scope.FileOpen(this->ifile.c_str());

  char *check_path = getenv("BETASCOPE_SCRIPTS");
  if (check_path != NULL) {
    std::string beta_scope_path(getenv("BETASCOPE_SCRIPTS"));
    BetaScope_AnaFramework::Initialize();
  } else {
    BetaScope_AnaFramework::Initialize("");
  }

  // do your own stuffs here
  auto br_check = this->beta_scope.SetInBranch<TTreeReaderArray, float>("w2", "w2");
  br_check = this->beta_scope.BuildOutBranch<std::vector<float>>("w");
  w = this->beta_scope.GetOutBranch<std::vector<float>>("w");

  br_check = this->beta_scope.SetInBranch<TTreeReaderArray, float>("t2", "t2");
  br_check = this->beta_scope.BuildOutBranch<std::vector<float>>("time");
  t = this->beta_scope.GetOutBranch<std::vector<float>>("time");
  br_check = this->beta_scope.BuildOutBranch<int>("event");
  this->Event = this->beta_scope.GetOutBranch<int>("event");
  br_check = this->beta_scope.BuildOutBranch<float>("pmax");
  br_check = this->beta_scope.BuildOutBranch<float>("tmax");
  br_check = this->beta_scope.BuildOutBranch<int>("max_indexing");
  br_check = this->beta_scope.BuildOutBranch<float>("pulseArea");
  br_check = this->beta_scope.BuildOutBranch<float>("negPmax");
  br_check = this->beta_scope.BuildOutBranch<float>("negTmax");
  br_check = this->beta_scope.BuildOutBranch<float>("riseTime");
  br_check = this->beta_scope.BuildOutBranch<float>("fallTime");
  br_check = this->beta_scope.BuildOutBranch<float>("fwhm");
  this->pmax = this->beta_scope.GetOutBranch<float>("pmax");
  this->tmax = this->beta_scope.GetOutBranch<float>("tmax");
  this->max_indexing = this->beta_scope.GetOutBranch<int>("max_indexing");
  this->pulseArea = this->beta_scope.GetOutBranch<float>("pulseArea");
  this->negPmax = this->beta_scope.GetOutBranch<float>("negPmax");
  this->negTmax = this->beta_scope.GetOutBranch<float>("negTmax");
  this->riseTime = this->beta_scope.GetOutBranch<float>("riseTime");
  this->fallTime = this->beta_scope.GetOutBranch<float>("fallTime");
  this->fwhm = this->beta_scope.GetOutBranch<float>("fwhm");

  //Meta Information
  br_check = this->beta_scope.BuildOutBranch<int>("run_number");
  br_check = this->beta_scope.BuildOutBranch<float>("rbs_angle");
  br_check = this->beta_scope.BuildOutBranch<float>("board_angle");
  br_check = this->beta_scope.BuildOutBranch<float>("beam_current");
  br_check = this->beta_scope.BuildOutBranch<float>("HV");
  br_check = this->beta_scope.BuildOutBranch<float>("au_foil");
  br_check = this->beta_scope.BuildOutBranch<float>("trg_level");
  br_check = this->beta_scope.BuildOutBranch<float>("vertical_scale");
  br_check = this->beta_scope.BuildOutBranch<float>("motor_enc");
  br_check = this->beta_scope.BuildOutBranch<float>("proton_energy");
  br_check = this->beta_scope.BuildOutBranch<float>("IsGood");
  br_check = this->beta_scope.BuildOutBranch<float>("pmax_temp");

  this->run_number = this->beta_scope.GetOutBranch<int>("run_number");
  this->rbs_angle = this->beta_scope.GetOutBranch<float>("rbs_angle");
  this->board_angle = this->beta_scope.GetOutBranch<float>("board_angle");
  this->beam_current = this->beta_scope.GetOutBranch<float>("beam_current");
  this->HV = this->beta_scope.GetOutBranch<float>("HV");
  this->au_foil = this->beta_scope.GetOutBranch<float>("au_foil");
  this->trg_level = this->beta_scope.GetOutBranch<float>("trg_level");
  this->vertical_scale = this->beta_scope.GetOutBranch<float>("vertical_scale");
  this->motor_enc = this->beta_scope.GetOutBranch<float>("motor_enc");
  this->proton_energy = this->beta_scope.GetOutBranch<float>("proton_energy");
  this->IsGood = this->beta_scope.GetOutBranch<float>("IsGood");
  this->pmax_temp = this->beta_scope.GetOutBranch<float>("pmax_temp");
}

//==============================================================================
//==============================================================================
//==============================================================================

void CENPAAna::LoopEvents() {
  int count = 0;
  
  float verScaler = -1.0;
  float horiScaler = 1.0;

  WaveformAnalysis WaveAna;
  float assistThreshold = 100.0;

  TTreeReader *InTreeReader = this->beta_scope.GetInTreeReader(); 
  TTreeReaderValue<int> inrun_number(*InTreeReader, "run_number");
  TTreeReaderValue<float> inrbs_angle(*InTreeReader, "rbs_angle");
  TTreeReaderValue<float> inboard_angle(*InTreeReader, "board_angle");
  TTreeReaderValue<float> inbeam_current(*InTreeReader, "beam_current");
  TTreeReaderValue<float> inHV(*InTreeReader, "HV");
  TTreeReaderValue<float> inau_foil(*InTreeReader, "au_foil");
  TTreeReaderValue<float> intrg_level(*InTreeReader, "trg_level");
  TTreeReaderValue<float> invertical_scale(*InTreeReader, "vertical_scale");
  TTreeReaderValue<float> inmotor_enc(*InTreeReader, "motor_enc");
  TTreeReaderValue<float> inproton_energy(*InTreeReader, "proton_energy");
  TTreeReaderValue<float> inIsGood(*InTreeReader, "IsGood");
  TTreeReaderValue<float> inpmax_temp(*InTreeReader, "pmax_temp");



  while (InTreeReader->Next()) {
    ///*
    for (int i = 0, max = this->beta_scope.GetInBranch<TTreeReaderArray, float>(Form("w2"))->GetSize(); i < max; i++) {
      w->push_back(this->beta_scope.GetInBranch<TTreeReaderArray, float>("w2")->At(i) * verScaler);
      t->push_back(this->beta_scope.GetInBranch<TTreeReaderArray, float>("t2")->At(i) * horiScaler);
    }

    //filling meta data
    *(this->run_number) = *inrun_number;
    *(this->rbs_angle) = *inrbs_angle;
    *(this->board_angle) = *inboard_angle;
    *(this->beam_current) = *inbeam_current;
    *(this->HV) = *inHV;
    *(this->au_foil) = *inau_foil;
    *(this->trg_level) = *intrg_level;
    *(this->vertical_scale) = *invertical_scale;
    *(this->motor_enc) = *inmotor_enc;
    *(this->proton_energy) = *inproton_energy;
    *(this->IsGood) = *inIsGood;
    *(this->pmax_temp) = *inpmax_temp;



    // Analysis:================================================================


    WaveformAnalysis *WaveAna_ptr = &WaveAna;

    loopHelper(WaveAna_ptr, this->w, this->t, this->pmax, this->tmax, this->max_indexing, this->negPmax, this->negTmax, this->pulseArea, this->riseTime, this->fallTime, this->fwhm, assistThreshold);
    
    *Event = count;    
    count++;

    BetaScope_AnaFramework::FillData();
  }
}

//==============================================================================
//==============================================================================
//==============================================================================

void CENPAAna::_Finalize() {
  // do your own stuffs here

  BetaScope_AnaFramework::Finalize();
}

void CENPAAna::Finalize() {
    beta_scope.GetOutFile()->cd();
    CENPAAna::_Finalize();
}
