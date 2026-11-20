/*! \file
 * \brief     SID types
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#ifndef SID_TYPES_H_
#define SID_TYPES_H_

#include <cstdint>
#include <string>


//! Definitions used by the SID chip simulation
namespace sid_types
{

using u8 = uint8_t;    /*!< An 8-bit unsigned int */
using b8 = uint8_t;    /*!< A byte (8 bits) */
using Address = u8;    /*!< A SID address has 5 bits (the upper 3 are unused) */
using PinNumber = u8;  /*!< A SID pin number */
using b16 = uint16_t;  /*!< Two bytes (16 bits) */
using u16 = uint16_t;  /*!< A 16-bit unsigned int */
using u32 = uint32_t;  /*!< A 32-bit unsigned int */
using i32 = int32_t;   /*!< A 32-bit signed int */
using u64 = uint64_t;  /*!< A 64-bit unsigned int */
using i64 = int64_t;   /*!< A 64-bit signed int */

using LogicLevel = bool;  /*!< Pin logic level */
#define LOW false         /*!< Pin logic level "high" */
#define HIGH true         /*!< Pin logic level "low" */

//! Returns the bitwise string representation of a byte
/*!
 * \param a_b8 The byte
 * \return The bitwise string representation
 */
std::string to_string(b8 a_b8);
//! Returns the bitwise string representation of two bytes
/*!
 * \param a_b16 The two bytes
 * \return The bitwise string representation
 */
std::string to_string(b16 a_b16);

//! Voice names (indices)
enum VoiceIdx : u8
{
    V1 = 0,         /*!< Voice 1 */
    V2 = 1,         /*!< Voice 2 */
    V3 = 2,         /*!< Voice 3 */
    N_VOICES = 3    /*!< Number of SID voices */
};

//! ++VoiceIdx
/*! \param a_orig The VoiceIdx to increment
 *  \return a_orig, after having been incremented
 */
VoiceIdx& operator++(VoiceIdx& a_orig);

//! VoiceIdx++
/*! \param a_orig The VoiceIdx to increment
 *  \return A copy of a_orig before before it had been incremented
 */
VoiceIdx operator++(VoiceIdx& a_orig, int);

//! Waveform type
enum WaveformType : u8
{
    NOISE = 8,  /*!< Noise */
    PULSE = 4,  /*!< Pulse */
    SAW = 2,    /*!< Sawtooth */
    TRI = 1     /*!< Triangle */
};

//! Filter type
enum FilterType : u8
{
    LP = 0,     /*!< Lowpass */
    BP = 1,     /*!< Bandpass */
    HP = 2      /*!< Highpass */
};

}  // namespace sid_types


#endif /* SID_TYPES_H_ */
