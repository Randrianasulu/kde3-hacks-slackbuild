//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "sdpattribute.h"
#include <assert.h>
#include <sstream>
#include <iomanip>
#include <kdebug.h>

using namespace std;

namespace KBluetooth
{
namespace SDP
{

uuid_t::uuid_t(QString s) {
    uuid_t();
    fromString(s);
}

uuid_t::uuid_t(uint64_t l, uint64_t h)
{
    this->hi = h;
    this->lo = l;
}

bool uuid_t::fromString(QString s)
{
    // Strip leading "0x"
    if (s.startsWith("0x"))
    {
        s = s.right(s.length()-2);
    }
    // Remove separators
    s = s.replace(":", "");

    bool bOk = false;
    if (s.length() == 4 || s.length() == 8)
    {
        uint32_t u32 = s.toUInt(&bOk, 16);
        setUUID32(u32);
        return bOk;
    }
    else if (s.length() == 32)
    {
#if (QT_VERSION >= 0x030200)
        uint64_t u64hi = s.left(16).toULongLong(&bOk, 16);
#else
        uint64_t u64hi = s.left(16).toULong(&bOk, 16);
#endif
        if (!bOk)
            return false;
#if (QT_VERSION >= 0x030200)
        uint64_t u64lo = s.right(16).toULongLong(&bOk, 16);
#else
        uint64_t u64lo = s.right(16).toULong(&bOk, 16);
#endif
        if (!bOk)
            return false;
        this->hi = u64hi;
        this->lo = u64lo;
        return true;
    }
    else {
        // TODO: parse uuid128 values
        this->hi = 0;
        this->lo = 0;
        return false;
    }
}

void uuid_t::setUUID32(uint32_t v)
{
    this->hi = uint64_t(0x1000) | (uint64_t(v) << 32);
    // see BT 1.1 Core spec p2.7.1
    this->lo = (uint64_t(0x80000080) << 32) | uint64_t(0x5F9B34FB);
}

void uuid_t::setUUID128(uint64_t hi, uint64_t lo)
{
    this->hi = hi;
    this->lo = lo;
}

uuid_t::operator QString() const
{
    QString ret;
    uint32_t v1 = uint32_t(hi >> 32);
    uint32_t v2 = uint32_t(hi & 0xFFFFFFFF);
    uint32_t v3 = uint32_t(lo >> 32);
    uint32_t v4 = uint32_t(lo & 0xFFFFFFFF);
    QString d;
    return d.sprintf("0x%08lx:%08lx:%08lx:%08lx",
                     (unsigned long)v1, (unsigned long)v2,
                     (unsigned long)v3, (unsigned long)v4);
}

uuid_t::operator ::uuid_t() const
{
    //kdDebug() << "uuid_t() -> " << QString(*this) << endl;
    ::uuid_t ret;
    if ((lo == (uint64_t(0x80000080) << 32) | uint64_t(0x5F9B34FB)) &&
        ((hi&0xFFFFFFFF) == 0x1000)) {
        uint32_t uuid32val = uint32_t(hi >> 32);    
        if (uuid32val > 0xFFFF) {
            ret.type = SDP_UUID16;
            ret.value.uuid16 = uint16_t(uuid32val);
        }
        else {
            ret.type = SDP_UUID32;
            ret.value.uuid32 = uuid32val;
        }
    }
    else {
        ret.value.uuid128.data[0] = (lo >> 0) & 0xFF;
        ret.value.uuid128.data[1] = (lo >> 8) & 0xFF;
        ret.value.uuid128.data[2] = (lo >> 16) & 0xFF;
        ret.value.uuid128.data[3] = (lo >> 24) & 0xFF;
        ret.value.uuid128.data[4] = (lo >> 32) & 0xFF;
        ret.value.uuid128.data[5] = (lo >> 40) & 0xFF;
        ret.value.uuid128.data[6] = (lo >> 48) & 0xFF;
        ret.value.uuid128.data[7] = (lo >> 56) & 0xFF;
    
        ret.value.uuid128.data[8] = (hi >> 0) & 0xFF;
        ret.value.uuid128.data[9] = (hi >> 8) & 0xFF;
        ret.value.uuid128.data[10] = (hi >> 16) & 0xFF;
        ret.value.uuid128.data[11] = (hi >> 24) & 0xFF;
        ret.value.uuid128.data[12] = (hi >> 32) & 0xFF;
        ret.value.uuid128.data[13] = (hi >> 40) & 0xFF;
        ret.value.uuid128.data[14] = (hi >> 48) & 0xFF;
        ret.value.uuid128.data[15] = (hi >> 56) & 0xFF;
    
        ret.type = SDP_UUID128;
    }
    return ret;
}

bool uuid_t::operator<(const uuid_t other) const
{
    if (hi != other.hi) return hi < other.hi;
    else return lo<other.lo;
}

bool uuid_t::operator ==(const uuid_t& u) const
{
    return (u.hi == hi) && (u.lo == lo);
}

Attribute::Attribute()
{
    this->size = 0;
    this->type = INVALID;
    intVal.lo = intVal.hi = 0;
    uintVal.lo = uintVal.hi = 0;
    uuidVal.lo = uuidVal.hi = 0;
    boolVal = false;
}

void Attribute::setNil()
{
    this->size = 0;
    this->type = NIL;
}

void Attribute::setInt(int len, int128_t val)
{
    this->size = len;
    this->type = INT;
    intVal = val;
}

void Attribute::setUInt(int len, uint128_t val)
{
    this->size = len;
    this->type = UINT;
    uintVal = val;
}

void Attribute::setUUID(int len, uuid_t val)
{
    this->size = len;
    this->type = UUID;
    uuidVal = val;
}

void Attribute::setBool(bool val)
{
    this->size = 1;
    this->type = BOOLEAN;
    boolVal = val;
}

void Attribute::setString(QCString val)
{
    this->size = val.length();
    this->type = STRING;
    this->stringVal = val;
}

void Attribute::setURL(QCString val)
{
    this->size = val.length();
    this->type = URL;
    this->stringVal = val;
}

void Attribute::setSequence(const AttrVec& val)
{
    this->size = val.size(); // TODO: What size? Number of elements or bytes?
    this->type = SEQUENCE;
    this->sequenceVal = val;
}

void Attribute::setAlternative(const AttrVec& val)
{
    this->size = val.size();
    this->type = ALTERNATIVE;
    this->sequenceVal = val;
}


Attribute::~Attribute()
{}


QCString Attribute::getString() const
{
    assert(type == STRING);
    return stringVal;
}

QCString Attribute::getURL() const
{
    assert(type == URL);
    return stringVal;
}

Attribute::int128_t Attribute::getInt() const
{
    assert(type == INT);
    return intVal.lo;
}


Attribute::uint128_t Attribute::getUInt() const
{
    assert(type == UINT);
    return uintVal;
}

uuid_t Attribute::getUUID() const
{
    assert(type == UUID);
    return uuidVal;
}

bool Attribute::getBool() const
{
    assert(type == BOOLEAN);
    return boolVal;
}

AttrVec Attribute::getSequence() const
{
    assert(type == SEQUENCE);
    return sequenceVal;
}

AttrVec Attribute::getAlternative() const
{
    assert(type == ALTERNATIVE);
    return sequenceVal;
}

vector<uuid_t> Attribute::getAllUUIDs() const
{
    vector<uuid_t> uuids;
    if (getType() == UUID)
    {
        kdDebug() << "Pushed UUID: " << (long int)(getUUID().hi)
        << (long int)(getUUID().lo) << endl;
        uuids.push_back(getUUID());
    }
    else
    {
        AttrVec subAttributes;
        if (getType() == SEQUENCE)
        {
            subAttributes = getSequence();
        }
        else if (getType() == ALTERNATIVE)
        {
            subAttributes = getAlternative();
        }
        AttrVec::iterator it;
        for (it = subAttributes.begin(); it != subAttributes.end(); ++it)
        {
            vector<uuid_t> subUUIDs = it->getAllUUIDs();
            copy(subUUIDs.begin(), subUUIDs.end(), back_inserter(uuids));
        }
    }
    return uuids;
}


int Attribute::getSize() const
{
    return this->size;
}

QString Attribute::getTypeString() const
{
    QString typeStr = "";
    switch (this->type)
    {
    case INVALID:
        typeStr = "invalid";
        break;
    case NIL:
        typeStr = "nil";
        break;
    case UINT:
        typeStr = "uint";
        break;
    case INT:
        typeStr = "int";
        break;
    case UUID:
        typeStr = "uuid";
        break;
    case BOOLEAN:
        typeStr = "boolean";
        break;
    case STRING:
        typeStr = "string";
        break;
    case SEQUENCE:
        typeStr = "sequence";
        break;
    case ALTERNATIVE:
        typeStr = "alternative";
        break;
    case URL:
        typeStr = "url";
        break;
    case UNKNOWN:
        typeStr = "unknown";
        break;
    }
    return typeStr;
}


Attribute::AttrType Attribute::getType() const
{
    return type;
}

}
}
