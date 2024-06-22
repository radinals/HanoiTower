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
    static inline size_t m_move_count = 0;

#ifndef DISABLE_AUDIO
    QSoundEffect *m_placement_fx = nullptr;
#endif

    struct GameSprites {
        QColor  stack_tint, slice_tint;
        QPixmap stack_pole, stack_base, arrow, slice;
    } m_sprites;

    struct Geometry {
        static inline QSizeF stack_area, stack_base, slice, dialog;
    };

    struct SelectedSlice {
        static inline HanoiStack *stack = nullptr;    // source stack
        static inline HanoiSlice *slice = nullptr;    // selected slice

        static inline bool hasSelected()
        {
            return stack != nullptr && slice != nullptr;
        }
    };

    struct SidebarWidgets {
        static inline QLabel *move_count_out    = nullptr,
                             *info_msg_label    = nullptr;
        static inline QPushButton *timer_out    = nullptr;
        static inline QTextEdit   *info_msg_out = nullptr;
    };

    struct HanoiStacks {
        // all slices in game
        static inline HanoiSlice *slices[Config::SLICE_MAX];

        // all stack in game
        static inline HanoiStack stacks[Config::STACK_MAX];

        // points to the target stack in the game
        static inline HanoiStack *goal_stack = nullptr;
    };

    struct TimeInfo {
        static inline QTimer        timer;
        static inline long long int elapsed = 0;    // ms
    };

    enum class GameState {
        NotRunning,
        Running,
        Paused,
        GameOverLost,
        GameOverWon,
        AutoSolving,
    };

    GameState m_game_state = GameState::NotRunning;

    struct SolverTask {
        static inline std::atomic_bool stop_solving  = false;
        static inline std::atomic_bool pause_solving = false;
        static inline std::thread     *work_thread   = nullptr;
    };

    void clear();    // reset the game states

    static bool has_solver_task();
    static bool has_paused_solver_task();

    void        start_solver_task();
    static void stop_solver_task();
    static void pause_solver_task();
    static void unpause_solver_task();

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
    static HanoiStack *getStack(size_t label);

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
    void mousePressEvent(QMouseEvent *const) override;
    void mouseReleaseEvent(QMouseEvent *const) override;
    void mouseMoveEvent(QMouseEvent *const) override;

    // render events
    void paintEvent(QPaintEvent *const) override;
    void resizeEvent(QResizeEvent *const) override;

    // widget state event
    void hideEvent(QHideEvent *const) override;
    void showEvent(QShowEvent *const) override;

    // tints pixmaps
    static void colorizeSprite(QPixmap *const, const QColor &);

    // move/pop the top slice of a stack
    static void makeLegalMove(HanoiStack *const source, HanoiStack *const dest);

    // generate random stack index from 1 to n-1
    static size_t getRandomGoalStackIndex();

    // move slice stored by m_selected, to the QPoints x and y values
    static void moveSelectedSlice(const QPoint &p);

    // check if the goal stack has all valid slices in it
    static bool goalStackIsComplete();

    inline static bool moveisLegal(const HanoiStack &source,
                                   const HanoiStack &dest);

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
