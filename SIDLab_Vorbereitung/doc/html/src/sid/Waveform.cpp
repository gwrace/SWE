/*! \file
 * \brief     SID waveforms implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "Waveform.h"


namespace sid_types
{

using std::array;
using std::size_t;

bool Triangle::tableInited_ = false;
bool Sawtooth::tableInited_ = false;

array<u16, SAMPLERATE_HZ> Triangle::table_;
array<u16, SAMPLERATE_HZ> Sawtooth::table_;


Waveform::Waveform()
{
    value_ = 0;
    freq_ = 1;
}

Waveform::~Waveform()
{
    // nothing
}

u16 Waveform::value() const
{
    return value_;
}

void Waveform::setFrequency(u16 a_freq)
{
    freq_ = a_freq%SAMPLERATE_HZ;
    if(freq_ == 0)
        freq_ = 1;
    return;
}

Triangle::Triangle()
{
    if(!tableInited_)
    {
        for(size_t i=0; i<SAMPLERATE_HZ/2; i++)
        {
            table_[i] = UINT16_MAX - (UINT16_MAX*i)/(SAMPLERATE_HZ/2);
        }
        for(size_t i=SAMPLERATE_HZ/2; i<SAMPLERATE_HZ; i++)
        {
            table_[i] = (UINT16_MAX*i)/(SAMPLERATE_HZ/2) - UINT16_MAX;
        }

        tableInited_ = true;
    }
    idx_ = 0;
}

void Triangle::reset()
{
    idx_ = 0;
    return;
}

void Triangle::onSample()
{
    idx_ += freq_;
    idx_ %= SAMPLERATE_HZ;
    value_ = table_[idx_];
    return;
}

Sawtooth::Sawtooth()
{
    if(!tableInited_)
    {
        for(size_t i=0; i<SAMPLERATE_HZ; i++)
        {
            table_[i] = UINT16_MAX - (UINT16_MAX*i)/(SAMPLERATE_HZ);
        }

        tableInited_ = true;
    }
    idx_ = 0;
}

void Sawtooth::reset()
{
    idx_ = 0;
    return;
}

void Sawtooth::onSample()
{
    idx_ += freq_;
    idx_ %= SAMPLERATE_HZ;
    value_ = table_[idx_];
    return;
}

Pulse::Pulse()
{
    idx_ = 0;
    pulseWidth_ = 2048;
}

void Pulse::reset()
{
    idx_ = 0;
    pulseWidth_ = 2048;
}

void Pulse::setPulseWidth(u16 a_pw)
{
    pulseWidth_ = a_pw%4096;
    return;
}

void Pulse::onSample()
{
    idx_ += freq_;
    idx_ %= SAMPLERATE_HZ;
    value_ = UINT16_MAX*(idx_ < (SAMPLERATE_HZ*pulseWidth_)/4096);
    return;
}

Noise::Noise()
{
    value_ = 1;
    nCallsSinceLastChange_ = 0;
}

void Noise::reset()
{
    value_ = 1;
    nCallsSinceLastChange_ = 0;
}

void Noise::onSample()
{
    nCallsSinceLastChange_++;
    u16 nCallsForNextChange = SAMPLERATE_HZ/freq_;
    if(nCallsSinceLastChange_ >= nCallsForNextChange)
    {
        // 16-bit xorshift prng
        value_ ^= value_ << 7;
        value_ ^= value_ >> 9;
        value_ ^= value_ << 8;
        nCallsSinceLastChange_ = 0;
    }

    return;
}

}  // namespace sid_types
