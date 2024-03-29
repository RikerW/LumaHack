// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4rip.h -- tombstone window

#ifndef QT4RIP_H
#define QT4RIP_H

namespace lumahack_qt4 {

class LumaHackQtRIP : public QWidget {
private:
	static QPixmap* pixmap;
	char** line;
	int riplines;

public:
	LumaHackQtRIP(QWidget* parent);

	void setLines(char** l, int n);

protected:
	virtual void paintEvent(QPaintEvent* event);
	QSize sizeHint() const;
};

} // namespace lumahack_qt4

#endif
