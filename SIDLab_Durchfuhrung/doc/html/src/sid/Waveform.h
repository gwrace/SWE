/*! \file
 * \brief     SID waveforms
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#ifndef WAVEFORM_H_
#define WAVEFORM_H_


#include "sid_types.h"

#include <array>


namespace sid_types
{

    const u16 SAMPLERATE_HZ = 44100;  /*!< Sampling rate in Hz */

    //! A waveform
    /*!
     *  Can be either a triangle, sawtooth, pulse or noise signal
     */
    class Waveform
    {
    public:
        //! Constructor
        Waveform();

        //! Destructor
        virtual ~Waveform();

        //! Returns the current value of the waveform
        /*!
         * \return The current value, ranging from 0 to UINT16_MAX
         */
        u16 value() const;

        //! Sets the frequency with which the waveform should advance
        /*!
         * \param a_freq The frequency, in Hz
         */
        void setFrequency(u16 a_freq);

        //! Advance the waveform according to the current frequency
        /*!
         * Call this once on each sample (in this implementation,
         * that is equal to each clock cycle).
         */
        virtual void onSample() = 0;

        //! Reset the waveform
        virtual void reset() = 0;

    protected:
        u16 value_;  /*!< The current value,
                          ranging from 0 to UINT16_MAX */
        u16 freq_;  /*!< The current frequency with which the waveform
                         should advance */

    };

    //! Triangle waveform
    /*!
     * \image html triangle.png
     */
    class Triangle : public Waveform
    {
    public:
        //! Constructor, inits the triangle table_ on first call
        Triangle();

        virtual void onSample();
        virtual void reset();

    private:
        static std::array<u16, SAMPLERATE_HZ>
            table_;  /*!< Table of triangle waveform values (one period,
                          SAMPLERATE_HZ values) */
        static bool tableInited_;  /*!< Whether the table_
                                        is already initialised */
        u16 idx_;  /*!< Current table_ index */

    };

    //! Sawtooth waveform
    /*!
     * \image html sawtooth.png
     */
    class Sawtooth : public Waveform
    {
    public:
        //! Constructor, inits the sawtooth table_ on first call
        Sawtooth();

        virtual void onSample();
        virtual void reset();

    private:
        static std::array<u16, SAMPLERATE_HZ>
            table_;   /*!< Table of sawtooth waveform values (one period,
                           SAMPLERATE_HZ values) */
        static bool tableInited_;  /*!< Whether the table_
                                        is already initialised */
        u16 idx_;  /*!< Current table_ index */

    };

    //! Pulse waveform
    /*!
     * \image html pulse.png
     */
    class Pulse : public Waveform
    {
    public:
        //! Constructor
        Pulse();

        //! Sets the pulse width
        /*!
         * \param a_pw The pulse width, ranging from 0 (0%) to 4095 (100%)
         */
        void setPulseWidth(u16 a_pw);

        virtual void onSample();
        virtual void reset();

    private:
        u16 idx_;   /*!< Current index in the SAMPLERATE_HZ number
                         of pulse samples */
        u16 pulseWidth_;  /*!< The pulse width, ranging from 0 (0%)
                               to 4095 (100%) */

    };

    //! Noise waveform, implemented using a 16-bit xorshift
    /*!
     * See e. g. https://en.wikipedia.org/wiki/Xorshift
     * \image html noise.png
     */
    class Noise : public Waveform
    {
    public:
        //! Constructor
        Noise();

        virtual void onSample();
        virtual void reset();

    private:
        u16 nCallsSinceLastChange_;  /*!< Number of samples that passed since
                                          the last change of value */

    };

}  // namespace sid_types


#endif /* WAVEFORM_H_ */
