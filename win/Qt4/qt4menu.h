// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4menu.cpp -- a menu or text-list widget

#ifndef QT4MENU_H
#define QT4MENU_H

#include "qt4win.h"
#include "qt4rip.h"

namespace lumahack_qt4 {

class LumaHackQtTextListBox : public QListWidget {
public:
    LumaHackQtTextListBox(QWidget* parent = NULL) : QListWidget(parent) { }

    int TotalWidth() const
    {
	int width = 0;
	QFontMetrics fm(font());
	for (int i = 0; i < count(); i++) {
	    int lwidth = fm.width(item(i)->text());
	    width = std::max(width, lwidth);
	}
	return width;
    }
    int TotalHeight() const
    {
	QFontMetrics fm(font());
	return fm.height() * count();
    }

    virtual QSize sizeHint() const;
};

class LumaHackQtMenuListBox : public QTableWidget {
public:
    LumaHackQtMenuListBox(QWidget* parent = NULL) : QTableWidget(parent) { }

    int TotalWidth() const;
    int TotalHeight() const;

    virtual QSize sizeHint() const;
};

class LumaHackQtMenuWindow : public QDialog, public LumaHackQtWindow {
	Q_OBJECT
public:
	LumaHackQtMenuWindow(QWidget *parent = NULL);
	~LumaHackQtMenuWindow();

	virtual QWidget* Widget();

	virtual void StartMenu();
	virtual void AddMenu(int glyph, const ANY_P* identifier, char ch, char gch, int attr,
			const QString& str, bool presel);
	virtual void EndMenu(const QString& prompt);
	virtual int SelectMenu(int how, MENU_ITEM_P **menu_list);

public slots:
	void All();
	void ChooseNone();
	void Invert();
	void Search();

	void ToggleSelect(int);
        void cellToggleSelect(int, int);
	void DoSelection(bool);

protected:
	virtual void keyPressEvent(QKeyEvent*);

private:
	struct MenuItem {
		MenuItem();
		~MenuItem();

		int glyph;
		ANY_P identifier;
		int attr;
		QString str;
		int count;
		char ch;
                char gch;
		bool selected;
                unsigned color;

		bool Selectable() const { return identifier.a_void!=0; }
	};

	QVector<MenuItem> itemlist;

	int itemcount;
	int next_accel;

	QTableWidget* table;
	QPushButton* ok;
	QPushButton* cancel;
	QPushButton* all;
	QPushButton* none;
	QPushButton* invert;
	QPushButton* search;
	QLabel prompt;

	// Count replaces prompt while it is being input
	QString promptstr;
	QString countstr;
	bool counting;
	void InputCount(char key);
	void ClearCount(void);

	int how;

	bool has_glyphs;

	bool isSelected(int row);
	int count(int row);

	void AddRow(int row, const MenuItem& mi);
	void WidenColumn(int column, int width);
};

class LumaHackQtTextWindow : public QDialog, public LumaHackQtWindow {
	Q_OBJECT
public:
	LumaHackQtTextWindow(QWidget *parent = NULL);
	~LumaHackQtTextWindow();

	virtual QWidget* Widget();

	virtual void Clear();
	virtual bool Destroy();
	virtual void Display(bool block);
	virtual void PutStr(int attr, const QString& text);
	virtual void UseRIP(int how, time_t when);

public slots:
	void Search();

private slots:
	void doUpdate();

private:
	bool use_rip;
	bool str_fixed;

	QPushButton ok;
	QPushButton search;
	LumaHackQtTextListBox* lines;

	LumaHackQtRIP rip;
};

class LumaHackQtMenuOrTextWindow : public LumaHackQtWindow {
private:
	LumaHackQtWindow* actual;
    QWidget *parent;

public:
	LumaHackQtMenuOrTextWindow(QWidget *parent = NULL);

	virtual QWidget* Widget();

	// Text
	virtual void Clear();
	virtual bool Destroy();
	virtual void Display(bool block);
	virtual void PutStr(int attr, const QString& text);

	// Menu
	virtual void StartMenu();
	virtual void AddMenu(int glyph, const ANY_P* identifier, char ch, char gch, int attr,
			const QString& str, bool presel);
	virtual void EndMenu(const QString& prompt);
	virtual int SelectMenu(int how, MENU_ITEM_P **menu_list);

};

} // namespace lumahack_qt4

#endif
