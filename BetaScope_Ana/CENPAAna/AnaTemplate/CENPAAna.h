#ifndef CENPA_ANA
#define CENPA_ANA

#include "BetaScope_Driver/include/BetaScopeExt_Class.h"
#include "BetaScope_Driver/include/BetaScopeExt_Templates.tpp"
#include "BetaScope_Driver/include/BetaScope_AnaFramework.h"
#include <iostream>
#include <string>


class CENPAAna : public BetaScope_AnaFramework<BetaScope_Ext> {
public:
  std::string ifile;

  CENPAAna(){};

  CENPAAna(std::string ifile){
      std::cout << "Using user template: "<< "CENPAAna" << std::endl;
      std::cout << "on file: " << ifile << std::endl;
      this->ifile = ifile;
  };

  ~CENPAAna(){};


  // required, user can add more to the existing methods;
  void Initialize();
  void Analysis(){};
  void LoopEvents();
  void Finalize();
  void _Finalize();
  void run() {
    Initialize();
    LoopEvents();
    Finalize();
  };

  std::vector<float> *w;
  std::vector<float> *t;

  float *pmax;
  float *pmaxGlobal;
  float *tmax;
  int *max_indexing;
  int *Event;

  float *pulseArea;
  float *negPmax;
  float *negTmax;
  float *riseTime;
  float *fallTime;
  float *fwhm;  

  int *run_number;
  float *rbs_angle;
  float *board_angle;
  float *beam_current;
  float *HV;
  float *au_foil;
  float *trg_level;
  float *vertical_scale;
  float *motor_enc;
  float *proton_energy;
  float *IsGood;
  float *pmax_temp;
  
};

int RunAnalysis(std::string ifile);

#endif //CENPA_ANA
