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

namespace tests
{
    void instantiateSID()
    {
        SID sid;

        return;
    }

    void playNote(){
    	// initiate SID
    	SID sid;
    	std::vector <float> audio;
    	// set NRES pin to High
    	sid.writePin(SID::NRES, HIGH);
    	// set volume to max
    	execute(&sid, Command(SID::MODE_VOL, 0b00001111));
    	// set freq to 440 Hz (0x1CD6)
    	execute(&sid, Command(SID::V_FREQ_LO.at(V1), 0xD6));
    	execute(&sid, Command(SID::V_FREQ_HI.at(V1), 0x1C));
    	// set attack and decay
    	execute(&sid, Command(SID::V_ATTACK_DECAY.at(V1), (10 << 4) | 8));
    	// set sustain and release
    	execute(&sid, Command(SID::V_SUSTAIN_RELEASE.at(V1), (8 << 4) | 10));
    	// triangle & set gate bit high
    	execute(&sid, Command(SID::V_CONTROL.at(V1), 0x11));

    	for(int i = 0; i < 1 * SAMPLERATE_HZ; i++){
    		audio.push_back(sid,onCLK());
    	}


    }

}  // namespace tests
