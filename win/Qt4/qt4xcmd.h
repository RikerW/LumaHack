// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4xcmd.h -- extended command widget

#ifndef QT4XCMD_H
#define QT4XCMD_H

namespace lumahack_qt4 {

class LumaHackQtExtCmdRequestor : public QDialog {
    Q_OBJECT

protected:
    virtual void keyPressEvent(QKeyEvent *event);

public:
    LumaHackQtExtCmdRequestor(QWidget *parent);
    int get();

private:
    QLabel *prompt;
    QVector<QPushButton *> buttons;
    void enableButtons();

private slots:
    void cancel();
};

} // namespace lumahack_qt4

#endif
