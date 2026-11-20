/*! \file
 * \brief     SID types implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "sid_types.h"

#include <bitset>


namespace sid_types
{

using std::string;
using std::bitset;

string to_string(b8 a_b8)
{
    return bitset<8>(a_b8).to_string();
}

string to_string(b16 a_b16)
{
    return to_string(static_cast<b8>((a_b16 & 0xFF00)>>8))
            + " "
            + to_string(static_cast<b8>(a_b16 & 0x00FF));
}

VoiceIdx& operator++(VoiceIdx& a_orig)
{
    a_orig = static_cast<VoiceIdx>(a_orig + 1);
    return a_orig;
}

VoiceIdx operator++(VoiceIdx& a_orig, int)
{
    VoiceIdx rVal = a_orig;
    ++a_orig;
    return rVal;
}

}  // namespace sid_types
