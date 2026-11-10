/*! \file
 * \brief     SID filters
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#ifndef FILTER_H_
#define FILTER_H_


#include <array>

#include "sid_types.h"


namespace sid_types
{

//! SID filter
/*!
 *  Implements either a lowpass, bandpass or a highpass filter
 */
class Filter
{
public:
    //! Constructor
    /*!
     * \param a_type The filter type to configure the filter for
     */
    Filter(FilterType a_type);

    //! Resets this filter (cutoff freq. 1000Hz, zero out state)
    void reset();

    //! Set the cutoff frequency of this filter
    /*!
     * \param a_freq Cutoff frequency, in Hz
     */
    void setCutoffFreq(float a_freq);

    //! Filter the given sample and return the filtered result
    /*!
     * Call this once on each sample (in this implementation,
     * that is equal to each clock cycle).
     * \param a_sample The input sample to filter
     * \return The filtered result
     */
    i32 sample(i32 a_sample);

private:
    FilterType type_;  /*!< The current filter type */

    //! Calculate and set the current filter coefficients
    void calcFilterCoefficients();

    //! Calculate and set the current lowpass filter coefficients
    void calcLPCoefficients();

    //! Calculate and set the current highpass filter coefficients
    void calcHPCoefficients();

    //! Calculate and set the current bandpass filter coefficients
    void calcBPCoefficients();

    u16 fCO_;  /*!< The current cutoff frequency, in Hz */

    static const u16 NTAPS = 51;  /*!< Number of filter coefficients
                                       ( = number of samples remembered) */

    std::array<i32, NTAPS> sr_;  /*!< Past samples */
    std::size_t sr0_idx_;  /*!< Array index of
                                the most recent sample in sr_ */

    std::array<i64, NTAPS> taps_k_;  /*!< Filter coefficients, scaled */

};

}  // namespace sid_types


#endif /* FILTER_H_ */
