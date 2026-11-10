/*! \file
 * \brief     SID voices
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include <array>
#include <map>

#include "Waveform.h"
#include "sid_types.h"


namespace sid_types
{

//! SID voice
/*!
 *  Implements the behavior of a single SID voice.
 *  The voice drives its four different waveforms (pulses, triangle,
 *  sawtooth, noise) and produces an audio mix of these, according to
 *  which are enabled.
 *  Applies an ADSR envelope (attack, decay, sustain, release) to the
 *  mix volume (see datasheet).
 *  SID must call onSample() and fetch the output() once each sample
 *  (for this implementation, that equals once each clock cycle)
 */
class Voice
{
public:
    //! Constructor
    Voice();

    //! Don't copy
    /*!
     * \param a_other Another Voice
     */
    Voice(const Voice& a_other) = delete;
    //! Don't assign
    /*!
     * \param a_rhs Right-hand side
     * \return The same Voice, after having been assigned
     */
    Voice& operator=(const Voice& a_rhs) = delete;

    //! Lets this voice proceed for the duration of one sample
    /*!
     * SID must call this once each sample (in this implementation, that
     * is equal to each cycle).
     * Makes its waveforms proceed for the duration of one sample,
     * calculates the current ADSR envelope volume and produces the
     * current audio mix (which should afterwards be fetched using
     * output())
     * \param a_sampleCtr The current sample counter
     */
    void onSample(u64 a_sampleCtr);

    //! Returns this voice's current audio output value
    /*!
     * Call this to fetch the current audio mix output after onSample().
     * \return The current audio output value, ranging from -32768
     *         (INT16_MIN) to 32767 (INT16_MAX)
     */
    i32 output() const;

    //! Resets this voice to its initial values (zero / off / disabled).
    void reset();

    //! Sets this voice's control register data byte
    /*!
     * Its value enables or disables waveforms and gates or ungates the
     * voice, triggering the ADSR envelope
     * \param a_value The control register data byte
     */
    void setControl(b8 a_value);

    //! Sets this voice's FREQ_LO register data byte
    /*!
     * On change, the frequency of the voice is adapted.
     * \param a_value The FREQ_LO register data byte
     */

    void setFreqLo(b8 a_value);

    //! Sets this voice's FREQ_HI register data byte
    /*!
     * On change, the frequency of the voice is adapted.
     * \param a_value The FREQ_HI register data byte
     */
    void setFreqHi(b8 a_value);

    //! Sets this voice's PW_LO register data byte
    /*!
     * * On change, the pulse width of the voice is adapted.
     * \param a_value The PW_LO register data byte
     */
    void setPWLo(b8 a_value);

    //! Sets this voice's PW_HI register data byte
    /*!
     * On change, the pulse width of the voice is adapted.
     * \param a_value The PW_HI register data byte
     */
    void setPWHi(b8 a_value);

    //! Sets this voice's ATTACK_DECAY register data byte
    /*!
     * Note that a change in ADSR parameters is reflected only on the next
     * gate trigger. They are held constant during the currently executed
     * envelope.
     * \param a_value ATTACK_DECAY register data byte
     */
    void setAttackDecay(b8 a_value);

    //! Sets this voice's SUSTAIN_RELEASE register data byte
    /*!
     * Note that a change in ADSR parameters is reflected only on the next
     * gate trigger. They are held constant during the currently executed
     * envelope.
     * \param a_value SUSTAIN_RELEASE register data byte
     */
    void setSustainRelease(b8 a_value);

    static const std::array<u32, 16>
    attackNSamples_;  /*!< Hardcoded table of attack durations
                           (as number of samples) for the possible
                           attack register values - see handbook */
    static const std::array<u32, 16>
    decayReleaseNSamples_;  /*!< Hardcoded table of delay/release
                                 durations (as number of samples)
                                 for the possible decay/release
                                 register values - see handbook */

private:
    i32 output_;  /*!< The current audio output value, ranging from
                       -32768 (INT16_MIN) to 32767 (INT16_MAX) */

    std::map<WaveformType, bool>
    waveform_enabled_;  /*!< The current enabled/disabled state
                             of the four waveforms */

    b16 freq_;  /*!< The current voice frequency hi-byte and lo-byte */
    void applyFreqToAllWaveforms();  /*!< Applies the current voice
                                          frequency to all waveforms */

    b16 pulseWidth_;  /*!< The current voice pulse width
                           hi-byte and lo-byte */

    //! ADSR envelope phase (state)
    enum class ADSR
    {
        OUTSIDE = 0,  /*!< Outside any ADSR envelope (zero volume) */
        ATTACK = 1,   /*!< In attack phase */
        DECAY = 2,    /*!< In decay phase */
        SUSTAIN = 3,  /*!< In sustain phase */
        RELEASE = 4   /*!< In release phase */
    } adsr_state_;    /*!< The current ADSR phase (state) */

    bool gate_;        /*!< Voice gated (true) or ungated (false) */
    bool gate_prev_;   /*!< Gated state in the previous sample, to
                            detect a positive edge on the gate bit */

    //! Parameters needed to calc the ADSR volume during attack phase
    struct Attack
    {
        b8 idx_;                /*!< Attack register data byte (nibble) */
        u64 nSamples_;          /*!< Sample counter, relative to attack
                                     phase begin */
        u64 begin_nSamples_;    /*!< Sample counter value at attack phase
                                     begin */
        u64 offs_nSamples_;     /*!< 0 if attack is triggered from outside
                                     the envelope (that is, silence),
                                     a positive value if triggered from an
                                     unfinished release phase - in this
                                     case, attack starts from a non-zero
                                     volume, which is expressed as an
                                     offset number of samples already
                                     passed at attack begin */
        u64 end_nSamples_;      /*!< Sample counter value at attack end
                                     (if not aborted) */
        u64 m_s_;               /*!< Volume slope, scaled */
        u64 peakAmpl_s_;        /*!< Peak volume reached at attack end,
                                     scaled */

    } attack_;  /*!< Current ADSR envelope attack phase parameters */

    //! Parameters needed to calc the ADSR volume during decay phase
    struct Decay
    {
        b8 idx_;                /*!< Decay register data byte (nibble) */
        u64 nSamples_;          /*!< Sample counter, relative to decay
                                     phase begin */
        u64 begin_nSamples_;    /*!< Sample counter value at decay phase
                                     begin */
        u64 end_nSamples_;      /*!< Sample counter value at decay end
                                     (if not aborted) */
        i64 m_s_;               /*!< Volume slope, scaled */
        u64 b_s_;               /*!< Volume offset, scaled */
        u64 lastAmpl_s_;        /*!< Last volume reached at decay end,
                                     scaled */
    } decay_;  /*!< Current ADSR envelope decay phase parameters */

    //! Parameters needed to calc the ADSR volume during sustain phase
    struct Sustain
    {
        b8 idx_;                /*!< Sustain register data byte (nibble) */
        b8 level_;              /*!< Sustain level */
        u64 ampl_s_;            /*!< Sustain volume, scaled */

    } sustain_;  /*!< Current ADSR envelope sustain phase parameters */

    //! Parameters needed to calc the ADSR volume during release phase
    struct Release
    {
        b8 idx_;                /*!< Release register data byte (nibble) */
        u64 nSamples_;          /*!< Sample counter, relative to release
                                     phase begin */
        u64 begin_nSamples_;    /*!< Sample counter value at release phase
                                     begin */
        u64 end_nSamples_;      /*!< Sample counter value at release end
                                     (if not aborted) */
        i64 m_s_;               /*!< Volume slope, scaled */
        u64 b_s_;               /*!< Volume offset, scaled */
        u64 lastAmpl_s_;        /*!< Last volume reached at release end,
                                     scaled - may be nonzero if another
                                     gate trigger occurs */

    } release_;  /*!< Current ADSR envelope release phase parameters */

    u64 adsr_ampl_s_;  /*!< Current ADSR envelope volume, scaled */

    std::map<WaveformType, Waveform*>
    waveforms_;  /*!< Pointers to the four waveforms, to enable
                      iteration / access by type */
    Triangle triangle_;  /*!< The triangle waveform */
    Sawtooth sawtooth_;  /*!< The sawtooth waveform */
    Pulse pulse_;        /*!< The pulse waveform */
    Noise noise_;        /*!< The noise waveform */

    static const i64 scale_;  /*!< A scaling factor for fixed-point
                                   calculations with amplitude (volume)
                                   values */

};

}  // namespace sid_types
