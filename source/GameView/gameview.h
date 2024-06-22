//-- Description -------------------------------------------------------------/
// The Hanoi Tower Widget                                                     /
//----------------------------------------------------------------------------/

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "../Config/config.h"
#include "../HanoiStack/hanoistack.h"

#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTime>
#include <QWidget>
#include <atomic>
#include <qpushbutton.h>
#include <thread>

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
    #include <QtMultimedia>
#endif

class GameView : public QWidget {
    Q_OBJECT

public slots:
    void reset();    // reset the game states, and re-draw
    void pause();    // halt the timer, inhibit input
    void solve();    // solve the game automaticly

public:
    explicit GameView(QWidget *parent = nullptr);

    ~GameView() override
    {
        if (m_game_state == GameState::AutoSolving) { stop_solver_task(); }
#ifndef DISABLE_AUDIO
        delete m_placement_fx;
#endif
    }

    // define pointers to the output widgets
    void setGameInfoOutputs(QPushButton *time,
                            QLabel      *moves,
                            QLabel      *info_box_label,
                            QTextEdit   *info_box);

private slots:
    // called by timer in every ms
    void checkWinState();

private:
    size_t m_move_count = 0;

#ifndef DISABLE_AUDIO
    QSoundEffect *m_placement_fx = nullptr;
#endif

    struct GameSprites_t {
        QColor  stack_tint, slice_tint;
        QPixmap stack_pole, stack_base, arrow, slice;
    } m_sprites;

    struct Geometry_t {
        QSizeF stack_area, stack_base, slice, dialog;
    } m_geometry;

    struct SelectedSlice {
        HanoiStack *stack = nullptr;    // source stack
        HanoiSlice *slice = nullptr;    // selected slice

        inline bool hasSelected()
        {
            return stack != nullptr && slice != nullptr;
        }
    } m_selected;

    struct SidebarWidgets_t {
        QLabel      *move_count_out = nullptr, *info_msg_label = nullptr;
        QPushButton *timer_out    = nullptr;
        QTextEdit   *info_msg_out = nullptr;
    } m_sidebar_widgets;

    struct HanoiData_t {
        // all slices in game
        // LinkedList<HanoiSlice *> slices;
        static inline HanoiSlice *slices[Config::SLICE_MAX];

        // all stack in game
        static inline HanoiStack stacks[Config::STACK_MAX];

        // points to a stack in m_stack
        static inline HanoiStack *goal_stack = nullptr;

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

    struct SolverTask_t {
        std::atomic_bool stop_solving  = false;
        std::atomic_bool pause_solving = false;
        std::thread     *work_thread   = nullptr;
    } m_solver_task;

    void clear();    // reset the game states

    bool has_solver_task();
    bool has_paused_solver_task();

    void stop_solver_task();
    void start_solver_task();
    void pause_solver_task();
    void unpause_solver_task();

    void resetStacks();
    void resetSlices();
    void setGoalStack();

    // calculate click area, returns stack under click
    HanoiStack *calculateStackByPos(const QPointF &);

    // calculate the base sizes
    void calculateBaseSizes();

    // updates sidebar values
    void updateInfo();

    // get pointer to stack of 'label'
    HanoiStack *getStack(size_t label);

    // handles drawing/rendering
    void drawStack(float offset, HanoiStack *const, QPainter *const p);
    void drawStackBase(size_t label, float offset, QPainter *const painter);
    void drawDialog(const QString  &text,
                    const QColor   &color,
                    QPainter *const painter);

    void scaleSlices();    // handles slice scaling
    void scaleStack();     // handles stack scaling

    void hanoiIterativeSolver();

    // QWidget Event Handlers

    // input event
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    // render events
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

    // widget state event
    void hideEvent(QHideEvent *) override;
    void showEvent(QShowEvent *) override;

    // tints pixmaps
    static void colorizeSprite(QPixmap *const, const QColor &);

    // move/pop the top slice of a stack
    static void makeLegalMove(HanoiStack *const source, HanoiStack *const dest);

    // generate random stack index from 1 to n-1
    static size_t getRandomGoalStackIndex();

    // move slice stored by m_selected, to the QPoints x and y values
    void moveSelectedSlice(const QPoint &p);

    // check if the goal stack has all valid slices in it
    bool goalStackIsComplete();

signals:
    void s_hidden();
    void s_paused();
    void s_unpaused();
    void s_solver_activated();
    void s_solver_exited();
    void s_game_inactive();
    void s_game_started();
    void s_game_over();
};

#endif    // GAMEVIEW_H
