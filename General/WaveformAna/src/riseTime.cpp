//////////////////////////////////////////
////                                    //
////  Rise and Fall Time implementation //
////                                    //
//////////////////////////////////////////

//==============================================================================
// Headers

#include "WaveformAna/include/Waveform_Analysis.hpp"
#include "WaveformAna/include/general.hpp"

//-------c++----------------//
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
//------ROOT----------------//
#include <TCanvas.h>
#include <TF1.h>
#include <TGraph.h>
#include <TH1.h>
#include <TImage.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TThread.h>

/*==============================================================================
Find the rise time
  param npoints     := number of sampled points in a waveform.
  param w           := waveform
  param t           := time trace of the waveform
  param pmax_holder := the Pmax tuple that contains the index
  param bottom      := percentage of the start point
  param top         := percentage of the end point

  return rise time of the signal
==============================================================================*/
double WaveformAnalysis::Find_Rise_Time(
    const std::vector<double> &voltageVec,
    const std::vector<double> &timeVec,
    const std::pair<double, unsigned int> &Pmax,
    const double &bottom,
    const double &top
)
{
    double rise = 0.0;

    unsigned int itop = voltageVec.size() - 2, ibottom = 0;

    bool ten = true, ninety = true;

    unsigned int imax = Pmax.second;
    double pmax = Pmax.first;

    double lowerval = pmax * bottom;
    double upperval = pmax * top;

    for (int j = imax; j > -1; j--)
    {
        if (ninety && voltageVec.at(j) < upperval)
        {
            itop = j; // find the index right below 90%
            ninety = false;
        }
        if (ten && voltageVec.at(j) < lowerval)
        {
            ibottom = j; // find the index right below 10%
            ten = false;
        }
        if (!ten && !ninety)
        {
            break;
        }
    }
    if (ibottom == voltageVec.size() - 1)
    {
        ibottom--;
    }
    if (itop == voltageVec.size() - 1)
    {
        itop--;
    }
    // std::cout<<itop<<std::endl;
    // std::cout<<ibottom<<std::endl;
    double tbottom = xlinearInter(timeVec.at(ibottom), voltageVec.at(ibottom), timeVec.at(ibottom + 1), voltageVec.at(ibottom + 1), lowerval);
    double ttop = xlinearInter(timeVec.at(itop), voltageVec.at(itop), timeVec.at(itop + 1), voltageVec.at(itop + 1), upperval);

    rise = ttop - tbottom; // rise
    return rise;
}
/*==============================================================================
Find the rise time
  param npoints     := number of sampled points in a waveform.
  param w           := waveform
  param t           := time trace of the waveform
  param pmax_holder := the Pmax tuple that contains the index
  param bottom      := percentage of the start point
  param top         := percentage of the end point

  return rise time of the signal
  Overloaded for floats
==============================================================================*/
float WaveformAnalysis::Find_Rise_Time(
    const std::vector<float> &voltageVec,
    const std::vector<float> &timeVec,
    const std::pair<float, unsigned int> &Pmax,
    const float &bottom,
    const float &top
)
{
    float rise = 0.0;

    unsigned int itop = voltageVec.size() - 2, ibottom = 0;

    bool ten = true, ninety = true;

    unsigned int imax = Pmax.second;
    float pmax = Pmax.first;

    float lowerval = pmax * bottom;
    float upperval = pmax * top;

    for (int j = imax; j > -1; j--)
    {
        if (ninety && voltageVec.at(j) < upperval)
        {
            itop = j; // find the index right below 90%
            ninety = false;
        }
        if (ten && voltageVec.at(j) < lowerval)
        {
            ibottom = j; // find the index right below 10%
            ten = false;
        }
        if (!ten && !ninety)
        {
            break;
        }
    }
    if (ibottom == voltageVec.size() - 1)
    {
        ibottom--;
    }
    if (itop == voltageVec.size() - 1)
    {
        itop--;
    }
     //std::cout<<itop<<std::endl;
     //std::cout<<ibottom<<std::endl;
    float tbottom = xlinearInter(timeVec.at(ibottom), voltageVec.at(ibottom), timeVec.at(ibottom - 1), voltageVec.at(ibottom - 1), lowerval);
    float ttop = xlinearInter(timeVec.at(itop), voltageVec.at(itop), timeVec.at(itop - 1), voltageVec.at(itop - 1), upperval);

    rise = ttop - tbottom; // rise
    return rise;
}
/*==============================================================================
Find the fall time
  param npoints     := number of sampled points in a waveform.
  param w           := waveform
  param t           := time trace of the waveform
  param pmax_holder := the Pmax tuple that contains the index
  param top         := percentage of the start point
  param bottom      := percentage of the end point

  return fall time of the signal
  Overloaded for floats
==============================================================================*/
float WaveformAnalysis::Find_Fall_Time(
    const std::vector<float> &voltageVec,
    const std::vector<float> &timeVec,
    const std::pair<float, unsigned int> &Pmax,
    const float &top,
    const float &bottom
)
{
    float fall = 0.0;

    unsigned int itop = voltageVec.size() - 2, ibottom = 0;

    int itoohigh = voltageVec.size()-2;

    bool ten = true, ninety = true;

    unsigned int imax = Pmax.second;
    float pmax = Pmax.first;

    float lowerval = pmax * bottom;
    float upperval = pmax * top;

    for (int j = imax; j < itoohigh; j++)
    {
        if (ninety && voltageVec.at(j) < upperval)
        {
            itop = j; // find the index right below 90%
            ninety = false;
        }
        if (ten && voltageVec.at(j) < lowerval)
        {
            ibottom = j; // find the index right below 10%
            ten = false;
        }
        if (!ten && !ninety)
        {
            break;
        }
    }
    if (ibottom == voltageVec.size() - 1)
    {
        ibottom--;
    }
    if (itop == voltageVec.size() - 1)
    {
        itop--;
    }

    float tbottom = xlinearInter(timeVec.at(ibottom), voltageVec.at(ibottom), timeVec.at(ibottom + 1), voltageVec.at(ibottom + 1), lowerval);
    float ttop = xlinearInter(timeVec.at(itop), voltageVec.at(itop), timeVec.at(itop + 1), voltageVec.at(itop + 1), upperval);

    //std::cout<<tbottom<<std::endl;
    //std::cout<<ttop<<std::endl;

    fall = tbottom - ttop; // fall
    return fall;
}
/*==============================================================================
Find the full width half max
  param npoints     := number of sampled points in a waveform.
  param w           := waveform
  param t           := time trace of the waveform
  param pmax_holder := the Pmax tuple that contains the index

  return full width half max
  Overloaded for floats
==============================================================================*/
float WaveformAnalysis::Find_FWHM(
    const std::vector<float> &voltageVec,
    const std::vector<float> &timeVec,
    const std::pair<float, unsigned int> &Pmax
)
{
    float fwhm = 0.0;

    unsigned int iright = voltageVec.size() - 2, ileft = 0;
 
    int itoohigh = voltageVec.size()-2;

    unsigned int imax = Pmax.second;
    float pmax = Pmax.first;

    float halfval = pmax * 0.5;

    for (int j = imax; j < itoohigh; j++)
    {
        if (voltageVec.at(j) < halfval)
        {
            iright = j; // find the index right below 50%
	    break;
        }
    }

    for (int j = imax; j > -1; j--)
    {
        if (voltageVec.at(j) < halfval)
        {
            ileft = j; // find the index right below 50%
	    break;
        }
    }

    if (ileft == voltageVec.size() - 1)
    {
        ileft--;
    }
    if (iright == voltageVec.size() - 1)
    {
        iright--;
    }
    float tleft  = xlinearInter(timeVec.at(ileft), voltageVec.at(ileft), timeVec.at(ileft + 1), voltageVec.at(ileft + 1), halfval);
    float tright = xlinearInter(timeVec.at(iright), voltageVec.at(iright), timeVec.at(iright + 1), voltageVec.at(iright + 1), halfval);

    fwhm = tright - tleft; // FWHM
    return fwhm;
}
