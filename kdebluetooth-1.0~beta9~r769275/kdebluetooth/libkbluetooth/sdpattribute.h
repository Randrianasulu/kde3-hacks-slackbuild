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

#ifndef SDPATTRIBUTE_H
#define SDPATTRIBUTE_H

#include <qstring.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <bluetooth/sdp.h>

namespace KBluetooth
{
namespace SDP
{

class Attribute;
typedef std::vector<Attribute> AttrVec;


struct uuid_t
{
    uuid_t(QString s);
    uuid_t(uint64_t l=0, uint64_t h=0);
    uint64_t hi;
    uint64_t lo;
    bool fromString(QString s);
    bool operator==(const uuid_t& uuid) const;
    void setUUID128(uint64_t hi, uint64_t lo);
    void setUUID32(uint32_t v);
    operator QString() const;
    operator ::uuid_t() const;
    bool operator<(const uuid_t other) const;
};

typedef std::vector<uuid_t> UUIDVec;

/**
@author Fred Schaettgen
*/
class Attribute
{
public:
    enum AttrType {INVALID, NIL, UINT, INT, UUID, BOOLEAN, STRING,
                   SEQUENCE, ALTERNATIVE, URL, UNKNOWN};

    struct int128_t
    {
        int128_t(int64_t l=0, int64_t h=0)
        {
            hi = h;
            lo = l;
        }
        int64_t hi;
        int64_t lo;
    };

    struct uint128_t
    {
        uint128_t(uint64_t l=0, uint64_t h=0)
        {
            hi = h;
            lo = l;
        }
        uint64_t hi;
        uint64_t lo;
    };


private:
    AttrType type;

    Attribute::int128_t intVal;
    Attribute::uint128_t uintVal;
    uuid_t uuidVal;
    bool boolVal;
    QCString stringVal;     // strings and urls
    AttrVec sequenceVal;   // sequences and alternatives
    int size;              // length of the value in bytes

public:
    void setNil();
    void setInt(int len, Attribute::int128_t val);
    void setUInt(int len, Attribute::uint128_t val);
    void setUUID(int len, uuid_t val);
    void setBool(bool val);
    void setString(QCString val);
    void setURL(QCString val);
    void setSequence(const AttrVec& val);
    void setAlternative(const AttrVec& val);

    QCString getString() const;
    QCString getURL() const;
    Attribute::int128_t getInt() const;
    uint128_t getUInt() const;
    uuid_t getUUID() const;
    bool getBool() const;
    AttrVec getSequence() const;
    AttrVec getAlternative() const;

    std::vector<uuid_t> getAllUUIDs() const;

    Attribute();
    AttrType getType() const;
    int getSize() const;
    //QString getValString();
    QString getTypeString() const;

    ~Attribute();
};


}
}

#endif
