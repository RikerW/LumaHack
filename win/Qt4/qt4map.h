// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4map.h -- the map window

#ifndef QT4MAP_H
#define QT4MAP_H

#include "qt4win.h"
#include "qt4clust.h"

namespace lumahack_qt4 {

class LumaHackQtClickBuffer;

class LumaHackQtMapViewport : public QWidget {
	Q_OBJECT
public:
	LumaHackQtMapViewport(LumaHackQtClickBuffer& click_sink);
	~LumaHackQtMapViewport(void);

protected:
	virtual void paintEvent(QPaintEvent* event);
	bool DrawWalls(QPainter& painter, int x, int y, int w, int h, unsigned ch);
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
	virtual void mousePressEvent(QMouseEvent* event);

private:
	QFont *rogue_font;
	unsigned short glyph[ROWNO][COLNO];
	unsigned short& Glyph(int x, int y) { return glyph[y][x]; }
	QPoint cursor;
	QPixmap pet_annotation;
        QPixmap pile_annotation;
	LumaHackQtClickBuffer& clicksink;
	Clusterizer change;

	void clickCursor();
	void Clear();
	void Display(bool block);
	void CursorTo(int x,int y);
	void PrintGlyph(int x,int y,int glyph);
	void Changed(int x, int y);
	void updateTiles();

	// LumaHackQtMapWindow2 passes through many calls to the viewport
	friend class LumaHackQtMapWindow2;
};

class LumaHackQtMapWindow2 : public QScrollArea, public LumaHackQtWindow {
	Q_OBJECT
public:
	LumaHackQtMapWindow2(LumaHackQtClickBuffer& click_sink);
	void clearMessages();
	void putMessage(int attr, const QString& text);
	void clickCursor();
	virtual QWidget *Widget();

	virtual void Clear();
	virtual void Display(bool block);
	virtual void CursorTo(int x,int y);
	virtual void PutStr(int attr, const QString& text);
	virtual void ClipAround(int x,int y);
	virtual void PrintGlyph(int x,int y,int glyph);

signals:
	void resized();

private slots:
	void updateTiles();

private:
	LumaHackQtMapViewport *m_viewport;
	QRect messages_rect;
	QString messages;
};

} // namespace lumahack_qt4

#endif
