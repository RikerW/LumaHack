// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4main.h -- the main window

#ifndef QT4MAIN_H
#define QT4MAIN_H

#ifdef KDE
#include <kapp.h>
#include <ktopwidget.h>
#else
#include "qt4kde0.h"
#endif

namespace lumahack_qt4 {

class LumaHackQtInvUsageWindow;
class LumaHackQtKeyBuffer;
class LumaHackQtMapWindow2;
class LumaHackQtMessageWindow;
class LumaHackQtStatusWindow;
class LumaHackQtWindow;

// This class is the main widget for LumaHack
//
// It is a collection of Message, Map, and Status windows.  In the current
// version of lumahack there is only one of each, and this class makes this
// assumption, not showing itself until all are inserted.
//
// This class simply knows how to layout such children sensibly.
//
// Since it is only responsible for layout, the class does not
// note the actual class of the windows.
//

class LumaHackQtMainWindow : public KTopLevelWidget {
	Q_OBJECT
public:
	LumaHackQtMainWindow(LumaHackQtKeyBuffer&);

	void AddMessageWindow(LumaHackQtMessageWindow* window);
	LumaHackQtMessageWindow * GetMessageWindow();
	void AddMapWindow(LumaHackQtMapWindow2* window);
	void AddStatusWindow(LumaHackQtStatusWindow* window);
	void RemoveWindow(LumaHackQtWindow* window);
	void updateInventory();

	void fadeHighlighting();

public slots:
	void doMenuItem(QAction *);
	void doQtSettings(bool);
	void doAbout(bool);
	//RLC void doGuidebook(bool);
	void doKeys(const QString&);

protected:
	virtual void resizeEvent(QResizeEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void closeEvent(QCloseEvent*);

private slots:
	void layout();
	void raiseMap();
	void zoomMap();
	void raiseMessages();
	void raiseStatus();

private:
	void ShowIfReady();

#ifdef KDE
	KMenuBar* menubar;
#else
	QMenuBar* menubar;
#endif
	LumaHackQtMessageWindow* message;
	LumaHackQtMapWindow2* map;
	LumaHackQtStatusWindow* status;
	LumaHackQtInvUsageWindow* invusage;

	QSplitter *hsplitter;
	QSplitter *vsplitter;

	LumaHackQtKeyBuffer& keysink;
	QStackedWidget* stack;
	int dirkey;
};

} // namespace lumahack_qt4

#endif
