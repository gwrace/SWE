/*! \file
 * \brief     SID filter banks
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#ifndef FILTERBANK_H_
#define FILTERBANK_H_


#include "Filter.h"
#include "sid_types.h"

#include <array>


namespace sid_types
{

//! A series connection of filters.
/*!
 *  There are 8 possible "routes" a voice's signal may take through the
 *  three SID filter types:
 *  - No filtering at all
 *  - Lowpass only
 *  - Bandpass only
 *  - Highpass only
 *  - Lowpass, then bandpass
 *  - Lowpass, then highpass
 *  - Bandpass, then highpass
 *  - Lowpass, then bandpass, then highpass
 *
 *  This implements all 8 routes, making it possible to switch between
 *  them "on the fly"
 */
class FilterBank
{
public:
    //! Constructor
    FilterBank();

    //! Don't copy
    /*!
     * \param a_other Another FilterBank
     */
    FilterBank(const FilterBank& a_other) = delete;
    //! Don't assign
    /*!
     * \param a_rhs Right-hand side
     * \return The same FilterBank, after having been assigned
     */
    FilterBank& operator=(const FilterBank& a_rhs) = delete;

    //! Reset all filter members
    void reset();

    //! Filter the given sample and return the filtered result
    /*!
     * Call this once on each sample (in this implementation,
     * that is equal to each cycle).
     * \param a_sample The input sample to filter
     * \param a_lp Apply lowpass-filtering
     * \param a_bp Apply bandpass-filtering
     * \param a_hp Apply highpass-filtering
     * \return The filtered result
     */
    i32 sample(i32 a_sample, bool a_lp, bool a_bp, bool a_hp);

    //! Set the cutoff frequency of this filter
    /*!
     * Calls Filter::setCutoffFreq() on all filter members
     * \param a_freq Cutoff frequency, in Hz
     */
    void setCutoffFreq(float a_freq);

private:
    Filter lp_;  /*!< Lowpass */
    Filter bp_;  /*!< Bandpass */
    Filter hp_;  /*!< Highpass */
    Filter bpLp_;  /*!< Bandpass for lowpass output */
    Filter hpLp_;   /*!< Highpass for lowpass output */
    Filter hpBp_;   /*!< Highpass for bandpass output */
    Filter hpBpLp_; /*!< Highpass for bandpass for lowpass output */

    std::array<Filter*, 7>
    filters_;   /*!< Pointers to the filter members,
                     to enable iteration */

};

} // namespace sid_types


#endif /* FILTERBANK_H_ */
