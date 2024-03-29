// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4yndlg.h -- yes/no dialog

#ifndef QT4YNDLG_H
#define QT4YNDLG_H

namespace lumahack_qt4 {

class LumaHackQtYnDialog : QDialog {
	Q_OBJECT
private:
	QString question;
	const char* choices;
	char def;
	char keypress;

protected:
	virtual void keyPressEvent(QKeyEvent*);

private slots:
	void doneItem(int);

public:
	LumaHackQtYnDialog(QWidget *,const QString&,const char*,char);

	char Exec();
};

} // namespace lumahack_qt4

#endif
