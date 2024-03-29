// LumaHack 3.6	qt_win.h	$NHDT-Date: 1447755972 2015/11/17 10:26:12 $  $NHDT-Branch: master $:$NHDT-Revision: 1.17 $
// Copyright (c) Warwick Allison, 1999.
// LumaHack may be freely redistributed.  See license for details.
//
// Qt Binding for LumaHack 3.4
//
// Unfortunately, this doesn't use Qt as well as I would like,
// primarily because LumaHack is fundamentally a getkey-type
// program rather than being event driven (hence the ugly key
// and click buffer rather), but also because this is my first
// major application of Qt.
//

#ifndef qt_win_h
#define qt_win_h

#define QT_CLEAN_NAMESPACE

#include <qdialog.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#if defined(QWS)
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qarray.h>
#include <qcombobox.h>
#include <qscrollview.h>
#if QT_VERSION >= 300
#include <qttableview.h>
// Should stop using QTableView
#define QTableView QtTableView
#else
#include <qtableview.h>
#endif
#include <qmainwindow.h>
#include <qwidgetstack.h>

#ifdef KDE
#include <kapp.h>
#include <ktopwidget.h>
#endif

#include "qt_clust.h"

class QVBox;
class QMenuBar;
class QRadioButton;
class NhPSListView;

//////////////////////////////////////////////////////////////
//
//  The beautiful, abstracted and well-modelled classes...
//
//////////////////////////////////////////////////////////////

class LumaHackQtGlyphs;

class LumaHackQtLineEdit : public QLineEdit
{
  public:
    LumaHackQtLineEdit();
    LumaHackQtLineEdit(QWidget *parent, const char *name);

    void fakeEvent(int key, int ascii, int state);
};

class LumaHackQtSettings : public QDialog
{
    Q_OBJECT
  public:
    // Size of window - used to decide default sizes
    LumaHackQtSettings(int width, int height);

    LumaHackQtGlyphs &glyphs();
    const QFont &normalFont();
    const QFont &normalFixedFont();
    const QFont &largeFont();

    bool ynInMessages();

  signals:
    void fontChanged();
    void tilesChanged();

  public slots:
    void toggleGlyphSize();
    void setGlyphSize(bool);

  private:
    QSpinBox tilewidth;
    QSpinBox tileheight;
    QLabel widthlbl;
    QLabel heightlbl;
    QCheckBox whichsize;
    QSize othersize;

    QComboBox fontsize;

    QFont normal, normalfixed, large;

    LumaHackQtGlyphs *theglyphs;

  private slots:
    void resizeTiles();
};

class LumaHackQtKeyBuffer
{
  public:
    LumaHackQtKeyBuffer();

    bool Empty() const;
    bool Full() const;

    void Put(int k, int ascii, int state);
    void Put(char a);
    void Put(const char *str);
    int GetKey();
    int GetAscii();
    int GetState();

    int TopKey() const;
    int TopAscii() const;
    int TopState() const;

  private:
    enum { maxkey = 64 };
    int key[maxkey];
    int ascii[maxkey];
    int state[maxkey];
    int in, out;
};

class LumaHackQtClickBuffer
{
  public:
    LumaHackQtClickBuffer();

    bool Empty() const;
    bool Full() const;

    void Put(int x, int y, int mod);

    int NextX() const;
    int NextY() const;
    int NextMod() const;

    void Get();

  private:
    enum { maxclick = 64 };
    struct ClickRec {
        int x, y, mod;
    } click[maxclick];
    int in, out;
};

class LumaHackQtSavedGameSelector : public QDialog
{
  public:
    LumaHackQtSavedGameSelector(const char **saved);

    int choose();
};

class LumaHackQtPlayerSelector : private QDialog
{
    Q_OBJECT
  public:
    enum { R_None = -1, R_Quit = -2, R_Rand = -3 };

    LumaHackQtPlayerSelector(LumaHackQtKeyBuffer &);

  protected:
    virtual void done(int);

  public slots:
    void Quit();
    void Random();

    void selectName(const QString &n);
    void selectRole();
    void selectRace();
    void setupOthers();
    void selectGender(int);
    void selectAlignment(int);

  public:
    bool Choose();

  private:
    LumaHackQtKeyBuffer &keysource;
    NhPSListView *role;
    NhPSListView *race;
    QRadioButton **gender;
    QRadioButton **alignment;
    bool fully_specified_role;
};

class LumaHackQtStringRequestor : QDialog
{
  private:
    QLabel prompt;
    LumaHackQtLineEdit input;
    QPushButton *okay;
    QPushButton *cancel;
    LumaHackQtKeyBuffer &keysource;

    virtual void done(int);

  public:
    LumaHackQtStringRequestor(LumaHackQtKeyBuffer &, const char *p,
                             const char *cancelstr = "Cancel");
    void SetDefault(const char *);
    bool Get(char *buffer, int maxchar = 80);
    virtual void resizeEvent(QResizeEvent *);
};

class LumaHackQtExtCmdRequestor : public QDialog
{
    Q_OBJECT

    LumaHackQtKeyBuffer &keysource;

  public:
    LumaHackQtExtCmdRequestor(LumaHackQtKeyBuffer &ks);
    int get();

  private slots:
    void cancel();
    void done(int i);
};

class LumaHackQtWindow
{
  public:
    LumaHackQtWindow();
    virtual ~LumaHackQtWindow();

    virtual QWidget *Widget() = 0;

    virtual void Clear();
    virtual void Display(bool block);
    virtual bool Destroy();
    virtual void CursorTo(int x, int y);
    virtual void PutStr(int attr, const char *text);
    virtual void StartMenu();
    virtual void AddMenu(int glyph, const ANY_P *identifier, char ch,
                         char gch, int attr, const char *str, bool presel);
    virtual void EndMenu(const char *prompt);
    virtual int SelectMenu(int how, MENU_ITEM_P **menu_list);
    virtual void ClipAround(int x, int y);
    virtual void PrintGlyph(int x, int y, int glyph);
    virtual void UseRIP(int how, time_t when);

    int nhid;
};

class LumaHackQtGlyphs
{
  public:
    LumaHackQtGlyphs();

    int
    width() const
    {
        return size.width();
    }
    int
    height() const
    {
        return size.height();
    }
    void toggleSize();
    void setSize(int w, int h);

    void drawGlyph(QPainter &, int glyph, int pixelx, int pixely);
    void drawCell(QPainter &, int glyph, int cellx, int celly);

  private:
    QImage img;
    QPixmap pm, pm1, pm2;
    QSize size;
    int tiles_per_row;
};

class BlackScrollView : public QScrollView
{
  public:
    BlackScrollView()
    {
        viewport()->setBackgroundColor(black);
    }
};

class LumaHackQtMapWindow : public QWidget, public LumaHackQtWindow
{
    Q_OBJECT
  private:
    LumaHackQtClickBuffer &clicksink;
    unsigned short glyph[ROWNO][COLNO];
    unsigned short &
    Glyph(int x, int y)
    {
        return glyph[y][x];
    }
    QPoint cursor;
    BlackScrollView viewport;
    QPixmap pet_annotation;
    Clusterizer change;
    QFont *rogue_font;
    QString messages;
    QRect messages_rect;

    void Changed(int x, int y);

  signals:
    void resized();

  private slots:
    void updateTiles();
    void moveMessages(int x, int y);
#ifdef SAFERHANGUP
    void timeout();
#endif

  protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);

  public:
    LumaHackQtMapWindow(LumaHackQtClickBuffer &click_sink);
    ~LumaHackQtMapWindow();

    virtual QWidget *Widget();
    virtual bool Destroy();

    virtual void Clear();
    virtual void Display(bool block);
    virtual void CursorTo(int x, int y);
    virtual void PutStr(int attr, const char *text);
    virtual void ClipAround(int x, int y);
    virtual void PrintGlyph(int x, int y, int glyph);

    void Scroll(int dx, int dy);

    // For messages
    void displayMessages(bool block);
    void putMessage(int attr, const char *text);
    void clearMessages();

    void clickCursor();
};

class LumaHackQtScrollText;
class LumaHackQtMessageWindow : QObject, public LumaHackQtWindow
{
    Q_OBJECT
  public:
    LumaHackQtMessageWindow();
    ~LumaHackQtMessageWindow();

    virtual QWidget *Widget();
    virtual void Clear();
    virtual void Display(bool block);
    virtual void PutStr(int attr, const char *text);

    void Scroll(int dx, int dy);

    void setMap(LumaHackQtMapWindow *);

  private:
    LumaHackQtScrollText *list;
    bool changed;
    LumaHackQtMapWindow *map;

  private slots:
    void updateFont();
};

class LumaHackQtLabelledIcon : public QWidget
{
  public:
    LumaHackQtLabelledIcon(QWidget *parent, const char *label);
    LumaHackQtLabelledIcon(QWidget *parent, const char *label,
                          const QPixmap &icon);

    enum { NoNum = -99999 };
    void setLabel(const char *, bool lower = TRUE);           // a string
    void setLabel(const char *, long, const char *tail = ""); // a number
    void setLabel(const char *, long show_value, long comparative_value,
                  const char *tail = "");
    void setIcon(const QPixmap &);
    virtual void setFont(const QFont &);

    void highlightWhenChanging();
    void lowIsGood();
    void dissipateHighlight();

    virtual void show();

  protected:
    void resizeEvent(QResizeEvent *);

  private:
    void initHighlight();
    void setAlignments();
    void highlight(const QPalette &highlight);
    void unhighlight();

    bool low_is_good;
    int prev_value;
    int turn_count; /* last time the value changed */
    QPalette hl_good;
    QPalette hl_bad;

    QLabel *label;
    QLabel *icon;
};

class LumaHackQtStatusWindow : QWidget, public LumaHackQtWindow
{
    Q_OBJECT
  public:
    LumaHackQtStatusWindow();

    virtual QWidget *Widget();

    virtual void Clear();
    virtual void Display(bool block);
    virtual void CursorTo(int x, int y);
    virtual void PutStr(int attr, const char *text);

    void fadeHighlighting();

  protected:
    void resizeEvent(QResizeEvent *);

  private slots:
    void doUpdate();

  private:
    enum { hilight_time = 1 };

    QPixmap p_str;
    QPixmap p_dex;
    QPixmap p_con;
    QPixmap p_int;
    QPixmap p_wis;
    QPixmap p_cha;

    QPixmap p_chaotic;
    QPixmap p_neutral;
    QPixmap p_lawful;

    QPixmap p_satiated;
    QPixmap p_hungry;

    QPixmap p_confused;
    QPixmap p_sick_fp;
    QPixmap p_sick_il;
    QPixmap p_blind;
    QPixmap p_stunned;
    QPixmap p_hallu;

    QPixmap p_encumber[5];

    LumaHackQtLabelledIcon name;
    LumaHackQtLabelledIcon dlevel;

    LumaHackQtLabelledIcon str;
    LumaHackQtLabelledIcon dex;
    LumaHackQtLabelledIcon con;
    LumaHackQtLabelledIcon intel;
    LumaHackQtLabelledIcon wis;
    LumaHackQtLabelledIcon cha;

    LumaHackQtLabelledIcon gold;
    LumaHackQtLabelledIcon hp;
    LumaHackQtLabelledIcon power;
    LumaHackQtLabelledIcon ac;
    LumaHackQtLabelledIcon level;
    LumaHackQtLabelledIcon exp;
    LumaHackQtLabelledIcon align;

    LumaHackQtLabelledIcon time;
    LumaHackQtLabelledIcon score;

    LumaHackQtLabelledIcon hunger;
    LumaHackQtLabelledIcon confused;
    LumaHackQtLabelledIcon sick_fp;
    LumaHackQtLabelledIcon sick_il;
    LumaHackQtLabelledIcon blind;
    LumaHackQtLabelledIcon stunned;
    LumaHackQtLabelledIcon hallu;
    LumaHackQtLabelledIcon encumber;

    QFrame hline1;
    QFrame hline2;
    QFrame hline3;

    int cursy;

    bool first_set;

    void nullOut();
    void updateStats();
    void checkTurnEvents();
};

class LumaHackQtMenuDialog : public QDialog
{
    Q_OBJECT
  public:
    LumaHackQtMenuDialog();

    void Accept();
    void Reject();
    void SetResult(int);

    virtual void done(int);

  protected:
    void resizeEvent(QResizeEvent *);

  signals:
    void Resized();
};

class LumaHackQtMenuWindow : public QTableView, public LumaHackQtWindow
{
    Q_OBJECT
  public:
    LumaHackQtMenuWindow(LumaHackQtKeyBuffer &);
    ~LumaHackQtMenuWindow();

    virtual QWidget *Widget();

    virtual void StartMenu();
    virtual void AddMenu(int glyph, const ANY_P *identifier, char ch,
                         char gch, int attr, const char *str, bool presel);
    virtual void EndMenu(const char *prompt);
    virtual int SelectMenu(int how, MENU_ITEM_P **menu_list);

  public slots:
    void All();
    void ChooseNone();
    void Invert();
    void Search();

    void Layout();
    void ToggleSelect(int);

  protected:
    virtual void keyPressEvent(QKeyEvent *);
    // virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void focusInEvent(QFocusEvent *);

    virtual void paintCell(QPainter *, int, int);
    virtual int cellWidth(int col);

  private:
    struct MenuItem {
        MenuItem();
        ~MenuItem();

        int glyph;
        ANY_P identifier;
        int attr;
        const char *str;
        int count;
        char ch;
        bool selected;

        bool
        Selectable() const
        {
            return identifier.a_void != 0;
        }
    };

    QArray<MenuItem> item;

    int itemcount;
    int str_width;
    bool str_fixed;
    int next_accel;

    LumaHackQtKeyBuffer &keysource;

    LumaHackQtMenuDialog *dialog;

    QPushButton *ok;
    QPushButton *cancel;
    QPushButton *all;
    QPushButton *none;
    QPushButton *invert;
    QPushButton *search;
    QLabel prompt;

    int how;

    bool has_glyphs;

    int pressed;
    bool was_sel;
};

class LumaHackQtTextListBox;

class LumaHackQtRIP : public QWidget
{
  private:
    static QPixmap *pixmap;
    char **line;
    int riplines;

  public:
    LumaHackQtRIP(QWidget *parent);

    void setLines(char **l, int n);

  protected:
    virtual void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;
};

class LumaHackQtTextWindow : public QDialog, public LumaHackQtWindow
{
    Q_OBJECT
  public:
    LumaHackQtTextWindow(LumaHackQtKeyBuffer &);
    ~LumaHackQtTextWindow();

    virtual QWidget *Widget();

    virtual void Clear();
    virtual bool Destroy();
    virtual void Display(bool block);
    virtual void PutStr(int attr, const char *text);
    virtual void UseRIP(int how, time_t when);

  public slots:
    void Search();

  protected:
    virtual void done(int);
    virtual void keyPressEvent(QKeyEvent *);

  private slots:
    void doUpdate();

  private:
    LumaHackQtKeyBuffer &keysource;

    bool use_rip;
    bool str_fixed;

    QPushButton ok;
    QPushButton search;
    LumaHackQtTextListBox *lines;

    LumaHackQtRIP rip;
};

class LumaHackQtMenuOrTextWindow : public LumaHackQtWindow
{
  private:
    LumaHackQtWindow *actual;
    LumaHackQtKeyBuffer &keysource;

  public:
    LumaHackQtMenuOrTextWindow(LumaHackQtKeyBuffer &);

    virtual QWidget *Widget();

    // Text
    virtual void Clear();
    virtual bool Destroy();
    virtual void Display(bool block);
    virtual void PutStr(int attr, const char *text);

    // Menu
    virtual void StartMenu();
    virtual void AddMenu(int glyph, const ANY_P *identifier, char ch,
                         char gch, int attr, const char *str, bool presel);
    virtual void EndMenu(const char *prompt);
    virtual int SelectMenu(int how, MENU_ITEM_P **menu_list);
};

class LumaHackQtDelay : QObject
{
  private:
    int msec;

  public:
    LumaHackQtDelay(int ms);
    void wait();
    virtual void timerEvent(QTimerEvent *timer);
};

class LumaHackQtInvUsageWindow : public QWidget
{
  public:
    LumaHackQtInvUsageWindow(QWidget *parent);
    virtual void paintEvent(QPaintEvent *);

  private:
    void drawWorn(QPainter &painter, obj *, int x, int y, bool canbe = TRUE);
};

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
#ifndef KDE
#include "qt_kde0.h"
#endif

class LumaHackQtMainWindow : public KTopLevelWidget
{
    Q_OBJECT
  public:
    LumaHackQtMainWindow(LumaHackQtKeyBuffer &);

    void AddMessageWindow(LumaHackQtMessageWindow *window);
    void AddMapWindow(LumaHackQtMapWindow *window);
    void AddStatusWindow(LumaHackQtStatusWindow *window);
    void RemoveWindow(LumaHackQtWindow *window);
    void updateInventory();

    void fadeHighlighting();

  public slots:
    void doMenuItem(int);
    void doKeys(const QString &);

  protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void closeEvent(QCloseEvent *);

  private slots:
    void layout();
    void raiseMap();
    void zoomMap();
    void raiseMessages();
    void raiseStatus();

  private:
    void ShowIfReady();

#ifdef KDE
    KMenuBar *menubar;
#else
    QMenuBar *menubar;
#endif
    LumaHackQtMessageWindow *message;
    LumaHackQtMapWindow *map;
    LumaHackQtStatusWindow *status;
    LumaHackQtInvUsageWindow *invusage;

    LumaHackQtKeyBuffer &keysink;
    QWidgetStack *stack;
    int dirkey;

    const char **macro;
};

class LumaHackQtYnDialog : QDialog
{
    Q_OBJECT
  private:
    const char *question;
    const char *choices;
    char def;
    LumaHackQtKeyBuffer &keysource;

  protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void done(int);

  private slots:
    void doneItem(int);

  public:
    LumaHackQtYnDialog(LumaHackQtKeyBuffer &keysource, const char *,
                      const char *, char);

    char Exec();
};

#ifdef KDE
#define LumaHackQtBindBase KApplication
#elif defined(QWS)
#define LumaHackQtBindBase QPEApplication
#else
#define LumaHackQtBindBase QApplication
#endif

class LumaHackQtBind : LumaHackQtBindBase
{
  private:
    // Single-instance preservation...
    LumaHackQtBind(int &argc, char **argv);

    static LumaHackQtBind *instance;

    static LumaHackQtKeyBuffer keybuffer;
    static LumaHackQtClickBuffer clickbuffer;

    static QWidget *splash;
    static LumaHackQtMainWindow *main;

  public:
    static void qt_init_nhwindows(int *argc, char **argv);
    static void qt_player_selection();
    static void qt_askname();
    static void qt_get_nh_event();
    static void qt_exit_nhwindows(const char *);
    static void qt_suspend_nhwindows(const char *);
    static void qt_resume_nhwindows();
    static winid qt_create_nhwindow(int type);
    static void qt_clear_nhwindow(winid wid);
    static void qt_display_nhwindow(winid wid, BOOLEAN_P block);
    static void qt_destroy_nhwindow(winid wid);
    static void qt_curs(winid wid, int x, int y);
    static void qt_putstr(winid wid, int attr, const char *text);
    static void qt_display_file(const char *filename, BOOLEAN_P must_exist);
    static void qt_start_menu(winid wid);
    static void qt_add_menu(winid wid, int glyph, const ANY_P *identifier,
                            CHAR_P ch, CHAR_P gch, int attr, const char *str,
                            BOOLEAN_P presel);
    static void qt_end_menu(winid wid, const char *prompt);
    static int qt_select_menu(winid wid, int how, MENU_ITEM_P **menu_list);
    static void qt_update_inventory();
    static void qt_mark_synch();
    static void qt_wait_synch();

    static void qt_cliparound(int x, int y);
    static void qt_cliparound_window(winid wid, int x, int y);
    static void qt_print_glyph(winid wid, XCHAR_P x, XCHAR_P y,
                               int glyph, int bkglyph);
    static void qt_raw_print(const char *str);
    static void qt_raw_print_bold(const char *str);
    static int qt_nhgetch();
    static int qt_nh_poskey(int *x, int *y, int *mod);
    static void qt_nhbell();
    static int qt_doprev_message();
    static char qt_yn_function(const char *question, const char *choices,
                               CHAR_P def);
    static void qt_getlin(const char *prompt, char *line);
    static int qt_get_ext_cmd();
    static void qt_number_pad(int);
    static void qt_delay_output();
    static void qt_start_screen();
    static void qt_end_screen();

    static void qt_outrip(winid wid, int how, time_t when);
    static int qt_kbhit();

  private:
    virtual bool notify(QObject *receiver, QEvent *event);
};

#endif
