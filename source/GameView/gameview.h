#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "../HanoiStack/hanoistack.h"

#include <QCoreApplication>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <atomic>
#include <thread>

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
        if (m_game_state == GameState::AutoSolving) { stop_solver_task(); }
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

    struct SelectedData_t {
        HanoiStack *stack = nullptr;    // source stack
        HanoiSlice *slice = nullptr;    // selected slice
        inline bool hasSelected()
        {
            return stack != nullptr && slice != nullptr;
        }
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

    struct SolverTask_t {
        std::atomic_bool stop_solving  = false;
        std::atomic_bool pause_solving = false;
        std::thread     *work_thread   = nullptr;
        SolverTask_t() {};
    } m_solver_task;

    bool has_solver_task();
    bool has_paused_solver_task();

    void stop_solver_task();
    void start_solver_task();
    void pause_solver_task();
    void unpause_solver_task();

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
    void scaleStack();     // handles stack scaling

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
    static QString numToChar(size_t);

    // move slice stored by m_selected, to the QPoints x and y values
    void moveSelectedSlice(const QPoint &p);

    // check if the goal stack has all valid slices in it
    bool goalStackIsComplete();

    // overloaded game movement rule check,
    // this checks the slice in m_selected
    bool moveIsValid(HanoiStack *&dest);

    // overloaded game movement rule check,
    // this checks the top slice in two stacks
    static bool moveIsValid(HanoiStack *source, HanoiStack *dest);

private slots:
    // called by timer in every ms
    void checkWinState();

signals:
};

#endif    // GAMEVIEW_H
