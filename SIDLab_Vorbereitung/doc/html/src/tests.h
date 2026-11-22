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
     * Testet Command und execute(): Voice 1 spielt einen Dreieckston mit
     * 440 Hz und ASDR Hüllkurve, das Audiosignal wird als .wav gespeichert.
     * + Getestete Anforderungen: A_Driver_003, A_Driver_004, A_Note_001, A_Wav_002
     * + Erwartetes Verhalten:
     * - Datei mit 3s Länge erzeugt und geöffnet
     * - Wellenform Dreieck mit ca 440 Hz
     * - Attack: Amplitude steigt in 0,5 s linear auf das Maximum
     * - Decay: Amplitude fällt in ca. 0,3 s auf das Sustain-Level
     *   (8/15, ca. 53 %)
     * - Sustain: Amplitude bleibt bis t = 1 s konstant
     * - Release: Nach GATE = 0 klingt der Ton in ca. 1,5 s ab,
     *   danach bis t = 3 s Stille
     */
    void playNote();

}  // namespace tests



#endif /* TESTS_H_ */
