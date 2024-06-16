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
    connect(&m_time.timer, &QTimer::timeout, this, &GameView::checkWinState);

    for (size_t i = 0; i < Config::STACK_MAX; i++) {
        m_hanoi.stacks[i] = HanoiStack(i);
    }

// load the placement sound effect
#ifndef DISABLE_AUDIO
    m_placement_fx = new QSoundEffect(this);
    m_placement_fx->setSource("qrc" + Config::get().AudioFiles().PLACEMENT_FX);
    m_placement_fx->setVolume(Config::get().Settings().fx_volume);
#endif

    m_sprites.arrow = QPixmap(Config::get().AssetFiles().ARROW);

    if (m_sprites.arrow.isNull()) {
        throw std::runtime_error(
            "GameView::GameView(): Failed to load Arrow Sprite");
    }

    colorizeSprite(&m_sprites.arrow, Config::get().Theme().highlight_tint);
}

void
GameView::solve()
{
    if (has_solver_task()) {
        emit(s_solver_is_already_running());
        return;
    }

    // kinda cheating here tbh...
    clear();

    m_game_state = GameState::AutoSolving;

    m_time.timer.start(1);    // start timer (helps with the winstate checking)

    repaint();    // repaint first

    start_solver_task();    // start the solver in a new thread
}

void
GameView::pause()
{
    switch (m_game_state) {
        case GameState::Paused:
            m_time.timer.start(1);
            updateInfo();
            if (has_paused_solver_task()) {
                unpause_solver_task();
                m_game_state = GameState::AutoSolving;
            } else {
                m_game_state = GameState::Running;
            }
            break;
        case GameState::AutoSolving:
            pause_solver_task();
        case GameState::Running:
            if (!m_time.timer.isActive()) { return; }
            m_sidebar_widgets.timer_out->setText("PAUSED");
            m_time.timer.stop();
            m_game_state = GameState::Paused;
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
    m_time.elapsed = m_move_count = 0;

    // stop the timer (if any)
    m_time.timer.stop();

    // clear the stacks, and resize if needed
    clear();

    // set the goal stack
    setGoalStack();

    // update the sidebar
    updateInfo();

#ifndef DISABLE_AUDIO
    // set the fx volume
    m_placement_fx->setVolume(Config::get().Settings().fx_volume);
#endif

    m_game_state = GameState::Running;

    repaint();
}
