//-- Description -------------------------------------------------------------/
// The Hanoi Tower Widget                                                     /
//----------------------------------------------------------------------------/

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "../Config/config.h"
#include "../HanoiStack/hanoistack.h"
#include "../Utils/Stack.h"

#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <atomic>
#include <thread>
#include <utility>

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
    #include <QtMultimedia>
#endif

class GameView : public QWidget {
    Q_OBJECT

public slots:
    // reset the game states, and re-draw
    void reset();

    // halt the timer, inhibit input
    void pause();

    // solve the game automaticly
    void solve();

    void undo();

    void redo();

private:
    explicit GameView(QWidget *parent = nullptr);

public:
    GameView(GameView &&) = delete;

    // create & return gameview instance
    static GameView *const &getInstance();

    // destroy gameview instance
    static void destroyInstance();

    ~GameView() override;

    // define pointers to the output widgets
    static void
    setSidebarWidget(QPushButton *, QLabel *, QLabel *, QTextEdit *);

private slots:
    // called by timer in every ms
    void checkWinState();

private:
    static inline size_t m_move_count = 0;

#ifndef DISABLE_AUDIO
    QSoundEffect *m_placement_fx = nullptr;
#endif

    // =======================================================================

    // Stores game sprites
    struct GameSprites {
        static inline QColor   stack_tint, slice_tint;
        static inline QPixmap *stack_pole = nullptr, *stack_base = nullptr,
                              *arrow = nullptr, *slice = nullptr;
    };

    // =======================================================================

    // Stores game object sizes
    struct Geometry {
        static inline QSizeF stack_area, stack_base, slice, dialog, window,
            stack_pole;
    };

    // =======================================================================

    // Stores the current slice and it's source stack selected
    struct SelectedSlice {
        static inline HanoiStack *stack = nullptr;    // source stack
        static inline HanoiSlice *slice = nullptr;    // selected slice
        static inline float       x = 0, y = 0;

        static inline bool hasSelected()
        {
            return stack != nullptr && slice != nullptr;
        }

        // move slice stored by SelectedSlice:: to the QPoints x and y values
        static inline void move(const QPoint &point)
        {
            x = (point.x() - (SelectedSlice::slice->Width() * 0.5F));
            y = (point.y() - (SelectedSlice::slice->Height() * 0.5F));
        }
    };

    // =======================================================================

    // Stores the Sidebar Widget instances
    struct SidebarWidgets {
        static inline QLabel *move_count_out    = nullptr,
                             *info_msg_label    = nullptr;
        static inline QPushButton *timer_out    = nullptr;
        static inline QTextEdit   *info_msg_out = nullptr;
    };

    // =======================================================================

    // Stores the stacks and slices of the game
    struct HanoiStacks {
        // all slices in game
        static inline HanoiSlice *slices[Config::SLICE_MAX];

        // all stack in game
        static inline HanoiStack stacks[Config::STACK_MAX];

        // points to the target stack in the game
        static inline HanoiStack *goal_stack = nullptr;
    };

    // =======================================================================

    // Stores the QTimer instances and time elapsed
    struct TimeInfo {
        static QTimer               timer;
        static inline long long int elapsed = 0;    // ms
    };

    // =======================================================================

    enum class GameState {
        GAME_INACTIVE,
        GAME_RUNNING,
        GAME_PAUSED,
        GAME_OVER_LOST,
        GAME_OVER_WON,
        GAME_OVER_SOLVER_DONE,
    };

    // Stores the current game state
    static inline GameState m_game_state = GameState::GAME_INACTIVE;

    static inline Stack<std::pair<HanoiStack *, HanoiStack *>> m_move_history;
    static inline Stack<std::pair<HanoiStack *, HanoiStack *>> m_redo_history;

    // =======================================================================

    // Stores the Solver Task thread instance and state
    struct SolverTask {
        static inline std::atomic_bool stop_solving  = false;
        static inline std::atomic_bool pause_solving = false;
        static inline std::thread     *work_thread   = nullptr;
    };

    // Handles Solver Thread ================================================

    // hanoi tower puzzle solver
    void hanoiIterativeSolver();

    // runs the solver on a different thread
    void start_solver_task();

    // check if a task is already running
    static bool has_solver_task();

    // check if the solver task is paused
    static bool has_paused_solver_task();

    // halt the solver loop, and return from thread
    static void stop_solver_task();

    // halt the solver loop
    static void pause_solver_task();

    // un-halt the solver loop
    static void unpause_solver_task();

    // Reset =================================================================

    // clear & reset the stacks & slices
    void clear();    // reset the game states

    // clear & reset all stack
    static void resetStacks();

    // clear & reset all stack
    static void resetSlices();

    // generate a random goal stack
    static void setGoalStack();

    // Scaling ===============================================================

    // calculate the base sizes
    void calculateBaseSizes();

    // handles slice scaling
    static void scaleSlices();

    // handles stack scaling
    static void scaleStack();

    // Drawing/Rendering =====================================================

    // draws a single stack and also it's slices
    static void drawStack(float, HanoiStack *, QPainter *const);

    // draw the stack base/background
    static void drawStackBase(size_t, float, QPainter *const);

    // draw a dialog sprite in the center of the screen
    static void drawDialog(const QString &, const QColor &, QPainter *const);

    // Input Event ===========================================================

    // on mouse press
    void mousePressEvent(QMouseEvent *const) override;

    // on mouse press release
    void mouseReleaseEvent(QMouseEvent *const) override;

    // on mouse movement
    void mouseMoveEvent(QMouseEvent *const) override;

    // Widget events =========================================================

    // widget repaint event
    void paintEvent(QPaintEvent *const) override;

    // widget resize event
    void resizeEvent(QResizeEvent *const) override;

    // hide event
    void hideEvent(QHideEvent *const) override;

    // show/unhide event
    void showEvent(QShowEvent *const) override;

    // =======================================================================

    static bool clickInBounds(const QPoint &);

    // calculate click area, returns stack under click
    static HanoiStack *calculateStackByPos(const QPointF &);

    // updates sidebar values
    static void updateInfo();

    // get pointer to stack of 'label'
    static HanoiStack *getStack(size_t label);

    // tints pixmaps
    static void colorizeSprite(QPixmap *const, const QColor &);

    // move/pop the top slice of a stack
    static void makeLegalMove(HanoiStack *const source, HanoiStack *const dest);

    // generate random stack index from 1 to n-1
    static size_t getRandomGoalStackIndex();

    // check if the goal stack has all valid slices in it
    static bool goalStackIsComplete();

    // check if a move from 'source' to 'dest' stack is possible
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
    void s_slice_moved();
};

#endif    // GAMEVIEW_H
