///
/// \package astlib
/// \file CodecPolicy.h
///
/// \author Marian Krivos <nezmar@tutok.sk>
/// \date 24Feb.,2017 
///
/// (C) Copyright 2017 R-SYS s.r.o
/// All rights reserved.
///

#pragma once

#include "Astlib.h"

namespace astlib
{

/**
 * Define policies for code/encode operations.
 */
struct ASTLIB_API CodecPolicy
{
    enum Code {
        Ignore,   ///< ignore error
        Warning,
        Error     ///< throw exception
    };

    CodecPolicy() {}
    CodecPolicy(Code mandatoryItems, bool verbose);

    Code mandatoryItems = Error; ///< what to do when mandatory item is not present
    bool normalizeValues = true;
    bool verbose = false;
};

} /* namespace astlib */

