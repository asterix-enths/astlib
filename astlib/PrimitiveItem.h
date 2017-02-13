///
/// \package astlib
/// \file PrimitiveItem.h
///
/// \author Marian Krivos <marian.krivos@rsys.sk>
/// \date 13Feb.,2017 
/// \brief definicia typu
///
/// (C) Copyright 2017 R-SYS s.r.o
/// All rights reserved.
///

#pragma once

#include "GeneratedTypes.h"

namespace astlib
{

class PrimitiveItem
{
public:
    PrimitiveItem();
    PrimitiveItem(const std::string& name, PrimitiveType type);
    virtual ~PrimitiveItem();

    const std::string& getName() const;
    PrimitiveType getType() const;

private:
    std::string _name;
    PrimitiveType _type = PrimitiveType::Unknown;
};

} /* namespace astlib */
