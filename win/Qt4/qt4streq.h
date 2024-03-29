// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4streq.h -- string requestor

#ifndef QT4STREQ_H
#define QT4STREQ_H

#include "qt4line.h"

namespace lumahack_qt4 {

class LumaHackQtStringRequestor : QDialog {
private:
	QLabel prompt;
	LumaHackQtLineEdit input;
	QPushButton* okay;
	QPushButton* cancel;

public:
	LumaHackQtStringRequestor(QWidget *parent, const char* p,const char* cancelstr="Cancel");
	void SetDefault(const char*);
	bool Get(char* buffer, int maxchar=80);
	virtual void resizeEvent(QResizeEvent*);
};

} // namespace lumahack_qt4

#endif
