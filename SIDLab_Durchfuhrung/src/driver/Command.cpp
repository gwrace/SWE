/*! \file
 * \brief     SID driver command implementation
 * \author    stketanuw
 * \date      19.11.2026
 */


#include "Command.h"


namespace sid_driver
{

Command::Command(sid_types::Address a_address, sid_types::b8 a_data):
		address_(std::min<int>(a_address, sid_types::SID::MODE_VOL)),
		data_(a_data)
		{

		}


float execute(sid_types::SID* a_sid, Command a_cmd)
{
	// set address pins
	for(int i = 0; i < 5; i++){
		a_sid->writePin(sid_types::SID::A[i], (a_cmd.address_ >> i) & 1);
	}
	// set data pins
	for(int i = 0; i < 8; i++){
		a_sid->writePin(sid_types::SID::D[i], (a_cmd.data_ >> i) & 1);
	}
	// set CS low
	a_sid->writePin(sid_types::SID::NCS, LOW);

	// negative flank of the clock
	float audioSignal = a_sid->onCLK();

	// set CS high
	a_sid->writePin(sid_types::SID::NCS, HIGH);

	return audioSignal;
}
}

