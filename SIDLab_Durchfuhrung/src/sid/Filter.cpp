/*! \file
 * \brief     SID filters implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "Filter.h"
#include "Waveform.h"

#include <cmath>
#include <iostream>

namespace sid_types
{

using std::max;
using std::min;
using std::size_t;

Filter::Filter(FilterType a_type)
{
    type_ = a_type;
    reset();
}

void Filter::reset()
{
    for(size_t i = 0; i < NTAPS; i++)
    {
        taps_k_[i] = 0;
        sr_[i] = 0;
    }

    sr0_idx_ = 0;
    fCO_ = 0;

    setCutoffFreq(1.0);
}

void Filter::calcFilterCoefficients()
{
    switch(type_)
    {
    case LP:
        calcLPCoefficients();
        break;
    case HP:
        calcHPCoefficients();
        break;
    case BP:
        calcBPCoefficients();
        break;
    default:
        break;
    }
    return;
}

void Filter::setCutoffFreq(float a_freq)
{
    fCO_ = a_freq;
    calcFilterCoefficients();
    return;
}

int32_t Filter::sample(int32_t a_sample)
{
    sr_[sr0_idx_] = a_sample;

    i64 sum_k = 0;
    for(size_t i = 0; i < NTAPS; i++)
    {
        i64 idx = (sr0_idx_-i)+NTAPS;
        u64 uidx = idx%NTAPS;
        sum_k += (i64)(sr_[uidx]) * taps_k_[i];
    }

    sr0_idx_++;
    sr0_idx_ %= NTAPS;

    return sum_k/1000;
}

void Filter::calcLPCoefficients()
{
    i64 lambda_k = (M_PI*fCO_*1000)/(SAMPLERATE_HZ/2);

    size_t n = 0;
    i64 mm_k = 0;
    for(n = 0; n < NTAPS; n++)
    {
        mm_k = n*1000 - ((NTAPS-1)*1000)/2;
        if(mm_k == 0)
        {
            taps_k_[n] = lambda_k/M_PI;
        }

        else
        {
            taps_k_[n] = (1000000*sin((mm_k*lambda_k)/1000000.0))
            /(mm_k * M_PI);
        }

    }

    return;
}

void Filter::calcHPCoefficients()
{
    i64 lambda_k = (M_PI*fCO_*1000)/(SAMPLERATE_HZ/2);

    size_t n = 0;
    i64 mm_k = 0;
    for(n = 0; n < NTAPS; n++)
    {
        mm_k = n*1000 - ((NTAPS-1)*1000)/2;
        if(mm_k == 0)
        {
            taps_k_[n] = 1000-(lambda_k/M_PI);
        }
        else
        {
            taps_k_[n] = -(1000000*sin((mm_k*lambda_k)/1000000.0))
            /(mm_k*M_PI);
        }
    }

    return;
}

void Filter::calcBPCoefficients()
{
    // TODO: Arbitrary bandwidth... but well, it's a bandpass.
    u16 fCO_low = max((i32)fCO_-200, 30);
    u16 fCO_high = min((i32)fCO_+200, 12000);

    i64 lambda_k = (M_PI*fCO_low*1000) / (SAMPLERATE_HZ/2);
    i64 phi_k = (M_PI*fCO_high*1000) / (SAMPLERATE_HZ/2);

    size_t n = 0;
    i64 mm_k = 0;
    for(n = 0; n < NTAPS; n++)
    {
        mm_k = n*1000 - ((NTAPS-1)*1000)/2;
        if(mm_k == 0)
        {
            taps_k_[n] = (phi_k-lambda_k)/M_PI;
        }
        else
        {
            taps_k_[n] = (1000000*(sin((mm_k*phi_k)/1000000.0)
                    - sin((mm_k*lambda_k)/1000000.0)))
                    / (mm_k*M_PI);
        }
    }

    return;
}

} // namespace sid_types
