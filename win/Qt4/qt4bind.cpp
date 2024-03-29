// Copyright (c) Warwick Allison, 1999.
// Qt4 conversion copyright (c) Ray Chason, 2012-2014.
// LumaHack may be freely redistributed.  See license for details.

// qt4bind.cpp -- bindings between the Qt 4 interface and the main code

extern "C" {
#include "hack.h"
}
#undef Invisible
#undef Warning
#undef index
#undef msleep
#undef rindex
#undef wizard
#undef yn
#undef min
#undef max

#include <QtGui/QtGui>
#include <QtCore/QStringList>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QtWidgets>
#include <QtMultimedia/QSound>
#else
#include <QtGui/QSound>
#endif
#include "qt4bind.h"
#include "qt4click.h"
#ifdef TIMED_DELAY
#include "qt4delay.h"
#endif
#include "qt4xcmd.h"
#include "qt4key.h"
#include "qt4map.h"
#include "qt4menu.h"
#include "qt4msg.h"
#include "qt4plsel.h"
#include "qt4svsel.h"
#include "qt4set.h"
#include "qt4stat.h"
#include "qt4streq.h"
#include "qt4yndlg.h"
#include "qt4str.h"

extern "C" {
#include "dlb.h"
}

// temporary
extern int qt_compact_mode;
// end temporary

namespace lumahack_qt4 {

// XXX Should be from Options
//
// XXX Hmm.  Tricky part is that perhaps some macros should only be active
// XXX       when a key is about to be gotten.  For example, the user could
// XXX       define "-" to do "E-yyyyyyyy\r", but would still need "-" for
// XXX       other purposes.  Maybe just too bad.
//
static struct key_macro_rec {
    int key;
    int state;
    const char* macro;
} key_macro[]={
    { Qt::Key_F1, 0, "n100." }, // Rest (x100)
    { Qt::Key_F2, 0, "n20s" },  // Search (x20)
    { Qt::Key_Tab, 0, "\001" },
    { 0, 0, 0 }
};

LumaHackQtBind::LumaHackQtBind(int& argc, char** argv) :
#ifdef KDE
    KApplication(argc,argv)
#elif defined(QWS) // not quite the right condition
    QPEApplication(argc,argv)
#else
    QApplication(argc,argv)
#endif
{
    QPixmap pm("nhsplash.xpm");
    if ( iflags.wc_splash_screen && !pm.isNull() ) {
	splash = new QFrame(NULL,
	    Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint );
	QVBoxLayout *vb = new QVBoxLayout(splash);
	QLabel *lsplash = new QLabel(splash);
	vb->addWidget(lsplash);
	lsplash->setAlignment(Qt::AlignCenter);
	lsplash->setPixmap(pm);
	QLabel* capt = new QLabel("Loading...",splash);
	vb->addWidget(capt);
	capt->setAlignment(Qt::AlignCenter);
	if ( !pm.isNull() ) {
	    lsplash->setFixedSize(pm.size());
	    lsplash->setMask(pm);
	}
	splash->move((QApplication::desktop()->width()-pm.width())/2,
		      (QApplication::desktop()->height()-pm.height())/2);
	//splash->setGeometry(0,0,100,100);
	if ( qt_compact_mode ) {
	    splash->showMaximized();
	} else {
	    splash->setFrameStyle(QFrame::WinPanel|QFrame::Raised);
	    splash->setLineWidth(10);
	    splash->adjustSize();
	    splash->show();
	}

	// force content refresh outside event loop
	splash->repaint();
	lsplash->repaint();
	capt->repaint();
	qApp->flush();

    } else {
	splash = 0;
    }
    main = new LumaHackQtMainWindow(keybuffer);
    connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
    qt_settings=new LumaHackQtSettings(main->width(),main->height());
    msgs_strings = new QStringList();
    msgs_initd = false;
    msgs_saved = false;
}

void LumaHackQtBind::qt_init_nhwindows(int* argc, char** argv)
{
    iflags.menu_tab_sep = true;

#ifdef UNIX
// Userid control
//
// Michael Hohmuth <hohmuth@inf.tu-dresden.de>...
//
// As the game runs setuid games, it must seteuid(getuid()) before
// calling XOpenDisplay(), and reset the euid afterwards.
// Otherwise, it can't read the $HOME/.Xauthority file and whines about
// not being able to open the X display (if a magic-cookie
// authorization mechanism is being used). 

    uid_t gamesuid=geteuid();
    seteuid(getuid());
#endif

    QApplication::setColorSpec(ManyColor);
    instance=new LumaHackQtBind(*argc,argv);

#ifdef UNIX
    seteuid(gamesuid);
#endif

#ifdef _WS_WIN_
    // This lumahack engine feature should be moved into windowport API
    nt_kbhit = LumaHackQtBind::qt_kbhit;
#endif
}

int LumaHackQtBind::qt_kbhit()
{
    return !keybuffer.Empty();
}


static bool have_asked = false;

void LumaHackQtBind::qt_player_selection()
{
    if ( !have_asked )
	qt_askname();
}

void LumaHackQtBind::qt_askname()
{
    have_asked = true;

    // We do it all here, and nothing in askname

    char** saved = get_saved_games();
    int ch = -1;
    if ( saved && *saved ) {
	if ( splash ) splash->hide();
	LumaHackQtSavedGameSelector sgsel((const char**)saved);
	ch = sgsel.choose();
	if ( ch >= 0 )
	    str_copy(plname, saved[ch], SIZE(plname));
    }
    free_saved_games(saved);

    switch (ch) {
      case -1:
	if ( splash ) splash->hide();
	if (LumaHackQtPlayerSelector(keybuffer).Choose())
	    return;
      case -2:
	break;
      default:
	return;
    }

    // Quit
    clearlocks();
    qt_exit_nhwindows(0);
    nh_terminate(0);
}

void LumaHackQtBind::qt_get_nh_event()
{
}

#if defined(QWS)
// Kludge to access lastWindowClosed() signal.
class TApp : public QApplication {
public:
    TApp(int& c, char**v) : QApplication(c,v) {}
    void lwc() { emit lastWindowClosed(); }
};
#endif
 
void LumaHackQtBind::qt_exit_nhwindows(const char *)
{
#if defined(QWS)
    // Avoids bug in SHARP SL5500
    ((TApp*)qApp)->lwc();
    qApp->quit();
#endif
 
    delete instance; // ie. qApp
}

void LumaHackQtBind::qt_suspend_nhwindows(const char *)
{
}

void LumaHackQtBind::qt_resume_nhwindows()
{
}

static QVector<LumaHackQtWindow*> id_to_window;

winid LumaHackQtBind::qt_create_nhwindow(int type)
{
    winid id;
    for (id = 0; id < (winid) id_to_window.size(); id++) {
	if ( !id_to_window[(int)id] )
	    break;
    }
    if ( id == (winid) id_to_window.size() )
	id_to_window.resize(id+1);

    LumaHackQtWindow* window=0;

    switch (type) {
     case NHW_MAP: {
	LumaHackQtMapWindow2* w=new LumaHackQtMapWindow2(clickbuffer);
	main->AddMapWindow(w);
	window=w;
    } break; case NHW_MESSAGE: {
	LumaHackQtMessageWindow* w=new LumaHackQtMessageWindow;
	main->AddMessageWindow(w);
	window=w;
    } break; case NHW_STATUS: {
	LumaHackQtStatusWindow* w=new LumaHackQtStatusWindow;
	main->AddStatusWindow(w);
	window=w;
    } break; case NHW_MENU:
	window=new LumaHackQtMenuOrTextWindow(mainWidget());
    break; case NHW_TEXT:
	window=new LumaHackQtTextWindow(mainWidget());
    }

    window->nhid = id;

    // Note: use of isHidden does not work with Qt 2.1
    if ( splash 
#if QT_VERSION >= 300
        && !main->isHidden()
#else
	&& main->isVisible()
#endif
	)
    {
	delete splash;
	splash = 0;
    }

    id_to_window[(int)id] = window;
    return id;
}

void LumaHackQtBind::qt_clear_nhwindow(winid wid)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->Clear();
}

void LumaHackQtBind::qt_display_nhwindow(winid wid, BOOLEAN_P block)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->Display(block);
}

void LumaHackQtBind::qt_destroy_nhwindow(winid wid)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    main->RemoveWindow(window);
    if (window->Destroy())
	delete window;
    id_to_window[(int)wid] = 0;
}

void LumaHackQtBind::qt_curs(winid wid, int x, int y)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->CursorTo(x,y);
}

void LumaHackQtBind::qt_putstr(winid wid, int attr, const char *text)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->PutStr(attr,QString::fromLatin1(text));
}

void LumaHackQtBind::qt_putstr(winid wid, int attr, const std::string& text)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->PutStr(attr,QString::fromLatin1(text.c_str(), text.size()));
}

void LumaHackQtBind::qt_putstr(winid wid, int attr, const QString& text)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->PutStr(attr,text);
}

void LumaHackQtBind::qt_display_file(const char *filename, BOOLEAN_P must_exist)
{
    LumaHackQtTextWindow* window=new LumaHackQtTextWindow(mainWidget());
    bool complain = false;

    {
	dlb *f;
	char buf[BUFSZ];
	char *cr;

	window->Clear();
	f = dlb_fopen(filename, "r");
	if (!f) {
	    complain = must_exist;
	} else {
	    while (dlb_fgets(buf, BUFSZ, f)) {
		if ((cr = strchr(buf, '\n')) != 0) *cr = 0;
#ifdef MSDOS
		if ((cr = strchr(buf, '\r')) != 0) *cr = 0;
#endif
		window->PutStr(ATR_NONE, tabexpand(buf));
	    }
	    window->Display(false);
	    (void) dlb_fclose(f);
	}
    }

    if (complain) {
	QString message;
	message.sprintf("File not found: %s\n",filename);
	QMessageBox::warning(NULL, "File Error", message, QMessageBox::Ignore);
    }
}

void LumaHackQtBind::qt_start_menu(winid wid)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->StartMenu();
}

void LumaHackQtBind::qt_add_menu(winid wid, int glyph,
    const ANY_P * identifier, CHAR_P ch, CHAR_P gch, int attr,
    const char *str, BOOLEAN_P presel)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->AddMenu(glyph, identifier, ch, gch, attr,
            QString::fromLatin1(str),
            presel);
}

void LumaHackQtBind::qt_end_menu(winid wid, const char *prompt)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->EndMenu(prompt);
}

int LumaHackQtBind::qt_select_menu(winid wid, int how, MENU_ITEM_P **menu_list)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    return window->SelectMenu(how,menu_list);
}

void LumaHackQtBind::qt_update_inventory()
{
    if (main)
	main->updateInventory();
    /* doesn't work yet
    if (program_state.something_worth_saving && iflags.perm_invent)
        display_inventory(NULL, false);
    */
}

void LumaHackQtBind::qt_mark_synch()
{
}

void LumaHackQtBind::qt_wait_synch()
{
}

void LumaHackQtBind::qt_cliparound(int x, int y)
{
    // XXXNH - winid should be a parameter!
    qt_cliparound_window(WIN_MAP,x,y);
}

void LumaHackQtBind::qt_cliparound_window(winid wid, int x, int y)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->ClipAround(x,y);
}
void LumaHackQtBind::qt_print_glyph(winid wid,XCHAR_P x,XCHAR_P y,int glyph,int bkglyph)
{
    /* TODO: bkglyph */
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->PrintGlyph(x,y,glyph);
}
//void LumaHackQtBind::qt_print_glyph_compose(winid wid,xchar x,xchar y,int glyph1, int glyph2)
//{
    //LumaHackQtWindow* window=id_to_window[(int)wid];
    //window->PrintGlyphCompose(x,y,glyph1,glyph2);
//}

void LumaHackQtBind::qt_raw_print(const char *str)
{
    puts(str);
}

void LumaHackQtBind::qt_raw_print_bold(const char *str)
{
    puts(str);
}

int LumaHackQtBind::qt_nhgetch()
{
    if (main)
	main->fadeHighlighting();

    // Process events until a key arrives.
    //
    while (keybuffer.Empty()) {
        int exc = qApp->exec();
        /*
         * On OSX (possibly elsewhere), this prevents an infinite
         * loop repeatedly issuing the complaint:
QCoreApplication::exec: The event loop is already running
         * to stderr if you syncronously start lumahack from a terminal
         * then switch focus back to that terminal and type ^C.
         *  SIGINT -> done1() -> done2() -> yn_function("Really quit?")
         * in the core asks for another keystroke.
         *
         * However, it still issues one such complaint, and whatever
         * prompt wanted a response ("Really quit?") is shown in the
         * message window but is auto-answered with ESC.
         */
        if (exc == -1)
            keybuffer.Put('\033');
    }

    return keybuffer.GetAscii();
}

int LumaHackQtBind::qt_nh_poskey(int *x, int *y, int *mod)
{
    if (main)
	main->fadeHighlighting();

    // Process events until a key or map-click arrives.
    //
    while (keybuffer.Empty() && clickbuffer.Empty()) {
        int exc = qApp->exec();
        // [see comment above in qt_nhgetch()]
        if (exc == -1)
            keybuffer.Put('\033');
    }
    if (!keybuffer.Empty()) {
	return keybuffer.GetAscii();
    } else {
	*x=clickbuffer.NextX();
	*y=clickbuffer.NextY();
	*mod=clickbuffer.NextMod();
	clickbuffer.Get();
	return 0;
    }
}

void LumaHackQtBind::qt_nhbell()
{
    QApplication::beep();
}

int LumaHackQtBind::qt_doprev_message()
{
    // Don't need it - uses scrollbar
    // XXX but could make this a shortcut
    return 0;
}

char LumaHackQtBind::qt_yn_function(const char *question_, const char *choices, CHAR_P def)
{
    QString question(QString::fromLatin1(question_));
    QString message;
    char yn_esc_map='\033';

    if (choices) {
        // anything beyond <esc> is hidden>
        QString choicebuf = choices;
        size_t cb = choicebuf.indexOf('\033');
        choicebuf = choicebuf.mid(0U, cb);
        message = QString("%1 [%2] ").arg(question, choicebuf);
        if (def) message += QString("(%1) ").arg(QChar(def));
        // escape maps to 'q' or 'n' or default, in that order
        yn_esc_map = (strchr(choices, 'q') ? 'q' :
                      (strchr(choices, 'n') ? 'n' : def));
    } else {
        message = question;
    }

    if (qt_settings->ynInMessages() && WIN_MESSAGE!=WIN_ERR) {
	// Similar to X11 windowport `slow' feature.

	int result = -1;

#ifdef USE_POPUPS
        if (choices) {
            if (!strcmp(choices,"ynq"))
                result = QMessageBox::information (LumaHackQtBind::mainWidget(),"LumaHack",question,"&Yes","&No","&Quit",0,2);
            else if (!strcmp(choices,"yn"))
                result = QMessageBox::information(LumaHackQtBind::mainWidget(),"LumaHack",question,"&Yes", "&No",0,1);
            else if (!strcmp(choices, "rl"))
                result = QMessageBox::information(LumaHackQtBind::mainWidget(),"LumaHack",question,"&Right", "&Left",0,1);

            if (result >= 0 && result < strlen(choices)) {
                char yn_resp = choices[result];
                message += QString(" %1").arg(yn_resp);
                result = yn_resp;
            }
        }
#endif

	LumaHackQtBind::qt_putstr(WIN_MESSAGE, ATR_BOLD, message);

	while (result < 0) {
	    char ch=LumaHackQtBind::qt_nhgetch();
	    if (ch=='\033') {
		result=yn_esc_map;
	    } else if (choices && !strchr(choices,ch)) {
		if (def && (ch==' ' || ch=='\r' || ch=='\n')) {
		    result=def;
		} else {
		    LumaHackQtBind::qt_nhbell();
		    // and try again...
		}
	    } else {
		result=ch;
	    }
	}

	LumaHackQtBind::qt_clear_nhwindow(WIN_MESSAGE);

	return result;
    } else {
	LumaHackQtYnDialog dialog(mainWidget(),question,choices,def);
	char ret = dialog.Exec();
        if (!(ret == '\0' || ret == '\033') && choices)
            message += QString(" %1").arg(ret);
        else if (def)
            message += QString(" %1").arg(def);
	LumaHackQtBind::qt_putstr(WIN_MESSAGE, ATR_BOLD, message);

        return ret;
    }
}

void LumaHackQtBind::qt_getlin(const char *prompt, char *line)
{
    LumaHackQtStringRequestor requestor(mainWidget(),prompt);
    if (!requestor.Get(line)) {
	line[0]=0;
    }
}

int LumaHackQtBind::qt_get_ext_cmd()
{
    LumaHackQtExtCmdRequestor requestor(mainWidget());
    return requestor.get();
}

void LumaHackQtBind::qt_number_pad(int)
{
    // Ignore.
}

void LumaHackQtBind::qt_delay_output()
{
#ifdef TIMED_DELAY
    LumaHackQtDelay delay(50);
    delay.wait();
#endif
}

void LumaHackQtBind::qt_start_screen()
{
    // Ignore.
}

void LumaHackQtBind::qt_end_screen()
{
    // Ignore.
}

void LumaHackQtBind::qt_outrip(winid wid, int how, time_t when)
{
    LumaHackQtWindow* window=id_to_window[(int)wid];
    window->UseRIP(how, when);
}

char * LumaHackQtBind::qt_getmsghistory(BOOLEAN_P init)
{
    LumaHackQtMessageWindow* window = main->GetMessageWindow();
    if (window)
        return (char *)window->GetStr(init);
    return NULL;
}

void LumaHackQtBind::qt_putmsghistory(const char *msg, BOOLEAN_P is_restoring)
{
    LumaHackQtMessageWindow* window = main->GetMessageWindow();
    if (!window)
        return;

    if (is_restoring && !msgs_initd) {
        /* we're restoring history from the previous session, but new
           messages have already been issued this session */
        int i = 0;
        const char *str;

        while ((str = window->GetStr((i == 0)))) {
            msgs_strings->append(str);
            i++;
        }
        msgs_initd = true;
        msgs_saved = (i > 0);
        window->ClearMessages();
    }

    if (msg) {
        //raw_printf("msg='%s'", msg);
        window->PutStr(ATR_NONE, QString::fromLatin1(msg));
#ifdef DUMPLOG
        dumplogmsg(msg);
#endif
    } else if (msgs_saved) {
        /* restore strings */
        int i;
        for (i = 0; i < msgs_strings->size(); i++) {
            window->PutStr(ATR_NONE, msgs_strings->at((i)));
#ifdef DUMPLOG
            dumplogmsg(msgs_strings->at(i).toLatin1().constData());
#endif
        }
        delete msgs_strings;
        msgs_initd = false;
    }
}

bool LumaHackQtBind::notify(QObject *receiver, QEvent *event)
{
    // Ignore Alt-key navigation to menubar, it's annoying when you
    // use Alt-Direction to move around.
    if ( main && event->type()==QEvent::KeyRelease && main==receiver
	    && ((QKeyEvent*)event)->key() == Qt::Key_Alt )
	return true;

    bool result=QApplication::notify(receiver,event);
    if (event->type()==QEvent::KeyPress) {
	QKeyEvent* key_event=(QKeyEvent*)event;

	if (!key_event->isAccepted()) {
	    const int k=key_event->key();
	    bool macro=false;
	    for (int i=0; !macro && key_macro[i].key; i++) {
		if (key_macro[i].key==k
		 && ((key_macro[i].state&key_event->modifiers())==key_macro[i].state))
		{
		    keybuffer.Put(key_macro[i].macro);
		    macro=true;
		}
	    }
	    QString key=key_event->text();
	    QChar ch = !key.isEmpty() ? key.at(0) : 0;
	    if (ch > 128) ch = 0;
	    if ( ch == 0 && (key_event->modifiers() & Qt::ControlModifier) ) {
		// On Mac, ascii control codes are not sent, force them.
		if ( k>=Qt::Key_A && k<=Qt::Key_Z )
		    ch = k - Qt::Key_A + 1;
	    }
	    if (!macro && ch != 0) {
		bool alt = (key_event->modifiers()&Qt::AltModifier) ||
		   (k >= Qt::Key_0 && k <= Qt::Key_9 && (key_event->modifiers()&Qt::ControlModifier));
		keybuffer.Put(key_event->key(),ch.cell() + (alt ? 128 : 0),
		    key_event->modifiers());
		key_event->accept();
		result=true;
	    }

	    if (ch != 0 || macro) {
		qApp->exit();
	    }
	}
    }
    return result;
}

LumaHackQtBind* LumaHackQtBind::instance=0;
LumaHackQtKeyBuffer LumaHackQtBind::keybuffer;
LumaHackQtClickBuffer LumaHackQtBind::clickbuffer;
LumaHackQtMainWindow* LumaHackQtBind::main=0;
QFrame* LumaHackQtBind::splash=0;
QStringList *LumaHackQtBind::msgs_strings;
boolean LumaHackQtBind::msgs_saved = false;
boolean LumaHackQtBind::msgs_initd = false;

static void Qt_positionbar(char *) {}

} // namespace lumahack_qt4

struct window_procs Qt_procs = {
    "Qt",
    WC_COLOR | WC_HILITE_PET
    | WC_ASCII_MAP | WC_TILED_MAP
    | WC_FONT_MAP | WC_TILE_FILE | WC_TILE_WIDTH | WC_TILE_HEIGHT
    | WC_PLAYER_SELECTION | WC_SPLASH_SCREEN,
    0L,
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},   /* color availability */
    lumahack_qt4::LumaHackQtBind::qt_init_nhwindows,
    lumahack_qt4::LumaHackQtBind::qt_player_selection,
    lumahack_qt4::LumaHackQtBind::qt_askname,
    lumahack_qt4::LumaHackQtBind::qt_get_nh_event,
    lumahack_qt4::LumaHackQtBind::qt_exit_nhwindows,
    lumahack_qt4::LumaHackQtBind::qt_suspend_nhwindows,
    lumahack_qt4::LumaHackQtBind::qt_resume_nhwindows,
    lumahack_qt4::LumaHackQtBind::qt_create_nhwindow,
    lumahack_qt4::LumaHackQtBind::qt_clear_nhwindow,
    lumahack_qt4::LumaHackQtBind::qt_display_nhwindow,
    lumahack_qt4::LumaHackQtBind::qt_destroy_nhwindow,
    lumahack_qt4::LumaHackQtBind::qt_curs,
    lumahack_qt4::LumaHackQtBind::qt_putstr,
    genl_putmixed,
    lumahack_qt4::LumaHackQtBind::qt_display_file,
    lumahack_qt4::LumaHackQtBind::qt_start_menu,
    lumahack_qt4::LumaHackQtBind::qt_add_menu,
    lumahack_qt4::LumaHackQtBind::qt_end_menu,
    lumahack_qt4::LumaHackQtBind::qt_select_menu,
    genl_message_menu,      /* no need for X-specific handling */
    lumahack_qt4::LumaHackQtBind::qt_update_inventory,
    lumahack_qt4::LumaHackQtBind::qt_mark_synch,
    lumahack_qt4::LumaHackQtBind::qt_wait_synch,
#ifdef CLIPPING
    lumahack_qt4::LumaHackQtBind::qt_cliparound,
#endif
#ifdef POSITIONBAR
    lumahack_qt4::Qt_positionbar,
#endif
    lumahack_qt4::LumaHackQtBind::qt_print_glyph,
    //LumaHackQtBind::qt_print_glyph_compose,
    lumahack_qt4::LumaHackQtBind::qt_raw_print,
    lumahack_qt4::LumaHackQtBind::qt_raw_print_bold,
    lumahack_qt4::LumaHackQtBind::qt_nhgetch,
    lumahack_qt4::LumaHackQtBind::qt_nh_poskey,
    lumahack_qt4::LumaHackQtBind::qt_nhbell,
    lumahack_qt4::LumaHackQtBind::qt_doprev_message,
    lumahack_qt4::LumaHackQtBind::qt_yn_function,
    lumahack_qt4::LumaHackQtBind::qt_getlin,
    lumahack_qt4::LumaHackQtBind::qt_get_ext_cmd,
    lumahack_qt4::LumaHackQtBind::qt_number_pad,
    lumahack_qt4::LumaHackQtBind::qt_delay_output,
#ifdef CHANGE_COLOR     /* only a Mac option currently */
    donull,
    donull,
    donull,
    donull,
#endif
    /* other defs that really should go away (they're tty specific) */
    lumahack_qt4::LumaHackQtBind::qt_start_screen,
    lumahack_qt4::LumaHackQtBind::qt_end_screen,
#ifdef GRAPHIC_TOMBSTONE
    lumahack_qt4::LumaHackQtBind::qt_outrip,
#else
    genl_outrip,
#endif
    genl_preference_update,

    lumahack_qt4::LumaHackQtBind::qt_getmsghistory,
    lumahack_qt4::LumaHackQtBind::qt_putmsghistory,
    genl_status_init,
    genl_status_finish, genl_status_enablefield,
#ifdef STATUS_HILITES
    genl_status_update,
#else
    genl_status_update,
#endif
    genl_can_suspend_yes,
};

#ifndef WIN32
extern "C" void play_usersound(const char* filename, int volume)
{
#ifdef USER_SOUNDS
#ifndef QT_NO_SOUND
    QSound::play(filename);
#endif
#endif
}
#endif
