////////////////////////////////////////
//// general functions               //
////                                //
//////////////////////////////////////
#include "WaveformAna/include/general.hpp"
#include "WaveformAna/include/Waveform_Analysis.hpp"

double xlinearInter(const double x1,
                    const double y1, const double x2,
                    const double y2, const double y)
{
    double x = 0.0;
    if (y1 == y2)
    {
        return x1;
    }
    else
    {
        x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
    }
    return x;
}

//overloaded for floats
float xlinearInter(const float x1,
                    const float y1, const float x2,
                    const float y2, const float y)
{
    float x = 0.0;

    if (y1 == y2)
    {
        return x1;
    }
    else
    {
        x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
    }
    return x;
}
