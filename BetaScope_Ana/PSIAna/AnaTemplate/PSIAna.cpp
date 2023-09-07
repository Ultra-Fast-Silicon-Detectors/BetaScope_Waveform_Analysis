#include "PSIAna.h"

// user include files
#include "General/WaveformAna/include/Waveform_Analysis.hpp"
#include "General/WaveformAna/include/general.hpp"
#include "csv_read.cpp"

#include <ROOT/TProcessExecutor.hxx>
#include <TROOT.h>
#include <TThread.h>
#include <boost/thread.hpp>
#include <mutex>
#include <stdlib.h>
#include <thread>
//---------------

void loopHelper(WaveformAnalysis *WaveAna, std::vector<double>  *w, std::vector<double> *t, double *pmax, double *pmaxGlobal, double *tmax, double *tmax_ps, double *tmaxGlobal, double *tmaxGlobal_ps, 
    int *max_indexing, int *max_indexing_global, double *negPmax, double *negPmaxGlobal, double *negTmax, double *negTmax_ps, double *negTmaxGlobal, double *negTmaxGlobal_ps, double *pulseArea, double *pmaxOutOfTime, 
    double *tmaxOutOfTime, double *tmaxOutOfTime_ps, double *negPmaxOutOfTime, double *negTmaxOutOfTime, double *negTmaxOutOfTime_ps, double assistThreshold, std::string run_number, int channel) {
  WaveAna->Correct_Baseline2(*w, 0.30);

  std::string fname("/home/psi_tb/Desktop/HGTD_BetaScope_FW_Test/BetaScope_Ana/PSIAna/AnaTemplate/PSIRuns.csv");
  csv_run_reader run_map(fname);

  //Ints correspond to the columns of the CSV file the description correspond to which column does the described knowledge live
  int trig_channel_id = 1;
  int scint_channel_id = 2;

  int  channel_trig_min_id = 3;
  int  channel_trig_max_id = 4;
  int  channelx_min_id = 5;
  int  channelx_max_id = 6;
  int  channel_scint_min_id = 7;
  int  channel_scint_max_id = 8;

  double searchrange[2];
  double unitsToPs = 400.0;
  double nsToUnits = 1000.0/400.0;

  double unitsStartTime = 0;
  double unitsNegPmaxEndTime = 900;
  double unitsEndTime = 1024;

  if (channel == run_map.run_selections(run_number)[trig_channel_id]){
      searchrange[0]=run_map.run_selections(run_number)[channel_trig_min_id]*nsToUnits;
      searchrange[1]=run_map.run_selections(run_number)[channel_trig_max_id]*nsToUnits;
  }
  else if (channel == run_map.run_selections(run_number)[scint_channel_id]){
      searchrange[0]=run_map.run_selections(run_number)[channel_scint_min_id]*nsToUnits;
      searchrange[1]=run_map.run_selections(run_number)[channel_scint_max_id]*nsToUnits;
  }
  else{
      searchrange[0]=(run_map.run_selections(run_number)[channelx_min_id])*nsToUnits;
      searchrange[1]=(run_map.run_selections(run_number)[channelx_max_id])*nsToUnits;
  }



  double windowLowerEnd = searchrange[0];
  double windowUpperEnd = searchrange[1];

  std::pair<double, unsigned int> pmax_pair = WaveAna->Find_Signal_Maximum(*w, *t, 1, searchrange);
  *pmax = pmax_pair.first;
  *tmax = t->at(pmax_pair.second);
  *tmax_ps = *tmax*unitsToPs;
  *max_indexing = pmax_pair.second;

  std::pair<double, unsigned int> pmax_pair_global = WaveAna->Find_Signal_Maximum(*w, *t, 0, 0);
  *pmaxGlobal = pmax_pair_global.first;
  *tmaxGlobal = t->at(pmax_pair_global.second);
  *tmaxGlobal_ps = *tmaxGlobal*unitsToPs;
  *max_indexing_global = pmax_pair_global.second;

  double beforeWindowSearchRange[2] = {unitsStartTime,windowLowerEnd};
  double afterWindowSearchRange[2] = {windowUpperEnd,unitsEndTime};

  std::pair<double, unsigned int> pmax_pair_before_window = WaveAna->Find_Signal_Maximum(*w, *t, 1, beforeWindowSearchRange);
  std::pair<double, unsigned int> pmax_pair_after_window = WaveAna->Find_Signal_Maximum(*w, *t, 1, afterWindowSearchRange);

  if (pmax_pair_before_window.first>=pmax_pair_after_window.first){
      *pmaxOutOfTime = pmax_pair_before_window.first;
      *tmaxOutOfTime = t->at(pmax_pair_before_window.second);
      *tmaxOutOfTime_ps = *tmaxOutOfTime*unitsToPs;
  }
  else {
      *pmaxOutOfTime = pmax_pair_after_window.first;
      *tmaxOutOfTime = t->at(pmax_pair_after_window.second);
      *tmaxOutOfTime_ps = *tmaxOutOfTime*unitsToPs;
  }

 

//neg pmax and tmax

  std::pair<double, unsigned int> negPmax_pair = WaveAna->Find_Negative_Signal_Maximum(*w, *t, 1, searchrange);
  *negPmax = negPmax_pair.first;
  *negTmax = t->at(negPmax_pair.second);
  *negTmax_ps = *negTmax*unitsToPs;

  std::pair<double, unsigned int> negPmax_pair_global = WaveAna->Find_Negative_Signal_Maximum(*w, *t, 0, 0);
  *negPmaxGlobal = negPmax_pair_global.first;
  *negTmaxGlobal = t->at(negPmax_pair_global.second);
  *negTmaxGlobal_ps = *negTmaxGlobal*unitsToPs;

  double negAfterWindowSearchRange[2] = {windowUpperEnd,unitsNegPmaxEndTime};

  std::pair<double, unsigned int> negPmax_pair_before_window = WaveAna->Find_Negative_Signal_Maximum(*w, *t, 1, beforeWindowSearchRange);
  std::pair<double, unsigned int> negPmax_pair_after_window = WaveAna->Find_Negative_Signal_Maximum(*w, *t, 1, negAfterWindowSearchRange);

  if (negPmax_pair_before_window.first<=negPmax_pair_after_window.first){
      *negPmaxOutOfTime = negPmax_pair_before_window.first;
      *negTmaxOutOfTime = t->at(negPmax_pair_before_window.second);
      *negTmaxOutOfTime_ps = *negTmaxOutOfTime*unitsToPs;
  }
  else {
      *negPmaxOutOfTime = negPmax_pair_after_window.first;
      *negTmaxOutOfTime = t->at(negPmax_pair_after_window.second);
      *negTmaxOutOfTime_ps = *negTmaxOutOfTime*unitsToPs;
  }
// other

  *pulseArea = WaveAna->Find_Pulse_Area(*w, *t, pmax_pair);

  return;
}

//---------------

void PSIAna::Initialize() {
  this->beta_scope.FileOpen(this->ifile.c_str());

  char *check_path = getenv("BETASCOPE_SCRIPTS");
  if (check_path != NULL) {
    std::string beta_scope_path(getenv("BETASCOPE_SCRIPTS"));
    BetaScope_AnaFramework::Initialize(
        beta_scope_path +
        "/../BetaScope_Ana/PSIAna/AnaTemplate/myOwnTree.ini");
  } else {
    BetaScope_AnaFramework::Initialize("");
  }

  // do your own stuffs here
  for (int ch = 0; ch < 16; ch++) {
    auto br_check = this->beta_scope.SetInBranch<TTreeReaderArray, double>(
        Form("w%i", ch), Form("w%i", ch));

    br_check = this->beta_scope.BuildOutBranch<std::vector<double>>(Form("w%i", ch));
    w[ch] = this->beta_scope.GetOutBranch<std::vector<double>>(Form("w%i", ch));

    if (ch == 0) {
      br_check = this->beta_scope.SetInBranch<TTreeReaderArray, double>("time", "time");
      br_check = this->beta_scope.BuildOutBranch<std::vector<double>>("time");
      t = this->beta_scope.GetOutBranch<std::vector<double>>("time");
      br_check = this->beta_scope.BuildOutBranch<std::vector<double>>("time_ps");
      t_ps = this->beta_scope.GetOutBranch<std::vector<double>>("time_ps");
      br_check = this->beta_scope.BuildOutBranch<std::vector<double>>("timeTrigger_ps");
      tTrigger_ps = this->beta_scope.GetOutBranch<std::vector<double>>("timeTrigger_ps");
      br_check = this->beta_scope.BuildOutBranch<int>("event");
      this->Event = this->beta_scope.GetOutBranch<int>("event");

       }
  }
  
  for (int i = 0; i < 16; i++) {
    auto br_check = this->beta_scope.BuildBranch<double>(Form("pmax%i", i));
    br_check = this->beta_scope.BuildBranch<double>(Form("pmaxGlobal%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("tmax%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("tmax_ps%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("tmaxGlobal%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("tmaxGlobal_ps%i",i));
    br_check = this->beta_scope.BuildOutBranch<int>(Form("max_indexing%i", i));
    br_check = this->beta_scope.BuildOutBranch<int>(Form("max_indexing_global%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("pulseArea%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negPmax%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negPmaxGlobal%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negTmax%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negTmax_ps%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negTmaxGlobal%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negTmaxGlobal_ps%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("pmaxOutOfTime%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("tmaxOutOfTime%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("tmaxOutOfTime_ps%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negPmaxOutOfTime%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negTmaxOutOfTime%i", i));
    br_check = this->beta_scope.BuildOutBranch<double>(Form("negTmaxOutOfTime_ps%i", i));
    this->pmax[i] = this->beta_scope.GetOutBranch<double>(Form("pmax%i", i));
    this->pmaxGlobal[i] = this->beta_scope.GetOutBranch<double>(Form("pmaxGlobal%i", i));
    this->tmax[i] = this->beta_scope.GetOutBranch<double>(Form("tmax%i", i));
    this->tmax_ps[i] = this->beta_scope.GetOutBranch<double>(Form("tmax_ps%i",i));
    this->tmaxGlobal[i] = this->beta_scope.GetOutBranch<double>(Form("tmaxGlobal%i", i));
    this->tmaxGlobal_ps[i] = this->beta_scope.GetOutBranch<double>(Form("tmaxGlobal_ps%i", i));
    this->max_indexing[i] = this->beta_scope.GetOutBranch<int>(Form("max_indexing%i", i));
    this->max_indexing_global[i] = this->beta_scope.GetOutBranch<int>(Form("max_indexing_global%i", i));
    this->pulseArea[i] = this->beta_scope.GetOutBranch<double>(Form("pulseArea%i", i));
    this->negPmax[i] = this->beta_scope.GetOutBranch<double>(Form("negPmax%i", i));
    this->negPmaxGlobal[i] = this->beta_scope.GetOutBranch<double>(Form("negPmaxGlobal%i", i));
    this->negTmax[i] = this->beta_scope.GetOutBranch<double>(Form("negTmax%i", i));
    this->negTmax_ps[i] = this->beta_scope.GetOutBranch<double>(Form("negTmax_ps%i", i));
    this->negTmaxGlobal[i] = this->beta_scope.GetOutBranch<double>(Form("negTmaxGlobal%i", i));
    this->negTmaxGlobal_ps[i] = this->beta_scope.GetOutBranch<double>(Form("negTmaxGlobal_ps%i", i));
    this->pmaxOutOfTime[i] = this->beta_scope.GetOutBranch<double>(Form("pmaxOutOfTime%i", i));
    this->tmaxOutOfTime[i] = this->beta_scope.GetOutBranch<double>(Form("tmaxOutOfTime%i", i));
    this->tmaxOutOfTime_ps[i] = this->beta_scope.GetOutBranch<double>(Form("tmaxOutOfTime_ps%i", i));
    this->negPmaxOutOfTime[i] = this->beta_scope.GetOutBranch<double>(Form("negPmaxOutOfTime%i", i));
    this->negTmaxOutOfTime[i] = this->beta_scope.GetOutBranch<double>(Form("negTmaxOutOfTime%i", i));
    this->negTmaxOutOfTime_ps[i] = this->beta_scope.GetOutBranch<double>(Form("negTmaxOutOfTime_ps%i", i));

  }
}

//==============================================================================
//==============================================================================
//==============================================================================

void PSIAna::LoopEvents() {
  // double *d = this->beta_scope.get<double>("ws0", "vector<double?");
  int count = 0;
  
  //this is temporary until a better solution is implemented
  std::string fname("/home/psi_tb/Desktop/HGTD_BetaScope_FW_Test/BetaScope_Ana/PSIAna/AnaTemplate/PSIRuns.csv");
  csv_run_reader run_map(fname);
  int trigger_channel_delay_id = 9;
  double triggerTimeDelay = run_map.run_selections(run_number)[trigger_channel_delay_id];
  //

  double verScaler = -1.0;
  double horiScaler = 1.0;
  double unitsToPs = 400.0;
  double nsToPs = 1000.0;

  WaveformAnalysis WaveAna;
  double assistThreshold = 100.0;

  while (this->beta_scope.GetInTreeReader()->Next()) {
    ///*
    for (int i = 0, max = this->beta_scope
                              .GetInBranch<TTreeReaderArray, double>(Form("w0"))
                              ->GetSize();
         i < max; i++) {
      w[0]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w0")->At(i) *
          verScaler);
      w[1]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w1")->At(i) *
          verScaler);
      w[2]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w2")->At(i) *
          verScaler);
      w[3]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w3")->At(i) *
          verScaler);
      w[4]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w4")->At(i) *
          verScaler);
      w[5]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w5")->At(i) *
          verScaler);
      w[6]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w6")->At(i) *
          verScaler);
      w[7]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w7")->At(i) *
          verScaler);
      w[8]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w8")->At(i) *
          verScaler);
      w[9]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w9")->At(i) *
          verScaler);
      w[10]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w10")->At(i) *
          verScaler);
      w[11]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w11")->At(i) *
          verScaler);
      w[12]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w12")->At(i) *
          verScaler);
      w[13]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w13")->At(i) *
          verScaler);
      w[14]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w14")->At(i) *
          verScaler);
      w[15]->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("w15")->At(i) *
          verScaler);
      t->push_back(
          this->beta_scope.GetInBranch<TTreeReaderArray, double>("time")->At(i) *
          horiScaler);
      t_ps->push_back(
	  this->beta_scope.GetInBranch<TTreeReaderArray, double>("time")->At(i) *
          unitsToPs);
      tTrigger_ps->push_back(
	  (this->beta_scope.GetInBranch<TTreeReaderArray, double>("time")->At(i) *
	  unitsToPs) - (triggerTimeDelay * nsToPs));

    }

    // Analysis:================================================================


    WaveformAnalysis *WaveAna_ptr = &WaveAna;
    std::vector<std::thread *> workers;

    for (int channel  = 0; channel < 16; channel++) {
      workers.push_back(new std::thread(
          loopHelper, WaveAna_ptr, this->w[channel], this->t, this->pmax[channel], this->pmaxGlobal[channel],
          this->tmax[channel], this->tmax_ps[channel], this->tmaxGlobal[channel], this->tmaxGlobal_ps[channel], this->max_indexing[channel], this->max_indexing_global[channel], this->negPmax[channel], this->negPmaxGlobal[channel],
          this->negTmax[channel], this->negTmax_ps[channel], this->negTmaxGlobal[channel], this->negTmaxGlobal_ps[channel], this->pulseArea[channel], this->pmaxOutOfTime[channel], this->tmaxOutOfTime[channel], 
	  this->tmaxOutOfTime_ps[channel], this->negPmaxOutOfTime[channel], this->negTmaxOutOfTime[channel], this->negTmaxOutOfTime_ps[channel], assistThreshold, this->run_number, channel));
    }

    for (std::size_t id = 0; id < workers.size(); id++) {
      workers[id]->join();
      delete workers[id];
    }
    *Event = count;
    
    count++;

    BetaScope_AnaFramework::FillData();
  }
}

//==============================================================================
//==============================================================================
//==============================================================================

void PSIAna::_Finalize() {
  // do your own stuffs here

  BetaScope_AnaFramework::Finalize();
}

void PSIAna::Finalize() {
    beta_scope.GetOutFile()->cd();
    PSIAna::_Finalize();
}
