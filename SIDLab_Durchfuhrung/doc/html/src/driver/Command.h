/*! \file
 * \brief     SID driver command
 * \author    stketanuw
 * \date      19.11.2026
 */

#ifndef DRIVER_COMMAND_H_
#define DRIVER_COMMAND_H_

#include "../sid/sid_types.h"
#include "../sid/SID.h"
#include <algorithm>



//! Treiber zur vereinfachten Ansteuerung des SID
namespace sid_driver
{
	//! Ein Schreibbefehl an den SID, die die Registeradresse und das Datenbyte zusammenfasst.
	struct Command
	{

		const sid_types::Address address_; /*!< Registeradresse (5 bits) höchstens SID::MODE_VOL */
		const sid_types::b8 data_; /*!< Datenbyte, das ins Register geschrieben wird */

		//! Konstruktor
		/*! Adressen größer als MODE_VOL werden auf MODE_VOL begrenzt
		 * \param a_address Die Registeradresse
		 *  \param a_data Das Datenbyte
		 */
		Command(sid_types::Address a_address, sid_types::b8 a_data);

	};

	//! Execute : Ausführen einen Befehl auf dem übergebenen SID
	/*! \param a_sid Pointer auf SID, der den Befehl ausführen soll
	 * \param a_cmd Der auszuführende Befehl
	 * \return Der während des Taktzyklus erzeugte Wert des Audiosignals am Pin AUDIO_OUT, in dem Bereich -1.0 bis +1.0
	 */
	float execute(sid_types::SID* a_sid, Command a_cmd);
}



#endif /* DRIVER_COMMAND_H_ */
