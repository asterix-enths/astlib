///
/// \package astlib
/// \file BitsDescription.h
///
/// \author Marian Krivos <nezmar@tutok.sk>
/// \date 2Feb.,2017 
///
/// (C) Copyright 2017 R-SYS s.r.o
/// All rights reserved.
///

#pragma once

#include "astlib/GeneratedTypes.h"
#include "astlib/AsterixItemCode.h"
#include <map>
#include <limits>

namespace astlib
{

class ASTLIB_API BitsDescription
{
public:
    using ValueMap = std::map<std::string, int>;

    BitsDescription() {}
    BitsDescription(AsterixItemCode code) : code(code) {}

    Encoding encoding = Encoding::Unsigned;
    std::string name;
    std::string description;
    ValueMap values;
    AsterixItemCode code;
    int bit = -1;
    int from = -1;
    int to = -1;
    int presence = 0;
    bool fx = false;
    bool repeat = false;
    double scale = 1.0;
    double min = -100000000000;
    double max = 100000000000;
    Units units = Units::None;

    void addEnumeration(const std::string& key, int value);

    int effectiveBitsWidth() const;
    Poco::UInt64 bitMask() const;

    std::string toString() const;

	BitsDescription& operator =(const BitsDescription&);
};

using BitsDescriptionArray = std::vector<BitsDescription>;

} /* namespace astlib */

