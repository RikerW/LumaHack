// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4msg.h -- a message window

#ifndef QT4MSG_H
#define QT4MSG_H

#include "qt4win.h"

namespace lumahack_qt4 {

class LumaHackQtMapWindow2;

class LumaHackQtMessageWindow : QObject, public LumaHackQtWindow {
	Q_OBJECT
public:
	LumaHackQtMessageWindow();
	~LumaHackQtMessageWindow();

	virtual QWidget* Widget();
	virtual void Clear();
	virtual void Display(bool block);
        virtual const char *GetStr(bool init);
	virtual void PutStr(int attr, const QString& text);

	void Scroll(int dx, int dy);
        void ClearMessages();

	void setMap(LumaHackQtMapWindow2*);

private:
	QListWidget* list;
	bool changed;
        int currgetmsg;
	LumaHackQtMapWindow2* map;

private slots:
	void updateFont();
};

} // namespace lumahack_qt4

#endif
