#include "gameview.h"

#include "../Config/config.h"
#include "../HanoiStack/hanoistack.h"

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
    // allocate spaces for all possible slices and stacks
    m_hanoi.slices = new HanoiSlice*[Config::get().Settings().SLICE_MAX];
    m_hanoi.stacks = new HanoiStack[Config::get().Settings().STACK_MAX];

    // initialize all possible stack
    for (int i = Config::get().Settings().STACK_MAX - 1; i >= 0; i--) {
        m_hanoi.stacks[i] = (HanoiStack(i));
    }

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
    calculateBaseSizes();

    if (has_solver_task()) { stop_solver_task(); }

    m_time.elapsed = m_move_count = 0;

    m_game_state = GameState::NotRunning;
    m_time.timer.stop();

    std::memset(m_hanoi.slices, 0, Config::get().Settings().SLICE_MAX);

    const size_t goalStackLabel = getRandomGoalStackIndex();

    m_hanoi.goal_stack = nullptr;

    // clear all stack
    // search for the goal stack node
    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        if (m_hanoi.stacks[i].label() == goalStackLabel) {
            m_hanoi.goal_stack = &m_hanoi.stacks[i];
        }
        m_hanoi.stacks[i].clearStack();
    }

    if (m_hanoi.goal_stack == nullptr
        || m_hanoi.goal_stack->label() != goalStackLabel) {
        std::runtime_error("clear(): failed to find goal stack");
    }

    m_sidebar_widgets.info_msg_out->setText(
        "Move All Slice to Stack " + numToChar(m_hanoi.goal_stack->label()));

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

void
GameView::resizeEvent(QResizeEvent* event)
{
    calculateBaseSizes();
    if (m_game_state != GameState::NotRunning) {
        scaleStack();
        scaleSlices();
    }
}

void
GameView::paintEvent(QPaintEvent* event)
{
    if (m_game_state == GameState::NotRunning) return;

    QPainter p(this);

    updateInfo();

    float offset = m_geometry.stack_area.width() * 0.5f;

    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        HanoiStack& current_stack = m_hanoi.stacks[i];

        // draw the stack base
        drawStackBase(current_stack.label(), offset, &p);

        // render the stack
        if (!current_stack.isEmpty()) { drawStack(offset, &current_stack, &p); }

        // shift to the right for the next stack
        offset += m_geometry.stack_area.width();
    }

    // render the selected stack
    if (m_selected.hasSelected()) {
        p.drawPixmap(
            m_selected.slice->Geometry().x,
            m_selected.slice->Geometry().y,
            m_sprites.slice.scaled(m_selected.slice->Geometry().width,
                                   m_selected.slice->Geometry().height));
    }

    switch (m_game_state) {
        case GameState::GameOverLost:
            drawDialog("TIME's UP!",
                       Config::get().Theme().lose_dialog_tint,
                       &p);
            break;

        case GameState::GameOverWon:
            drawDialog("YOU WIN", Config::get().Theme().win_dialog_tint, &p);
            break;

        default:
            break;
    }
}
