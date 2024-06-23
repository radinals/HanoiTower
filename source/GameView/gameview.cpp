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

GameView::GameView(QWidget* parent) : QWidget { parent }
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

    m_sprites.arrow = QPixmap(Config::AssetsFiles::ARROW);

    assert(!m_sprites.arrow.isNull());

    colorizeSprite(&m_sprites.arrow, Config::Theme().highlight_tint);
}

void
GameView::solve()
{
    if (has_solver_task()) { return; }

    emit(s_solver_activated());

    // kinda cheating here tbh...
    clear();

    m_game_state = GameState::AutoSolving;

    TimeInfo::timer.start(
        1);    // start timer (helps with the winstate checking)

    repaint();    // repaint first

    start_solver_task();    // start the solver in a new thread
}

void
GameView::pause()
{
    switch (m_game_state) {
        case GameState::Paused:
            TimeInfo::timer.start(1);
            updateInfo();
            if (has_paused_solver_task()) {
                unpause_solver_task();
                m_game_state = GameState::AutoSolving;
            } else {
                m_game_state = GameState::Running;
            }
            emit(s_unpaused());
            break;
        case GameState::AutoSolving:
            pause_solver_task();
        case GameState::Running:
            if (!TimeInfo::timer.isActive()) { return; }
            TimeInfo::timer.stop();
            m_game_state = GameState::Paused;
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

    m_game_state = GameState::Running;

    emit(s_game_inactive());

    repaint();
}
