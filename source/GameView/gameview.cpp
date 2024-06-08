//-- Description -------------------------------------------------------------/
// main gameview interface definitions, public method that modify or          /
// interract with the game state.                                             /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include "../HanoiStack/hanoistack.h"
#include "../Utils/utils.h"

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIO

#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QTimer>
#include <stdexcept>

GameView::GameView(QWidget* parent) : QWidget { parent }
{
    // init timer.
    // timer will call checkWinState every tick (should be every 1ms).
    connect(&m_time.timer, &QTimer::timeout, this, &GameView::checkWinState);

// load the placement sound effect
#ifndef DISABLE_AUDIO
    m_placement_fx = new QSoundEffect(this);
    m_placement_fx->setSource("qrc" + Config::get().AudioFiles().PLACEMENT_FX);
    m_placement_fx->setVolume(Config::get().Settings().fx_volume);
#endif

    m_sprites.arrow = QPixmap(Config::get().AssetFiles().ARROW);
    m_sprites.slice = QPixmap(Config::get().AssetFiles().SLICE);

    colorizeSprite(&m_sprites.arrow, Config::get().Theme().highlight_tint);
}

void
GameView::solve()
{
    if (m_game_state != GameState::Running || m_time.timer.isActive()) {
        return;
    }

    m_game_state = GameState::AutoSolving;

    m_time.timer.start(1);    // start timer

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

// (re)initialize an empty state
void
GameView::reset()
{
    clear();
    updateInfo();
#ifndef DISABLE_AUDIO
    m_placement_fx->setVolume(Config::get().Settings().fx_volume);
#endif
    m_game_state = GameState::Running;
    repaint();
}

void
GameView::clear()
{
    if (has_solver_task()) { stop_solver_task(); }

    m_time.elapsed = m_move_count = 0;

    m_game_state = GameState::NotRunning;
    m_time.timer.stop();

    const size_t goalStackLabel = getRandomGoalStackIndex();

    if (m_hanoi.slices_arr_size != Config::get().Settings().slice_amount) {
        if (m_hanoi.slices != nullptr) { delete[] m_hanoi.slices; }
        m_hanoi.slices = new HanoiSlice*[Config::get().Settings().slice_amount];
        m_hanoi.slices_arr_size = Config::get().Settings().slice_amount;
    }

    std::memset(m_hanoi.slices, 0, Config::get().Settings().slice_amount);

    if (m_hanoi.stacks_arr_size != Config::get().Settings().stack_amount) {
        // delete the stack first if already exists
        if (m_hanoi.stacks != nullptr) { delete[] m_hanoi.stacks; }

        // allocate space and save the size for comparison in the next reset
        m_hanoi.stacks = new HanoiStack[Config::get().Settings().stack_amount];
        m_hanoi.stacks_arr_size = Config::get().Settings().stack_amount;

        // initialize all possible stack
        for (int i = Config::get().Settings().stack_amount - 1; i >= 0; i--) {
            m_hanoi.stacks[i] = (HanoiStack(i));
        }
    } else if (m_hanoi.stacks_arr_size
               == Config::get().Settings().stack_amount) {
        // clear all stack
        for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
            m_hanoi.stacks[i].clearStack();
        }
    }

    assert(m_hanoi.stacks != nullptr);
    assert(m_hanoi.slices != nullptr);

    assert(m_hanoi.slices_arr_size == Config::get().Settings().slice_amount);
    assert(m_hanoi.stacks_arr_size == Config::get().Settings().stack_amount);

    m_hanoi.goal_stack = nullptr;
    m_hanoi.goal_stack = &m_hanoi.stacks[goalStackLabel];

    if (m_hanoi.goal_stack == nullptr
        || m_hanoi.goal_stack->label() != goalStackLabel) {
        std::runtime_error("clear(): failed to find goal stack");
    }

    calculateBaseSizes();

    m_sidebar_widgets.info_msg_out->setText(
        "Move All Slice to Stack "
        + Utils::numToChar(m_hanoi.goal_stack->label()));

    m_sidebar_widgets.info_msg_out->setAlignment(Qt::AlignCenter);

    if (Config::get().Settings().slice_amount <= 0) {
        std::runtime_error("clear(): invalid slice amount");
    }

    // populate the first stack
    HanoiStack::fillStack(&m_hanoi.stacks[0],
                          Config::get().Settings().slice_amount);

    if (m_hanoi.stacks[0].getSize() != Config::get().Settings().slice_amount) {
        std::runtime_error("clear(): failed to fill starting stack");
    }

    // tint the slice sprite
    if (m_sprites.slice_tint != Config::get().Theme().slice_tint) {
        QPixmap sprite(Config::get().AssetFiles().SLICE);
        colorizeSprite(&sprite, Config::get().Theme().slice_tint);
        m_sprites.slice      = sprite;
        m_sprites.slice_tint = Config::get().Theme().slice_tint;
    }

    HanoiSlice* slice = m_hanoi.stacks[0].getHead();
    while (slice != nullptr) {
        // save the slices for ease of access
        m_hanoi.slices[slice->getValue()] = slice;

        slice = slice->next;
    }

    // scale the stacks
    scaleStack();

    // scale the slices
    scaleSlices();
}
