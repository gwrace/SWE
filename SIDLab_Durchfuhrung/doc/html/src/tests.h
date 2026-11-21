/*! \file
 * \brief     Testing functions
 * \author
 * \date
 * \copyright h_da fbeit
 */

#ifndef TESTS_H_
#define TESTS_H_

#include "sid/SID.h"


//! Contains all the testing functions
namespace tests
{
    /*!
     * Es werden Konstruktor und Destruktor des SID aufgerufen.
     * + Getestete Anforderungen: ...
     * + Erwartetes Verhalten: ...
     */
    void instantiateSID();

    /*!
     * Testet Command und execute(): Voice 1 uspielt einen Dreieckston mit
     * 440 Hz und ASDR Hüllkurve, das Audiosignal wird als .wav gespeichert.
     */
    void playNote();

}  // namespace tests



#endif /* TESTS_H_ */
