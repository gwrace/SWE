/*! \file
 * \brief     SID simulation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#ifndef SID_H_
#define SID_H_


#include <bitset>
#include <map>
#include <functional>
#include <array>
#include <chrono>

// #define REAL_SID_ATTACHED  // Uncomment to play back on the RealSID board

#include "Voice.h"
#include "FilterBank.h"
#include "sid_types.h"


namespace sid_types
{

const u16 PHI2 = SAMPLERATE_HZ;  /*!< SID clock frequency - MUST equal
                                      the sampling rate of 44.1 kHz in this
                                      implementation */

//! SID chip simulation
/*!
 * \image html sid_pins.png
 * Simulates the behavior of the Commodore C64's sound chip,
 * the MOS 6581 Sound Interface Device (SID), released in 1982.
 *
 * This simulation is *INCOMPLETE* and *NOT cycle-accurate*.
 * Not implemented:
 * - Accurate timing with a PHI2 clock of 1.0MHz - instead,
 *   **the clock frequency PHI2 equals the sampling rate of 44.1kHz**.
 *   Cycle-accurracy has been dropped in favour of simulation speed
 * - External input
 * - Read-only registers (Pin RW is always held low)
 * - Filter resonant frequency adjustment (see RES/FILT register)
 * - Ring modulation, synchronization, TEST bit
 * - ...
 *
 * Usage:
 * - Instantiate a SID.
 * - Periodically call onCLK() - once every clock cycle. Store the return
 *   value in some container for later playback.
 * - Use writePin() to write values to the SID's pins.
 */
class SID
{
public:
    //! Constructor
    SID();

    //! Destructor
    ~SID();

    /*! \brief Generates a negative edge on the PHI2 pin (clock signal),
     * making the SID proceed its work for one cycle. Then returns the
     * resulting audio output value on pin AUDIO_OUT
     * \details This will make the SID proceed with its work for one clock
     * cycle. Call this periodically to achieve a time based simulation.
     * Note that PHI2 = 44.1kHz in this implementation for speed.
     * \return The resulting audio output value on pin AUDIO_OUT, ranging
     *         from -1.0 to +1.0.
     */
    [[nodiscard]] float onCLK();

    //! Writes the given logic level to the given SID pin
    /*!
     * \param a_pinNumber The Pin number to write to - see the static
     *        PinNumber member constants.
     * \param a_value The logic level to write (LOW or HIGH).
     */
    void writePin(PinNumber a_pinNumber, LogicLevel a_value);

    static const PinNumber
        NRES;   /*!< Reset pin number */
    static const PinNumber
        NCS;    /*!< Chip select pin number */
    static const std::array<PinNumber, 5>
        A;      /*!< Address bit pin numbers */
    static const std::array<PinNumber, 8>
        D;      /*!< Data bit pin numbers */

    static const Address
        FC_LO;      /*!< FC_LO register address */
    static const Address
        FC_HI;      /*!< FC_HI register address */
    static const Address
        RES_FILT;   /*!< RES_FILT registers address */
    static const Address
        MODE_VOL;   /*!< MODE_VOL registers address */
    static const std::map<VoiceIdx, Address>
        V_FREQ_LO;          /*!< FREQ_LO register addresses, by voice */
    static const std::map<VoiceIdx, Address>
        V_FREQ_HI;          /*!< FREQ_HI register addresses, by voice */
    static const std::map<VoiceIdx, Address>
        V_PW_LO;            /*!< PW_LO register addresses, by voice */
    static const std::map<VoiceIdx, Address>
        V_PW_HI;            /*!< PW_HI register addresses, by voice */
    static const std::map<VoiceIdx, Address>
        V_ATTACK_DECAY;     /*!< ATTACK_DECAY reg. addresses, by voice */
    static const std::map<VoiceIdx, Address>
        V_SUSTAIN_RELEASE;  /*!< SUSTAIN_RELEASE reg. addresses, by voice */
    static const std::map<VoiceIdx, Address>
        V_CONTROL;          /*!< CONTROL register addresses, by voice */

private:
    std::bitset<28> pins_;  /*!< The pins. Pins 14 (GND) 25 (VCC), 26
                                 (EXT_IN) and 28 (VDD) are unused, pin 6
                                 (PHI2), pin 27 (AUDIO_OUT): see onCLK() */

    std::chrono::steady_clock::time_point
        startTime_;  /*!< SID instantiation time for exec. speed measurement */
    u64 sampleCtr_;  /*!< Counter for the current sample (time base),
                          incremented on every call to onCLK() */

    std::map<VoiceIdx, FilterBank> filterBanks_;  /*!< Filter banks,
                                                       by voice index */
    std::map<VoiceIdx, Voice> voices_;  /*!< Voices, by voice index */

    //! Resets all members to their initial values.
    /*!
     * \param a_verbose Output call to stdout
     */
    void reset(bool a_verbose);

    //! Callback to execute if pin NRES is low.
    void onNRES();

    float volume_;  /*!< The current volume level in a range from 0.0 (muted)
                         to +1.0 (full volume) */
    b8 filterCutoffFreqLo_;  /*!< Filter cutoff frequency low-byte */
    b8 filterCutoffFreqHi_;  /*!< Filter cutoff frequency high-byte */
    std::map<VoiceIdx, bool>
        voicePassFilter_;  /*!< Voice pass filter enabled, by voice index */
    std::map<FilterType, bool>
        filterEnabled_;    /*!< Filter enabled, by type */

    std::map<Address, std::function<void(b8)>>
        voiceSetFcts_;  /*!< Voice parameter set functions,
                             by register address */

    #if defined(REAL_SID_ATTACHED)
        class RealSID
        {
        public:
            RealSID();
            bool connected() const;

            struct Package
            {
                Package(u64 a_sampleCtr, b8 a_addrbyte, b8 a_databyte) :
                    sampleCtr_(a_sampleCtr),
                    addrbyte_(a_addrbyte),
                    databyte_(a_databyte)
                {
                    // empty
                }
                u64 sampleCtr_;
                b8 addrbyte_;
                b8 databyte_;
            };

            void send(Package a_pkg);
            void play();

        private:
            void readEcho();
            int fd_;
            bool ready_;

            int nCommands_;

        } realSID_;
    #endif  // defined(REAL_SID_ATTACHED)
};

}  // namespace sid_types


#endif /* SID_H_ */
