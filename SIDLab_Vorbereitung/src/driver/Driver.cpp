/*! \file
 * \brief     SID driver implementation
 */

#include "Driver.h"

using namespace sid_types;

namespace sid_driver
{

	Driver::Driver(sid_types::SID* a_sid) :
        		sid_(a_sid)
	{
		reset();
	}

    void Driver::execute(const Command& a_cmd)
    {
    	// maskierte Bits aus dem Command, Rest aus sidData_
    	sid_types::b8 newByte = (a_cmd.data_ & a_cmd.mask_)
    	                      | (sidData_[a_cmd.address_] & ~a_cmd.mask_);

    	// set address pins
    	for(int i = 0; i < 5; i++)
    	{
    		sid_->writePin(sid_types::SID::A[i], (a_cmd.address_ >> i) & 1);
    	}

    	// set data pins
    	for (int i = 0; i < 8; i++) {
    	    sid_->writePin(sid_types::SID::D[i], (newByte >> i) & 1);
    	}

        // Buchführung aktualisieren
        sidData_[a_cmd.address_] = newByte;

    	// set CS low
    	sid_->writePin(sid_types::SID::NCS, LOW);

    	// negative flank of the clock
    	audio_.push_back(sid_->onCLK());

    	// set CS high
    	sid_->writePin(sid_types::SID::NCS, HIGH);

    }

    const std::vector<float>& Driver::getAudio() const
    {
        return audio_;
    }

    void Driver::printSID() const
    {

        // Registernamen aus den SID-Konstanten zusammenbauen
        std::map<sid_types::Address, std::string> names;
        for (sid_types::VoiceIdx voice = sid_types::V1; voice < sid_types::N_VOICES; ++voice)
        {
            std::string p = "V" + std::to_string(voice + 1) + "_";
            names[SID::V_FREQ_LO.at(voice)]         = p + "FREQ_LO";
            names[SID::V_FREQ_HI.at(voice)]         = p + "FREQ_HI";
            names[SID::V_PW_LO.at(voice)]           = p + "PW_LO";
            names[SID::V_PW_HI.at(voice)]           = p + "PW_HI";
            names[SID::V_CONTROL.at(voice)]         = p + "CONTROL";
            names[SID::V_ATTACK_DECAY.at(voice)]    = p + "ATTACK_DECAY";
            names[SID::V_SUSTAIN_RELEASE.at(voice)] = p + "SUSTAIN_RELEASE";
        }
        names[SID::FC_LO]    = "FC_LO";
        names[SID::FC_HI]    = "FC_HI";
        names[SID::RES_FILT] = "RES_FILT";
        names[SID::MODE_VOL] = "MODE_VOL";

        // Tabellenkopf
        std::cout << std::left
                  << std::setw(20) << "Register"
                  << std::setw(10) << "Adresse"
                  << "Inhalt" << std::endl;

        // eine Zeile pro Register
        for (const auto& [addr, data] : sidData_)
        {
            std::cout << std::left
                      << std::setw(20) << names[addr]
                      << std::setw(10) << static_cast<int>(addr)
                      << sid_types::to_string(data) << std::endl;
        }
    }

    void Driver::reset()
    {
        // Audiosignal und Befehlsbuffer leeren
        audio_.clear();
        commands_.clear();

        // Buchführung: alle Register auf 0
        for (sid_types::Address a = 0; a <= sid_types::SID::MODE_VOL; ++a)
        {
            sidData_[a] = 0;
        }

        // SID-Reset: NRES für mindestens 10 negative Taktflanken auf LOW
        sid_->writePin(sid_types::SID::NRES, LOW);
        for (int i = 0; i < 10; ++i)
        {
            audio_.push_back(sid_->onCLK());
        }
        sid_->writePin(sid_types::SID::NRES, HIGH);
    }

}  // namespace sid_driver
