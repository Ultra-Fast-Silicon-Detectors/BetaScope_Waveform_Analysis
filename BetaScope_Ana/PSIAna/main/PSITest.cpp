#include "PSIAna/AnaTemplate/PSIAna.h"

#include "General/utilities/include/dir.h"

#include <dirent.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include <boost/thread.hpp>

#include <TROOT.h>
#include <TThread.h>

void runAna(std::string fileName) {
  PSIAna doAna(fileName);
  doAna.Initialize();
  doAna.LoopEvents();
  doAna.Finalize();
}

int main(int argc, char **argv) {

  ROOT::EnableThreadSafety();
  TThread::IsInitialized();
  ROOT::EnableImplicitMT(16);

  std::cout<<argc<<" "<<argv[1]<<std::endl;

  if (argc < 2) {
    runAna(std::string(argv[1]));
    return 0;
  }

  // Get files in directory with pattern
  auto files = BetaScope_Utilities::Dir::getFiles(std::string(argv[1]), std::string(argv[2]));
  for (auto f : files) {
    runAna(f);
  }

  return 0;
}
