// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4click.h -- a mouse click buffer

#ifndef QT4CLICK_H
#define QT4CLICK_H

namespace lumahack_qt4 {

class LumaHackQtClickBuffer {
public:
	LumaHackQtClickBuffer();

	bool Empty() const;
	bool Full() const;

	void Put(int x, int y, int mod);

	int NextX() const;
	int NextY() const;
	int NextMod() const;

	void Get();

private:
	enum { maxclick=64 };
	struct ClickRec {
		int x,y,mod;
	} click[maxclick];
	int in,out;
};

} // namespace lumahack_qt4

#endif
