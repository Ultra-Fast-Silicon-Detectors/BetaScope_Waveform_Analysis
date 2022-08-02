#ifndef PSI_ANA
#define PSI_ANA

#include "BetaScope_Driver/include/BetaScopeExt_Class.h"
#include "BetaScope_Driver/include/BetaScopeExt_Templates.tpp"
#include "BetaScope_Driver/include/BetaScope_AnaFramework.h"
#include <iostream>
#include <string>

class PSIAna : public BetaScope_AnaFramework<BetaScope_Ext> {
public:
  std::string ifile;

  PSIAna(){};

  PSIAna(std::string ifile){
    std::cout << "Using user template: "<< "PSIAna" << std::endl;
    std::cout << "on file: " << ifile << std::endl;
    this->ifile = ifile;
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

  std::vector<double> *pmax[16];
  std::vector<double> *tmax[16];
  std::vector<int> *max_indexing[16];

  std::vector<double> *pulseArea[16];
  std::vector<double> *negPmax[16];
  std::vector<double> *negTmax[16];

  // example of filling standalone histograms
  TH1D standAloneHisto = TH1D("standAloneHisto", "standAloneHisto", 100, 1, 1);
  TH1D *standAloneHisto_ptr = 0;

  // template <typename o_type, typename i_type>
  // void copyTTreeReaderArrayToVector( o_type *output_v,
  // TTreeReaderArray<i_type> *input_v);
};

//PSIAna::PSIAna(std::string ifile){this->ifile = ifile;};

//PSIAna::PSIAna(){};
//PSIAna::~PSIAna(){};

int RunAnalysis(std::string ifile);

#endif //PSI_ANA
