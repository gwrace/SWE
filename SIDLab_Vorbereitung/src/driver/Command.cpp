/*! \file
 * \brief     SID driver command implementation
 * \author    stketanuw
 * \date      19.11.2026
 */


#include "Command.h"


namespace sid_driver
{

Command::Command(sid_types::Address a_address, sid_types::b8 a_data, sid_types::b8 a_mask):
        address_(std::min<int>(a_address, sid_types::SID::MODE_VOL)),
        data_(a_data),
        mask_(a_mask)
{
}


}

