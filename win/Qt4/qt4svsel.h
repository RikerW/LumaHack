// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4svsel.h -- saved game selector

#ifndef QT4SVSEL_H
#define QT4SVSEL_H

namespace lumahack_qt4 {

class LumaHackQtSavedGameSelector : public QDialog {
public:
	LumaHackQtSavedGameSelector(const char** saved);

	int choose();
};

} // namespace lumahack_qt4

#endif
