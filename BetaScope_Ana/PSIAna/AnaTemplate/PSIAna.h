#ifndef PSI_ANA
#define PSI_ANA

#include "BetaScope_Driver/include/BetaScopeExt_Class.h"
#include "BetaScope_Driver/include/BetaScopeExt_Templates.tpp"
#include "BetaScope_Driver/include/BetaScope_AnaFramework.h"
#include <iostream>
#include <string>


class PSIAna : public BetaScope_AnaFramework<BetaScope_Ext> {
public:
  std::string  run_number;
  std::string ifile;

  PSIAna(){};

  PSIAna(std::string ifile){
      std::cout << "Using user template: "<< "PSIAna" << std::endl;
      std::cout << "on file: " << ifile << std::endl;
      this->ifile = ifile;
      std::string underscore("_");
      std::string PSI_Run_("PSI_Run_");
      for (int i =0; i<ifile.length();i++){
          std::string test_substring(ifile.substr(i,8));
          if (PSI_Run_.compare(test_substring)==0){
              if (underscore.compare(ifile.substr(i+9,1))==0) {
                  run_number=ifile.substr(i+8,1);
              }
              else if (underscore.compare(ifile.substr(i+10,1))==0){
                  run_number=ifile.substr(i+8,2);
              }
              else if (underscore.compare(ifile.substr(i+11,1))==0){
                  run_number=ifile.substr(i+8,3);
              }
          }
      }
  };

  ~PSIAna(){};


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

  std::vector<double> *w[16];
  std::vector<double> *t;
  std::vector<double> *t_ps;
  std::vector<double> *tTrigger_ps;

  double *pmax[16];
  double *pmaxGlobal[16];
  double *tmax[16];
  double *tmax_ps[16];
  double *tmaxGlobal[16];
  double *tmaxGlobal_ps[16];
  int *max_indexing[16];
  int *max_indexing_global[16];

  double *pulseArea[16];
  double *negPmax[16];
  double *negTmax[16];
  double *negTmax_ps[16];

  // example of filling standalone histograms
  //TH1D standAloneHisto = TH1D("standAloneHisto", "standAloneHisto", 100, 1, 1);
  //TH1D *standAloneHisto_ptr = 0;

  // template <typename o_type, typename i_type>
  // void copyTTreeReaderArrayToVector( o_type *output_v,
  // TTreeReaderArray<i_type> *input_v);
};

//PSIAna::PSIAna(std::string ifile){this->ifile = ifile;};

//PSIAna::PSIAna(){};
//PSIAna::~PSIAna(){};

int RunAnalysis(std::string ifile);

#endif //PSI_ANA
