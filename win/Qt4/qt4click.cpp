// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4click.cpp -- a mouse click buffer

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
#include "qt4click.h"

namespace lumahack_qt4 {

LumaHackQtClickBuffer::LumaHackQtClickBuffer() :
    in(0), out(0)
{
}

bool LumaHackQtClickBuffer::Empty() const { return in==out; }
bool LumaHackQtClickBuffer::Full() const { return (in+1)%maxclick==out; }

void LumaHackQtClickBuffer::Put(int x, int y, int mod)
{
    click[in].x=x;
    click[in].y=y;
    click[in].mod=mod;
    in=(in+1)%maxclick;
}

int LumaHackQtClickBuffer::NextX() const { return click[out].x; }
int LumaHackQtClickBuffer::NextY() const { return click[out].y; }
int LumaHackQtClickBuffer::NextMod() const { return click[out].mod; }

void LumaHackQtClickBuffer::Get()
{
    out=(out+1)%maxclick;
}

} // namespace lumahack_qt4
