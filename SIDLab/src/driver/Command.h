/*
 * Command.h
 *
 *  Created on: 19 Nov 2026
 *      Author: graceyg209
 */

#ifndef DRIVER_COMMAND_H_
#define DRIVER_COMMAND_H_

#include "sid/sid_types.h"
#include "sid/SID.h"
#include <algorithm>

namespace sid_driver {

struct Command {
    Command(sid_types::Address a_address, sid_types::b8 a_data);

    const sid_types::Address address_;
    const sid_types::b8      data_;
};

float execute(sid_types::SID* a_sid, Command a_cmd);
}
#endif /* DRIVER_COMMAND_H_ */
