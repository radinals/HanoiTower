#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "config.h"
#include "hanoistack.h"
#include "linkedlist.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <QtMultimedia>
#include <qaccessible_base.h>
#include <qaudiooutput.h>
#include <qmainwindow.h>
#include <qmediaplayer.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qsoundeffect.h>
#include <string>
#include <utility>

class GameView : public QWidget {
    Q_OBJECT

private:
    const float   m_slice_scale_factor = 0.9f;
    unsigned int  m_move_count         = 0;
    QSoundEffect *m_placement_fx       = nullptr;

    struct GameSprites {
        QPixmap stack_pole, stack_base, arrow;
    } m_sprites;

    struct GameGeometry_t {
        QSizeF stack_area, stack_base, slice, dialog;
    } m_sizes;

    struct SelectedData_t {
        HanoiStack *stack = nullptr;    // source stack
        HanoiSlice *slice = nullptr;    // selected slice
        inline bool valid() { return stack != nullptr && slice != nullptr; }
    } m_selected;

    struct SidebarWidgets_t {
        QLabel    *timer_out = nullptr, *move_count_out = nullptr;
        QTextEdit *info_msg_out = nullptr;
    } m_sidebar_widgets;

    struct HanoiStackData_t {
        // all slices in game
        LinkedList<HanoiSlice *> slices;

        // all stack in game
        LinkedList<std::pair<size_t, HanoiStack>> stacks;

        // points to a stack in m_stack
        std::pair<size_t, HanoiStack> *goal_stack = nullptr;

    } m_stack_data;

    struct TimeInfo_t {
        QTimer                 timer;
        unsigned long long int elapsed = 0;    // ms
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

    LinkedList<std::pair<long int, long int>> m_solving_moves;

public:
    explicit GameView(QWidget *parent = nullptr);
    ~GameView() override { delete m_placement_fx; }
    // ~GameView() override {};

    void reset();    // reset the game states, and re-draw
    void clear();    // reset the game states
    void pause();    // halt the timer, inhibit input
    void solve();    // solve the game automaticly

    bool isPaused() { return m_game_state == GameState::Paused; }
    bool isAutoSolving() { return m_game_state == GameState::AutoSolving; }
    bool isTimerRunning() { return m_time.timer.isActive(); }

    void setGameInfoOutputs(QLabel *time, QLabel *moves, QTextEdit *info_box)
    {
        m_sidebar_widgets.timer_out      = time;
        m_sidebar_widgets.move_count_out = moves;
        m_sidebar_widgets.info_msg_out   = info_box;
    }

private slots:
    void checkWinState();

private:
    inline static QString numToChar(size_t n)
    {
        std::string str;
        str += char('A' + n);
        return QString::fromStdString(str);
    };

    inline void moveSelectedSlice(const QPoint &p)
    {
        m_selected.slice->Geometry().x
            = (p.x() - (m_selected.slice->Geometry().width * 0.5f));
        m_selected.slice->Geometry().y
            = (p.y() - (m_selected.slice->Geometry().height * 0.5f));
        update();
    }

    inline bool goalStackIsComplete()
    {
        return (m_stack_data.goal_stack->second.getSize()
                == Config::get().Settings().slice_amount);
    }

    inline bool moveIsValid(const std::pair<size_t, HanoiStack *> &dest)
    {
        return (dest.second != nullptr)
               && dest.first < Config::get().Settings().stack_amount
               && (dest.second->isEmpty()
                   || m_selected.slice->getValue()
                          > dest.second->peek()->getValue());
    }

    static inline bool moveIsValid(HanoiStack *source, HanoiStack *dest)
    {
        return (!source->isEmpty())
               && (dest->isEmpty()
                   || source->peek()->getValue() > dest->peek()->getValue());
    }

    static void delay(int sec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(sec);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    void drawDialog(const QString  &text,
                    const QColor   &color,
                    QPainter *const painter);

    void          hanoiIterativeSolver();
    static size_t getRandomGoalStackIndex();

    void updateInfo();

    static void colorizeSprite(QPixmap *, const QColor &);

    HanoiStack *getStack(size_t label);
    static void moveTopSlice(HanoiStack *source, HanoiStack *dest);
    std::pair<size_t, HanoiStack *> calculateStackByPos(const QPointF &);

    void calculateBaseSizes();
    void drawStack(float offset, HanoiStack *, QPainter *p);
    void drawStackBase(size_t label, float offset, QPainter *painter);
    void scaleSlices();

    // QWidget Event Handlers
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

signals:
};

#endif    // GAMEVIEW_H
