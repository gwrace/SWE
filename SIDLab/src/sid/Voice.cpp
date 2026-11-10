/*! \file
 * \brief     SID voices implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "Voice.h"

#include <utility>
#include <cmath>


namespace sid_types
{

using std::array;
using std::pair;
using std::max;

const i64 Voice::scale_ = (1 << 24);
const array<u32, 16> Voice::attackNSamples_
= { 88,
    353,
    706,
    1058,
    1676,
    2470,
    2999,
    3528,
    4410,
    11025,
    22050,
    35280,
    44100,
    132300,
    220500,
    352800 };
const array<u32, 16> Voice::decayReleaseNSamples_
= { 265,
    1058,
    2117,
    3175,
    5027,
    7409,
    8996,
    10584,
    13230,
    33075,
    66150,
    105840,
    132300,
    396900,
    661500,
    1058400 };

Voice::Voice()
{
    waveforms_[TRI] = &triangle_;
    waveforms_[NOISE] = &noise_;
    waveforms_[SAW] = &sawtooth_;
    waveforms_[PULSE] = &pulse_;
    reset();
}

void Voice::reset()
{
    waveform_enabled_[TRI] = false;
    waveform_enabled_[NOISE] = false;
    waveform_enabled_[SAW] = false;
    waveform_enabled_[PULSE] = false;

    freq_ = 0;
    pulseWidth_ = 0;

    adsr_ampl_s_ = 0;
    gate_ = false;
    gate_prev_ = false;
    adsr_state_ = ADSR::OUTSIDE;

    output_ = 0;

    attack_.idx_ = 0;
    attack_.nSamples_ = 0;
    attack_.begin_nSamples_ = 0;
    attack_.offs_nSamples_ = 0;
    attack_.end_nSamples_ = 0;
    attack_.m_s_ = 0;
    attack_.peakAmpl_s_ = 0;

    decay_.idx_ = 0;
    decay_.nSamples_ = 0;
    decay_.begin_nSamples_ = 0;
    decay_.end_nSamples_ = 0;
    decay_.m_s_= 0 ;
    decay_.b_s_= 0 ;
    decay_.lastAmpl_s_ = 0;

    sustain_.idx_ = 0;
    sustain_.level_ = 0;
    sustain_.ampl_s_ = 0;

    release_.idx_ = 0;
    release_.nSamples_ = 0;
    release_.begin_nSamples_ = 0;
    release_.end_nSamples_ = 0;
    release_.m_s_ = 0;
    release_.b_s_ = 0;
    release_.lastAmpl_s_ = 0;

    for(pair<const WaveformType, Waveform*>& wf : waveforms_)
        wf.second->reset();

    return;
}

void Voice::setFreqLo(b8 a_value)
{
    freq_ = (freq_&0xFF00) |  a_value;
    applyFreqToAllWaveforms();
    return;
}

void Voice::setFreqHi(b8 a_value)
{
    b16 val = a_value;
    freq_ = (freq_&0x00FF) | (val << 8);
    applyFreqToAllWaveforms();
    return;
}

void Voice::setPWLo(b8 a_value)
{
    pulseWidth_ = (pulseWidth_&0xFF00) | a_value;
    pulse_.setPulseWidth(pulseWidth_);
    return;
}

void Voice::setPWHi(b8 a_value)
{
    b16 val = a_value;
    pulseWidth_ = (pulseWidth_&0x00FF) | (val << 8);
    pulse_.setPulseWidth(pulseWidth_);
    return;
}

void Voice::applyFreqToAllWaveforms()
{
    b16 freq = (double)freq_*1000000.0/16777216.0;
    for(pair<const WaveformType, Waveform*>& wf : waveforms_)
        wf.second->setFrequency(freq);
    return;
}

void Voice::setAttackDecay(b8 a_value)
{
    attack_.idx_ = a_value >> 4;
    decay_.idx_ = a_value & 0b00001111;
    return;
}
void Voice::setSustainRelease(b8 a_value)
{
    sustain_.idx_ = a_value >> 4;
    release_.idx_ = a_value & 0b00001111;
    return;
}

void Voice::setControl(b8 a_value)
{
    for(pair<const WaveformType, bool>& wf : waveform_enabled_)
    {
        wf.second =  a_value & (wf.first << 4);
    }

    // TEST, RINGMOD, SYNC are not yet implemented.
    gate_ = a_value & 0b00000001;
    return;
}

i32 Voice::output() const
{
    return output_;
}

void Voice::onSample(u64 a_sampleCtr)
{
    for(pair<const WaveformType, Waveform*>& wf : waveforms_)
    {
        wf.second->onSample();
    }

    switch(adsr_state_)
    {
    case ADSR::OUTSIDE:
        if(gate_ && !gate_prev_)  // GATE hit from complete silence
        {
            attack_.nSamples_ = attackNSamples_[attack_.idx_];
            decay_.nSamples_ = decayReleaseNSamples_[decay_.idx_];
            release_.nSamples_ = decayReleaseNSamples_[release_.idx_];
            sustain_.level_ = sustain_.idx_;

            adsr_state_ = ADSR::ATTACK;
            attack_.begin_nSamples_ = a_sampleCtr + 1;
            attack_.end_nSamples_ = a_sampleCtr + 1 + attack_.nSamples_;
            attack_.m_s_ = scale_
                            / (attack_.end_nSamples_-attack_.begin_nSamples_);
            attack_.offs_nSamples_ = 0;
            break;
        }
        break;

    case ADSR::ATTACK:
    {
        u64 nSamples_rel = a_sampleCtr - attack_.begin_nSamples_
                               + attack_.offs_nSamples_;
        adsr_ampl_s_ = max((i64)0, (i64)attack_.m_s_*(i64)nSamples_rel);
        attack_.peakAmpl_s_ = adsr_ampl_s_;

        // Attack phase end, now decay
        if(a_sampleCtr >= attack_.end_nSamples_ && adsr_ampl_s_ >= scale_)
        {
            sustain_.ampl_s_ = (sustain_.level_*attack_.peakAmpl_s_)/15;

            adsr_state_ = ADSR::DECAY;
            decay_.begin_nSamples_ = a_sampleCtr + 1;
            decay_.end_nSamples_ = a_sampleCtr + 1 + decay_.nSamples_;
            i64 delta_ampl_d_s = sustain_.ampl_s_ - (i64)attack_.peakAmpl_s_;
            i64 delta_nSamples_d_ = decay_.end_nSamples_
                                        - decay_.begin_nSamples_;
            decay_.m_s_ = delta_ampl_d_s / delta_nSamples_d_;
            decay_.b_s_ = attack_.peakAmpl_s_;
            break;
        }
        // GATE released
        if(gate_prev_ && !gate_)
        {
            adsr_state_ = ADSR::RELEASE;
            release_.begin_nSamples_ = a_sampleCtr + 1;
            release_.end_nSamples_  = a_sampleCtr + 1 + release_.nSamples_;
            i64 delta_nSamples_r_ = release_.end_nSamples_
                                        - release_.begin_nSamples_;
            i64 delta_ampl_r_s = attack_.peakAmpl_s_;
            release_.m_s_ = -delta_ampl_r_s / delta_nSamples_r_;
            release_.b_s_ = attack_.peakAmpl_s_;
            break;
        }
    }
    break;

    case ADSR::DECAY:
    {
        u64 nSamples_rel = a_sampleCtr - decay_.begin_nSamples_;
        adsr_ampl_s_ = max((i64)0,
                           (i64)decay_.m_s_*(i64)nSamples_rel
                               + (i64)decay_.b_s_);
        decay_.lastAmpl_s_ = adsr_ampl_s_;

        // Decay phase end, now sustain (hold)
        if(a_sampleCtr >= decay_.end_nSamples_
        && adsr_ampl_s_ <= sustain_.ampl_s_)
        {
            adsr_state_ = ADSR::SUSTAIN;
            break;
        }
        // GATE released
        if(gate_prev_ && !gate_)
        {
            adsr_state_ = ADSR::RELEASE;
            release_.begin_nSamples_ = a_sampleCtr + 1;
            release_.end_nSamples_  = a_sampleCtr + 1 + release_.nSamples_;
            i64 delta_nSamples_r_ = release_.end_nSamples_
                                        - release_.begin_nSamples_;
            i64 delta_ampl_r_s = decay_.lastAmpl_s_;
            release_.m_s_ = -delta_ampl_r_s / delta_nSamples_r_;
            release_.b_s_ = decay_.lastAmpl_s_;
            break;
        }
    }
    break;

    case ADSR::SUSTAIN:
    {
        // GATE released, sustain phase end
        if(gate_prev_ && !gate_)
        {
            adsr_state_ = ADSR::RELEASE;
            release_.begin_nSamples_ = a_sampleCtr + 1;
            release_.end_nSamples_  = a_sampleCtr + 1 + release_.nSamples_;
            i64 delta_nSamples_r_ = release_.end_nSamples_
                                        -release_.begin_nSamples_;
            i64 delta_ampl_r_s = decay_.lastAmpl_s_;
            release_.m_s_ = -delta_ampl_r_s / delta_nSamples_r_;
            release_.b_s_ = decay_.lastAmpl_s_;
            break;
        }
    }
    break;

    case ADSR::RELEASE:
    {
        u64 nSamples_rel = a_sampleCtr - release_.begin_nSamples_;
        adsr_ampl_s_ = max((i64)0,
                           (i64)release_.m_s_*(i64)nSamples_rel
                               + (i64)release_.b_s_);
        release_.lastAmpl_s_ = adsr_ampl_s_;

        // Done, silence
        if(a_sampleCtr >= release_.end_nSamples_ && adsr_ampl_s_ == 0)
        {
            adsr_state_ = ADSR::OUTSIDE;
            adsr_ampl_s_ = 0;
            break;
        }
        // GATE hit from unfinished release phase
        if(!gate_prev_ && gate_)
        {
            attack_.nSamples_ = attackNSamples_[attack_.idx_];
            decay_.nSamples_ = decayReleaseNSamples_[decay_.idx_];
            release_.nSamples_ = decayReleaseNSamples_[release_.idx_];
            sustain_.level_ = sustain_.idx_;

            adsr_state_ = ADSR::ATTACK;
            attack_.begin_nSamples_ = a_sampleCtr + 1;
            attack_.m_s_ = scale_ / attack_.nSamples_;
            u64 delta_ampl_s = max((i64)0, scale_ - (i64)release_.lastAmpl_s_);
            attack_.end_nSamples_ = attack_.begin_nSamples_
                                        + (attack_.nSamples_*delta_ampl_s)
                                    /scale_;
            attack_.offs_nSamples_ = (attack_.nSamples_ * release_.lastAmpl_s_)
                                         / scale_;
            break;
        }
    }
    break;

    default:
        break;
    }

    u64 sum = 0;
    u16 nWfEnabled = 0;
    for(pair<const WaveformType, bool>& wf : waveform_enabled_)
    {
        int ena = (wf.second == true);
        nWfEnabled += ena;
        sum += (waveforms_[wf.first]->value())*ena;
    }

    output_ = 0.0;
    gate_prev_ = gate_;

    if(nWfEnabled == 0)
    {
        return;
    }

    u16 rawValue = sum / nWfEnabled;
    i32 r = (i32)(rawValue) - 32768;
    i64 radsr_scale = r*(i64)adsr_ampl_s_;
    output_ = radsr_scale / scale_;

    return;
}

} // namespace sid_types
