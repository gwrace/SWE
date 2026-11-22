/*! \file
 * \brief     Testing functions implementation
 * \author
 * \date
 * \copyright h_da fbeit
 */

#include "tests.h"
#include "wav/Wav.h"
#include "driver/Command.h"

#include <vector>



using namespace sid_types;
using namespace sid_driver;
using namespace wav_files;

namespace tests
{
    void instantiateSID()
    {
        SID sid;

        return;
    }

/*    void playNote()
    {
    	// initiate SID
    	SID sid;
    	std::vector<float> audio;

    	// set NRES pin to High
    	sid.writePin(SID::NRES, HIGH);

    	// set volume to max
    	execute(&sid, Command(SID::MODE_VOL, 0b00001111));

    	// set freq to 440 Hz (0x1CD6)
    	execute(&sid, Command(SID::V_FREQ_LO.at(V1), 0b11010110));
    	execute(&sid, Command(SID::V_FREQ_HI.at(V1), 0b00011100));

    	// set attack auf 500ms und decay auf 300 ms
    	execute(&sid, Command(SID::V_ATTACK_DECAY.at(V1), (10 << 4) | 8));

    	// set sustain 50% and release 1,5s
    	execute(&sid, Command(SID::V_SUSTAIN_RELEASE.at(V1), (8 << 4) | 10));

    	// Voice 1 auf Dreieck und GATE-Bi auf HIGH
    	execute(&sid, Command(SID::V_CONTROL.at(V1), 0x11));

    	// 1 Sekunde laufen lassen
    	for(int i=0; i < 1*SAMPLERATE_HZ; i++)
    	{
    		audio.push_back(sid.onCLK());
    	}

    	// GATE aus, Dreieck bleibt an
    	execute(&sid, Command(SID::V_CONTROL.at(V1), 0x10));

    	// 2 Sekunden laufen lassen
    	for(int i = 0; i < 2*SAMPLERATE_HZ; i++)
    	{
    		audio.push_back(sid.onCLK());
    	}

    	// Wave Datei speichern und anzeigen
    	Wav wav(audio);
    	wav.saveAs("commandExecute.wav");
    	wav.view();

    	return;

    } // playNote test
*/

}  // namespace tests

 // namespace tests
