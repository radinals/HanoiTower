#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "../Config/config.h"
#include "../HanoiStack/hanoistack.h"

#include <QCoreApplication>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <string>

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
    #include <QtMultimedia>
#endif

class GameView : public QWidget {
    Q_OBJECT

public:
    explicit GameView(QWidget *parent = nullptr);

    ~GameView() override
    {
        delete[] m_hanoi.stacks;
        delete[] m_hanoi.slices;
#ifndef DISABLE_AUDIO
        delete m_placement_fx;
#endif
    }

    void reset();    // reset the game states, and re-draw
    void clear();    // reset the game states
    void pause();    // halt the timer, inhibit input
    void solve();    // solve the game automaticly

    // used for checking GameView current status
    bool isPaused() { return m_game_state == GameState::Paused; }
    bool isAutoSolving() { return m_game_state == GameState::AutoSolving; }
    bool isTimerRunning() { return m_time.timer.isActive(); }

    // define pointers to the output widgets
    void setGameInfoOutputs(QLabel *time, QLabel *moves, QTextEdit *info_box)
    {
        m_sidebar_widgets.timer_out      = time;
        m_sidebar_widgets.move_count_out = moves;
        m_sidebar_widgets.info_msg_out   = info_box;
    }

private:
    // scale (down) of slices
    const float m_slice_scale_factor = 0.9f;
    size_t      m_move_count         = 0;

#ifndef DISABLE_AUDIO
    QSoundEffect *m_placement_fx = nullptr;
#endif

    struct GameSprites {
        QPixmap stack_pole, stack_base, arrow;
    } m_sprites;

    struct Geometry_t {
        QSizeF stack_area, stack_base, slice, dialog;
    } m_geometry;

    struct SelectedData_t {
        HanoiStack *stack = nullptr;    // source stack
        HanoiSlice *slice = nullptr;    // selected slice
        inline bool valid() { return stack != nullptr && slice != nullptr; }
    } m_selected;

    struct SidebarWidgets_t {
        QLabel    *timer_out = nullptr, *move_count_out = nullptr;
        QTextEdit *info_msg_out = nullptr;
    } m_sidebar_widgets;

    struct HanoiData_t {
        // all slices in game
        // LinkedList<HanoiSlice *> slices;
        HanoiSlice **slices = nullptr;

        // all stack in game
        HanoiStack *stacks = nullptr;

        // points to a stack in m_stack
        HanoiStack *goal_stack = nullptr;

    } m_hanoi;

    struct TimeInfo_t {
        QTimer        timer;
        long long int elapsed = 0;    // ms
    } m_time;

    enum class GameState {
        NotRunning,
        Running,
        Paused,
        GameOverLost,
        GameOverWon,
        AutoSolving,
    };

    GameState m_game_state = GameState::NotRunning;

    // calculate click area, returns stack under click
    HanoiStack *calculateStackByPos(const QPointF &);

    // calculate the base sizes
    void calculateBaseSizes();

    // updates sidebar values
    void updateInfo();

    // get pointer to stack of 'label'
    HanoiStack *getStack(size_t label);

    // handles drawing/rendering
    void drawStack(float offset, HanoiStack *, QPainter *p);
    void drawStackBase(size_t label, float offset, QPainter *painter);
    void drawDialog(const QString  &text,
                    const QColor   &color,
                    QPainter *const painter);

    void scaleSlices();    // handles slice scaling

    // basic auto-solving mode (NOT THREAD SAFE)
    void hanoiIterativeSolver();

    // QWidget Event Handlers
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

    // tints pixmaps
    static void colorizeSprite(QPixmap *, const QColor &);

    // move/pop the top slice of a stack
    static void moveTopSlice(HanoiStack *source, HanoiStack *dest);

    // generate random stack index from 1 to n-1
    static size_t getRandomGoalStackIndex();

    // convert the numeric labels of stacks to alphabets
    inline static QString numToChar(size_t n)
    {
        std::string str;
        str += char('A' + n);
        return QString::fromStdString(str);
    };

    // move slice stored by m_selected, to the QPoints x and y values
    inline void moveSelectedSlice(const QPoint &p)
    {
        m_selected.slice->Geometry().x
            = (p.x() - (m_selected.slice->Geometry().width * 0.5f));
        m_selected.slice->Geometry().y
            = (p.y() - (m_selected.slice->Geometry().height * 0.5f));
        update();
    }

    // check if the goal stack has all valid slices in it
    inline bool goalStackIsComplete()
    {
        return (m_hanoi.goal_stack->getSize()
                == Config::get().Settings().slice_amount);
    }

    // overloaded game movement rule check,
    // this checks the slice in m_selected
    inline bool moveIsValid(HanoiStack *&dest)
    {
        return (dest != nullptr)
               && (dest->isEmpty()
                   || m_selected.slice->getValue() > dest->peek()->getValue());
    }

    // overloaded game movement rule check,
    // this checks the top slice in two stacks
    static inline bool moveIsValid(HanoiStack *source, HanoiStack *dest)
    {
        return (!source->isEmpty())
               && (dest->isEmpty()
                   || source->peek()->getValue() > dest->peek()->getValue());
    }

    // delay by miliseconds
    static void delay(int ms)
    {
        QTime dieTime = QTime::currentTime().addMSecs(ms);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

private slots:
    // called by timer in every ms
    void checkWinState();

signals:
};

#endif    // GAMEVIEW_H
