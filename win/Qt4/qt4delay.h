// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4delay.h -- implement a delay

#ifndef QT4DELAY_H
#define QT4DELAY_H

namespace lumahack_qt4 {

class LumaHackQtDelay : QObject {
private:
	int msec;
    int m_timer;
    QEventLoop m_loop;

public:
	LumaHackQtDelay(int ms);
	void wait();
	virtual void timerEvent(QTimerEvent* timer);
};

} // namespace lumahack_qt4

#endif
