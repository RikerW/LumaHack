// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4line.h -- a one line input window

#ifndef QT4LINE_H
#define QT4LINE_H

namespace lumahack_qt4 {

class LumaHackQtLineEdit : public QLineEdit {
public:
	LumaHackQtLineEdit();
	LumaHackQtLineEdit(QWidget* parent, const char* name);

	void fakeEvent(int key, int ascii, Qt::KeyboardModifiers state);
};

} // namespace lumahack_qt4

#endif
