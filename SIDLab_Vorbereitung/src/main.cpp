/*! \file
 * \brief     Main file, containing main() function
 * \author
 * \date
 * \copyright h_da fbeit
 */

#include "tests.h"

#include <iostream>
using std::cout;
using std::endl;


//! main() function, where program execution starts.
/*!
 * \return 0 on success
 */
int main()
{
    cout << "SIDLab: Enter main()." << endl;

    tests::instantiateSID();

    // tests::playNote();

    cout << "SIDLab: Exit main() (success)." << endl;
    return 0;
}

