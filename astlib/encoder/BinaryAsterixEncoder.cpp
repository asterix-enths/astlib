///
/// \package astlib
/// \file BinaryAsterixEncoder.cpp
///
/// \author Marian Krivos <marian.krivos@rsys.sk>
/// \date 20Feb.,2017 
/// \brief definicia typu
///
/// (C) Copyright 2017 R-SYS s.r.o
/// All rights reserved.
///

#include "BinaryAsterixEncoder.h"
#include "ValueEncoder.h"
#include "FspecGenerator.h"
#include "astlib/model/FixedItemDescription.h"
#include "astlib/model/VariableItemDescription.h"
#include <iostream>


namespace astlib
{

BinaryAsterixEncoder::BinaryAsterixEncoder()
{
}

BinaryAsterixEncoder::~BinaryAsterixEncoder()
{
}

size_t BinaryAsterixEncoder::encode(const CodecDescription& codec, ValueEncoder& valueEncoder, std::vector<Byte>& buffer, const std::string& uap)
{
    FspecGenerator fspec;
    Byte aux[MAX_PACKET_SIZE];

    const CodecDescription::UapItems& uapItems = codec.enumerateUapItems();
    size_t encodedSize = encodePayload(codec, valueEncoder, uapItems, fspec, aux);

    size_t len = 1 + 2 + fspec.size() + encodedSize;
    buffer.resize(len);
    buffer[0] = codec.getCategoryDescription().getCategory();
    // TODO: pre littleendian treba zvlast vetvu
    buffer[1] = (len >> 8) & 0xFF;
    buffer[2] = len & 0xFF;
    memcpy(&buffer[3], fspec.data(), fspec.size());
    memcpy(&buffer[3+fspec.size()], aux, encodedSize);

    return len;
}

size_t BinaryAsterixEncoder::encodePayload(const CodecDescription& codec, ValueEncoder& valueEncoder, const CodecDescription::UapItems& uapItems, FspecGenerator& fspec, Byte buffer[])
{
    size_t bufferPosition = 0;

    for(const auto& entry: uapItems)
    {
        if (!entry.second.item)
        {
            fspec.skipItem();
            continue;
        }

        const ItemDescription& item = *entry.second.item;
        size_t len = 0;

        switch(item.getType().toValue())
        {
            case ItemFormat::Fixed:
                len = encodeFixed(item, valueEncoder, uapItems, fspec, buffer + bufferPosition);
                break;
            case ItemFormat::Variable:
                len = encodeVariable(item, valueEncoder, uapItems, fspec, buffer + bufferPosition);
                break;
        }

        if (len > 0)
        {
            bufferPosition += len;
            fspec.addItem();
        }
        else
        {
            fspec.skipItem();
        }
    }

    return bufferPosition;
}

size_t BinaryAsterixEncoder::encodeFixed(const ItemDescription& item, ValueEncoder& valueEncoder, const CodecDescription::UapItems& uapItems, FspecGenerator& fspec, Byte buffer[])
{
    const FixedItemDescription& fixedItem = static_cast<const FixedItemDescription&>(item);
    const Fixed& fixed = fixedItem.getFixed();
    return encodeBitset(item, fixed, valueEncoder, buffer);
}

size_t BinaryAsterixEncoder::encodeVariable(const ItemDescription& item, ValueEncoder& valueEncoder, const CodecDescription::UapItems& uapItems, FspecGenerator& fspec, Byte buffer[])
{
    const VariableItemDescription& varItem = static_cast<const VariableItemDescription&>(item);
    const FixedVector& fixedVector = varItem.getFixedVector();
    auto ptr = buffer;
    int encodedByteCount = 0;

    for(const Fixed& fixed: fixedVector)
    {
        auto len = fixed.length;
        poco_assert(len == 1);
        encodeBitset(item, fixed, valueEncoder, ptr);
        encodedByteCount++;
        ptr++;
    }

    if (encodedByteCount > 1)
    {
        auto aux = ptr-1;
        for(int i = encodedByteCount-1; i > 0; i--)
        {
            if (*aux == 0)
            {
                encodedByteCount--;
            }
            else
            {
                break;
            }
        }
    }

    if (encodedByteCount > 1)
    {
        for(int i = 0; i < encodedByteCount-1; i++)
        {
            ptr[i] |= FX_BIT;
        }
    }

    return encodedByteCount;
}

size_t BinaryAsterixEncoder::encodeBitset(const ItemDescription& item, const Fixed& fixed, ValueEncoder& valueEncoder, Byte buffer[])
{
    const BitsDescriptionArray& bitsDescriptions = fixed.bitsDescriptions;
    Poco::UInt64 data = 0;
    bool encoded = false;

    for (const BitsDescription& bits : bitsDescriptions)
    {
        CodecContext context(item, bits, 0);
        Poco::UInt64 value = 0;

        if (valueEncoder.encode(context, value))
        {
            encoded = true;
            Poco::UInt64 mask = 0;
            int leftShift = 0;

            if (context.width == 1)
            {
                // Send non FX bits only
                if (!bits.fx)
                {
                    mask = 1;
                    leftShift = bits.bit-1;
                }
            }
            else
            {
                mask = bits.bitMask();
                leftShift = bits.to-1;
            }
std::cout << bits.name << " = " << (value&mask) << " size = " << context.width << std::endl;
            data |= ((value & mask) << leftShift);
        }
    }

    if (encoded)
    {
        int len = fixed.length;
        ByteUtils::pokeBigEndian(buffer, data, len);
std::cout << "  " << Poco::NumberFormatter::formatHex(data) << std::endl;
        return len;
    }

    return 0;
}

} /* namespace astlib */
