// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4key.cpp -- a key buffer

#include "hack.h"
#undef Invisible
#undef Warning
#undef index
#undef msleep
#undef rindex
#undef wizard
#undef yn
#undef min
#undef max

#include <QtGui/QtGui>
#include "qt4key.h"

namespace lumahack_qt4 {

LumaHackQtKeyBuffer::LumaHackQtKeyBuffer() :
    in(0), out(0)
{
}

bool LumaHackQtKeyBuffer::Empty() const { return in==out; }
bool LumaHackQtKeyBuffer::Full() const { return (in+1)%maxkey==out; }

void LumaHackQtKeyBuffer::Put(int k, int a, int state)
{
    if ( Full() ) return;	// Safety
    key[in]=k;
    ascii[in]=a;
    in=(in+1)%maxkey;
}

void LumaHackQtKeyBuffer::Put(char a)
{
    Put(0,a,0);
}

void LumaHackQtKeyBuffer::Put(const char* str)
{
    while (*str) Put(*str++);
}

int LumaHackQtKeyBuffer::GetKey()
{
    if ( Empty() ) return 0;
    int r=TopKey();
    out=(out+1)%maxkey;
    return r;
}

int LumaHackQtKeyBuffer::GetAscii()
{
    if ( Empty() ) return 0; // Safety
    int r=TopAscii();
    out=(out+1)%maxkey;
    return r;
}

Qt::KeyboardModifiers LumaHackQtKeyBuffer::GetState()
{
    if ( Empty() ) return 0;
    Qt::KeyboardModifiers r=TopState();
    out=(out+1)%maxkey;
    return r;
}

int LumaHackQtKeyBuffer::TopKey() const
{
    if ( Empty() ) return 0;
    return key[out];
}

int LumaHackQtKeyBuffer::TopAscii() const
{
    if ( Empty() ) return 0;
    return ascii[out];
}

Qt::KeyboardModifiers LumaHackQtKeyBuffer::TopState() const
{
    if ( Empty() ) return 0;
    return state[out];
}

} // namespace lumahack_qt4
