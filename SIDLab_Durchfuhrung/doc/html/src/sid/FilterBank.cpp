/*! \file
 * \brief     SID filter banks implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "FilterBank.h"
#include "Voice.h"

#include <iostream>


namespace sid_types
{

using std::array;

FilterBank::FilterBank() :
    lp_(LP),
    bp_(BP),
    hp_(HP),
    bpLp_(BP),
    hpLp_(HP),
    hpBp_(HP),
    hpBpLp_(HP)
{
    reset();
}

i32 FilterBank::sample(i32 a_sample, bool a_lp, bool a_bp, bool a_hp)
{
    array<i32, 8> outputs;

    outputs[0] = a_sample;
    outputs[4] = lp_.sample(outputs[0]);
    outputs[2] = bp_.sample(outputs[0]);
    outputs[1] = hp_.sample(outputs[0]);
    outputs[6] = bpLp_.sample(outputs[4]); // LP, then BP
    outputs[5] = hpLp_.sample(outputs[4]); // LP, then HP
    outputs[3] = hpBp_.sample(outputs[2]); // BP, then HP
    outputs[7] = hpBpLp_.sample(outputs[6]); // LP, then BP, then HP

    int cas = (a_lp << 2) | (a_bp << 1) | (a_hp << 0);

    return outputs[cas];
}

void FilterBank::setCutoffFreq(float a_freq)
{
    for(Filter*& fil : filters_)
        fil->setCutoffFreq(a_freq);
    return;
}

void FilterBank::reset()
{
    filters_[0] = &lp_;
    filters_[1] = &bp_;
    filters_[2] = &hp_;
    filters_[3] = &bpLp_;
    filters_[4] = &hpLp_;
    filters_[5] = &hpBp_;
    filters_[6] = &hpBpLp_;

    for(Filter*& fil : filters_)
        fil->reset();

    return;
}

}  // namespace sid_types
