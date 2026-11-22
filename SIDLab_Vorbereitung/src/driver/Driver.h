/*! \file
 * \brief     SID driver
 */

#ifndef DRIVER_DRIVER_H_
#define DRIVER_DRIVER_H_

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <string>

#include "../sid/SID.h"

#include "Command.h"

namespace sid_driver
{
    //! Treiber zur vereinfachten Ansteuerung eines SID
    class Driver
    {

    private:
        sid_types::SID* sid_;  /*!< Der angesteuerte SID */

        std::vector<float> audio_;   /*!< Vom SID erzeugtes Audiosignal */
        std::vector<Command> commands_;  /*!< FIFO-Buffer für noch auszuführende Befehle */

        //! Führt einen Befehl auf dem SID aus
        /*! Das dabei erzeugte Audiosample wird in audio_ gespeichert.
    	 *  \param a_cmd Der auszuführende Befehl
    	 */
        void execute(const Command& a_cmd);

        std::map<sid_types::Address, sid_types::b8> sidData_;  /*!< Aktueller Inhalt jedes SID-Registers, nach Adresse */

    public:

        //! Konstruktor
        /*!
         * \param a_sid Pointer auf den SID, der angesteuert werden soll
         */
        Driver(sid_types::SID* a_sid);

        //! Gibt das bisher aufgezeichnete Audiosignal zurück
        /*! \return Das Audiosignal als Samples im Bereich -1.0 bis +1.0 */
        const std::vector<float>& getAudio() const;

        //! Gibt den Inhalt aller SID-Register tabellarisch auf std::cout aus
        /*! Pro Register: Name, Adresse und Inhalt (bitweise). */
        void printSID() const;

        //! Setzt Driver und SID in den Anfangszustand zurück
        /*! Leert Audiosignal und Befehlsbuffer, setzt sidData_ auf 0 und
         *  führt einen Hardware-Reset des SID aus (NRES für 10 Takte LOW).
         *  Die dabei erzeugten Audiosamples werden in audio_ gespeichert.
         */
        void reset();

    };
}  // namespace sid_driver

#endif /* DRIVER_DRIVER_H_ */
