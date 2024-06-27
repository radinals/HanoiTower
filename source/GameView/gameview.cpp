//-- Description -------------------------------------------------------------/
// main gameview interface definitions, public method that modify or          /
// interract with the game state.                                             /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIO

#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QTimer>

QTimer GameView::TimeInfo::timer = QTimer();

GameView::GameView(QWidget *parent) : QWidget { parent }
{
    // init timer.
    // timer will call checkWinState every tick (should be every 1ms).
    connect(&TimeInfo::timer, &QTimer::timeout, this, &GameView::checkWinState);

    for (size_t i = 0; i < Config::STACK_MAX; i++) {
        HanoiStacks::stacks[i] = HanoiStack(i);
    }

// load the placement sound effect
#ifndef DISABLE_AUDIO
    m_placement_fx = new QSoundEffect(this);
    m_placement_fx->setSource("qrc" + Config::AudioFiles::PLACEMENT_FX);
    m_placement_fx->setVolume(Config::Settings::fx_volume);
    connect(this, &GameView::s_slice_moved, [&]() { m_placement_fx->play(); });
    assert(m_placement_fx->status() != QSoundEffect::Error);
#endif

    GameSprites::stack_pole = new QPixmap();
    GameSprites::stack_base = new QPixmap();
    GameSprites::arrow      = new QPixmap();
    GameSprites::slice      = new QPixmap();

    // load arrow sprite ====================================================
    GameSprites::arrow->load(Config::AssetsFiles::ARROW);
    assert(!GameSprites::arrow->isNull());
    colorizeSprite(GameSprites::arrow, Config::Theme().highlight_tint);

    // load stack sprites ===================================================
    GameSprites::stack_pole->load(Config::AssetsFiles::STACK_POLE);
    GameSprites::stack_base->load(Config::AssetsFiles::STACK_BASE);

    colorizeSprite(GameSprites::stack_base, Config::Theme::stack_tint);
    colorizeSprite(GameSprites::stack_pole, Config::Theme::stack_tint);
    GameSprites::stack_tint = Config::Theme::stack_tint;
}

GameView::~GameView()
{
    if (has_solver_task()) { stop_solver_task(); }
#ifndef DISABLE_AUDIO
    delete m_placement_fx;
#endif
    delete GameSprites::stack_pole;
    delete GameSprites::stack_base;
    delete GameSprites::arrow;
    delete GameSprites::slice;
}

GameView *const &
GameView::getInstance()
{
    static GameView *G = new GameView();
    return G;
};

void
GameView::destroyInstance()
{
    delete getInstance();
};

void
GameView::solve()
{
    if (has_solver_task()) { return; }

    emit(s_solver_activated());

    // reset the stacks first
    clear();

    // stop the timer if active
    if (TimeInfo::timer.isActive()) { TimeInfo::timer.stop(); }

    // set state to be running
    m_game_state = GameState::GAME_RUNNING;

    repaint();    // repaint first

    start_solver_task();    // start the solver in a new thread
}

void
GameView::pause()
{
    switch (m_game_state) {
        case GameState::GAME_PAUSED:
            TimeInfo::timer.start(1);
            updateInfo();
            if (has_paused_solver_task()) { unpause_solver_task(); }
            m_game_state = GameState::GAME_RUNNING;
            emit(s_unpaused());
            break;
        case GameState::GAME_RUNNING:
            if (has_solver_task()) { pause_solver_task(); }
            if (!TimeInfo::timer.isActive()) { return; }
            TimeInfo::timer.stop();
            m_game_state = GameState::GAME_PAUSED;
            emit(s_paused());
            updateInfo();
            break;
        default:
            return;
    }
}

// reset all states
void
GameView::reset()
{
    // stop the solver process if present
    if (has_solver_task()) { stop_solver_task(); }

    // reset some states
    TimeInfo::elapsed = 0;
    m_move_count      = 0;

    m_redo_history.clear();
    m_redo_history.clear();

    // stop the timer (if any)
    TimeInfo::timer.stop();

    // clear the stacks, and resize if needed
    clear();

    // set the goal stack
    setGoalStack();

    // update the sidebar
    updateInfo();

#ifndef DISABLE_AUDIO
    // set the fx volume
    m_placement_fx->setVolume(Config::Settings::fx_volume);
#endif

    m_game_state = GameState::GAME_RUNNING;

    emit(s_game_inactive());

    repaint();
}

void
GameView::undo()
{
    if (m_move_history.isEmpty()
        || (m_game_state != GameState::GAME_RUNNING
            && m_game_state != GameState::GAME_PAUSED)) {
        return;
    }

    std::pair<HanoiStack *, HanoiStack *> move = m_move_history.getTop();

    try {
        move.first->push(move.second->pop());
    } catch (...) {
        return;
    }

    --m_move_count;

    repaint();

    m_redo_history.push(move);

    m_move_history.pop();
}

void
GameView::redo()
{
    if (m_redo_history.isEmpty()
        || (m_game_state != GameState::GAME_RUNNING
            && m_game_state != GameState::GAME_PAUSED)) {
        return;
    }

    std::pair<HanoiStack *, HanoiStack *> move = m_redo_history.getTop();

    try {
        move.second->push(move.first->pop());
    } catch (...) {
        return;
    }

    ++m_move_count;

    repaint();

    m_move_history.push(move);

    m_redo_history.pop();
}
