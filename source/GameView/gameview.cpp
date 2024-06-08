#include "gameview.h"

#include "../Config/config.h"
#include "../HanoiStack/hanoistack.h"
#include "../Utils/utils.h"

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIO

#include <QFont>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QTimer>
#include <chrono>
#include <cmath>
#include <random>
#include <stdexcept>
#include <thread>

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
GameView::hanoiIterativeSolver()
{
    const size_t n = Config::get().Settings().slice_amount;

    size_t possible_moves = (1 << n) - 1;

    size_t source = 0,
           aux    = ((m_hanoi.goal_stack->label() == 1)
                         ? m_hanoi.goal_stack->label() + 1
                         : 1),
           dest   = m_hanoi.goal_stack->label();

    if (n % 2 == 0) {
        size_t tmp;
        tmp  = dest;
        dest = aux;
        aux  = tmp;
    }

    for (int i = 1; i <= possible_moves && !m_solver_task.stop_solving; i++) {
        {
            while (m_solver_task.pause_solving) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (m_solver_task.stop_solving) { return; }
            }
        }

        if (i % 3 == 0) {
            moveTopSlice(getStack(aux), getStack(dest));
        } else if (i % 3 == 1) {
            moveTopSlice(getStack(source), getStack(dest));
        } else {
            moveTopSlice(getStack(source), getStack(aux));
        }

        ++m_move_count;

        QMetaObject::invokeMethod(this, "repaint", Qt::QueuedConnection);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
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
GameView::stop_solver_task()
{
    if (!has_solver_task()) {
        throw std::runtime_error(
            "GameView::start_solver_task(): failed to stop solving "
            "task, no task is currently running");
    }

    m_solver_task.stop_solving = true;

    // wait for the thread to exit
    if (m_solver_task.work_thread->joinable()) {
        m_solver_task.work_thread->join();
    }

    // de-allocate the thread
    delete m_solver_task.work_thread;

    // reset the states
    m_solver_task.pause_solving = false;
    m_solver_task.stop_solving  = false;

    m_solver_task.work_thread = nullptr;
}

void
GameView::start_solver_task()
{
    if (has_solver_task()) {
        throw std::runtime_error(
            "GameView::start_solver_task(): failed to init a new solving "
            "task, a task is already present");
    }

    // start the thread;
    m_solver_task.work_thread
        = new std::thread(&GameView::hanoiIterativeSolver, this);
}

void
GameView::unpause_solver_task()
{
    if (!has_paused_solver_task()) {
        throw std::runtime_error("GameView::unpause_solver_task(): failed to "
                                 "un-pause task");
    }
    m_solver_task.pause_solving = false;
}

void
GameView::pause_solver_task()
{
    if (has_paused_solver_task()) {
        throw std::runtime_error("GameView::pause_solver_task(): failed to "
                                 "pause task");
    }
    m_solver_task.pause_solving = true;
}

void
GameView::moveTopSlice(HanoiStack* source, HanoiStack* dest)
{
    if (source == nullptr || dest == nullptr) {
        std::invalid_argument("moveTopSlice(): null stack was passed");
    }

    if (moveIsValid(source, dest)) {
        dest->push(source->pop());
    } else if (!moveIsValid(source, dest) && moveIsValid(dest, source)) {
        source->push(dest->pop());
    }
}

HanoiStack*
GameView::getStack(size_t label)
{
    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        if (m_hanoi.stacks[i].label() == label) { return &m_hanoi.stacks[i]; }
    }
    throw std::runtime_error("getStack(): failed to find stack");
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

// generate the base sizes to be used to render the sprites and etc.
void
GameView::calculateBaseSizes()
{
    m_geometry.stack_area.setWidth(float(width())
                                   / Config::get().Settings().stack_amount);

    m_geometry.stack_area.setHeight(height() * 0.9f);

    m_geometry.slice.setHeight(m_geometry.stack_area.height()
                               / Config::get().Settings().SLICE_MAX);

    m_geometry.slice.setWidth(m_geometry.stack_area.width() * 0.9f);

    m_geometry.stack_base.setWidth(m_geometry.slice.width() * 1.1f);
    m_geometry.stack_base.setHeight(m_geometry.slice.height());

    m_geometry.dialog.setWidth(width() * 0.4f);
    m_geometry.dialog.setHeight(height() * 0.2f);
}

// - draw the stack's slices
// - configure each slice placement in the screen, starting from the bottom
//   of the stack
// - set Slices size to be 90% of the base slice size and the size keeps
//   getting smaller for each slice
void
GameView::drawStack(float offset, HanoiStack* stack, QPainter* painter)
{
    if (stack == nullptr) {
        throw std::invalid_argument("drawStack(): null stack was passed");
    }

    float y = height() - m_geometry.slice.height();

    HanoiSlice* slice = stack->getTail();
    while (slice != nullptr) {
        y -= std::floor(slice->Geometry().height);

        slice->Geometry().x = offset - (slice->Geometry().width * 0.5f);
        slice->Geometry().y = y;

        painter->drawPixmap(QPointF(slice->Geometry().x, slice->Geometry().y),
                            m_sprites.slice.scaled(slice->Geometry().width,
                                                   slice->Geometry().height));

        slice = slice->prev;
    }
}

// render the stack base
void
GameView::drawStackBase(size_t label, float offset, QPainter* painter)
{
    if (painter == nullptr || !painter->isActive()) {
        std::invalid_argument("drawStackBase(): invalid painter was passed");
    }

    //--Draw Stack Base+Pole----------------------------------------------

    const float pole_y = height() - m_sprites.stack_pole.height();

    // draw the pole
    painter->drawPixmap(offset - (m_sprites.stack_pole.width() * 0.5f),
                        pole_y,
                        m_sprites.stack_pole);

    // draw the base
    painter->drawPixmap(offset - (m_geometry.stack_base.width() * 0.5f),
                        height() - m_geometry.stack_base.height(),
                        m_sprites.stack_base);

    //--Draw Stack Label--------------------------------------------------

    const QFont font(Config::get().Theme().font_name,        // fontname
                     m_geometry.stack_base.width() * 0.1f    // size
    );

    QColor font_color = Config::get().Theme().font_color;

    // highlight + draw the indicator if current stack is the goal stack
    if (label == (m_hanoi.goal_stack->label())) {
        const float box_size = (m_geometry.stack_base.width() * 0.1f) + 4;

        font_color = Config::get().Theme().highlight_tint;

        if (!m_time.timer.isActive() && m_game_state == GameState::Running) {
            const QPixmap arrow_sprite = m_sprites.arrow.scaled(
                offset - m_geometry.stack_area.width() * 0.5f,
                m_geometry.stack_base.width() * 0.1f);

            painter->drawPixmap(m_geometry.stack_area.width() * 0.5f,
                                pole_y - (arrow_sprite.height()),
                                arrow_sprite);
        } else {
            painter->fillRect(offset - (box_size * 0.5f),    // x
                              pole_y - (box_size * 0.5f),    // y
                              box_size,                      // w
                              box_size * 0.2f,               // h
                              Config::get().Theme().highlight_tint);
        }
    }

    // set font & color for drawing the label
    painter->setPen(font_color);
    painter->setFont(font);

    // draw the stack label
    painter->drawText(offset - (painter->font().pointSizeF() * 0.5f),
                      pole_y - painter->font().pointSizeF(),
                      numToChar(label));
}

void
GameView::scaleStack()
{
    // fetch the originals
    m_sprites.stack_pole = QPixmap(Config::get().AssetFiles().STACK_POLE);
    m_sprites.stack_base = QPixmap(Config::get().AssetFiles().STACK_BASE);

    // scale the sprites
    m_sprites.stack_pole = m_sprites.stack_pole.scaled(
        m_geometry.stack_base.width() * 0.1f,
        m_geometry.stack_base.height() * Config::get().Settings().SLICE_MAX);

    m_sprites.stack_base
        = m_sprites.stack_base.scaled(m_geometry.stack_base.width(),
                                      m_geometry.stack_base.height());

    // tint the sprites
    colorizeSprite(&m_sprites.stack_base, Config::get().Theme().stack_tint);
    colorizeSprite(&m_sprites.stack_pole, Config::get().Theme().stack_tint);
}

void
GameView::scaleSlices()
{
    float width  = m_geometry.slice.width(),
          height = m_geometry.stack_base.height();

    // every slice has a different size
    for (size_t i = 0; i < Config::get().Settings().slice_amount; i++) {
        m_hanoi.slices[i]->Geometry().height
            = (height *= Config::get().Settings().SCALE_FACTOR);
        m_hanoi.slices[i]->Geometry().width
            = (width *= Config::get().Settings().SCALE_FACTOR);
    }
}

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
HanoiStack*
GameView::calculateStackByPos(const QPointF& point)
{
    const float stack_area_height = m_geometry.stack_area.height();
    const float stack_area_width  = m_geometry.stack_area.width();

    float area_width = stack_area_width;

    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        const float x = (point.x() != 0) ? (point.x() / area_width) : 0;
        const float y = (point.y() != 0) ? (point.y() / stack_area_height) : 0;

        if ((x >= 0 && x <= 1) && (y >= 0 && y <= 1)) {
            return &m_hanoi.stacks[i];
        }

        area_width += stack_area_width;
    }
    return nullptr;
}

// add tint to a pixmap, by using masks
void
GameView::colorizeSprite(QPixmap* sprite, const QColor& color)
{
    if (sprite == nullptr || sprite->isNull()) {
        std::runtime_error("colorizeSprite(): invalid pixmap was passed");
    }

    QPixmap  mask(*sprite);
    QPainter painter;

    // create the mask
    painter.begin(&mask);
    if (painter.isActive()) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(mask.rect(), color);
        painter.end();
    }

    // apply the mask
    painter.begin(sprite);
    if (painter.isActive()) {
        painter.setCompositionMode(QPainter::CompositionMode_Overlay);
        painter.drawImage(0, 0, mask.toImage());
        painter.end();
    }
}

void
GameView::drawDialog(const QString&  text,
                     const QColor&   color,
                     QPainter* const painter)
{
    QPixmap dialog(Config::get().AssetFiles().DIALOG);
    dialog = dialog.scaled(m_geometry.dialog.toSize());

    colorizeSprite(&dialog, color);

    // setup font
    const QFont font(Config::get().Theme().font_name,             // fontname
                     m_geometry.dialog.width() / text.length()    // size
    );
    painter->setFont(font);
    const int font_size = painter->font().pointSizeF();

    // set text color
    painter->setPen(Config::get().Theme().font_color);

    painter->drawPixmap((width() * 0.5f) - (dialog.width() * 0.5f),
                        (height() * 0.5f) - (m_geometry.dialog.height() * 0.5f),
                        dialog);

    painter->drawText((width() * 0.5f) - ((text.length() * font_size) * 0.4f),
                      (height() * 0.5f) + m_geometry.dialog.height() * 0.1f,
                      text);
}

void
GameView::checkWinState()
{
    if (m_time.timer.isActive()) {
        m_time.elapsed++;
        updateInfo();
    }

    if (goalStackIsComplete()) {
        m_game_state = GameState::GameOverWon;
        m_time.timer.stop();
        repaint();
    } else if (m_time.elapsed >= Config::get().Settings().time_length_ms) {
        m_game_state = GameState::GameOverLost;
        m_time.timer.stop();
        repaint();
    }
}

// update the Game's sidebar information
// the current time is calculated from subtracting the start time with a
// time elapsed counter.
void
GameView::updateInfo()
{
    if (m_sidebar_widgets.timer_out != nullptr) {
        if (has_solver_task()) {
            m_sidebar_widgets.timer_out->setText(".....");
        } else {
            auto hh_mm_ss = Utils::extractTimeFromMs(
                Config::get().Settings().time_length_ms - m_time.elapsed);

            QString h, m, s;
            h = QString::number(std::get<0>(hh_mm_ss));
            m = QString::number(std::get<1>(hh_mm_ss));
            s = QString::number(std::get<2>(hh_mm_ss));

            if (std::get<0>(hh_mm_ss) < 10) { h = '0' + h; }
            if (std::get<1>(hh_mm_ss) < 10) { m = '0' + m; }
            if (std::get<2>(hh_mm_ss) < 10) { s = '0' + s; }

            m_sidebar_widgets.timer_out->setText(h + ":" + m + ":" + s);
        }

        if (m_sidebar_widgets.move_count_out != nullptr) {
            m_sidebar_widgets.move_count_out->setText(
                QString::number(m_move_count));
        }
    }
}

size_t
GameView::getRandomGoalStackIndex()
{
    const size_t min = 1, max = Config::get().Settings().stack_amount - 1;

    std::random_device rd;
    std::mt19937       gen(rd());

    std::uniform_int_distribution<size_t> distr(min, max);

    return distr(gen);
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

void
GameView::resizeEvent(QResizeEvent* event)
{
    calculateBaseSizes();
    if (m_game_state != GameState::NotRunning) {
        scaleStack();
        scaleSlices();
    }
}

// on mouse press, pop the slice of the stack below the mouse click,
// and store it.
void
GameView::mousePressEvent(QMouseEvent* event)
{
    if (m_selected.hasSelected() || m_game_state != GameState::Running
        || (event->pos().x() <= 0 || event->pos().y() <= 0)
        || (event->pos().x() >= width() || event->pos().y() >= height())) {
        return;
    }

    HanoiStack* clicked_stack;
    switch (event->button()) {
        case Qt::LeftButton:
            clicked_stack = calculateStackByPos(event->position().toPoint());
            break;
        default:
            return;
    }

    if (clicked_stack == nullptr || clicked_stack->isEmpty()) { return; }

    m_selected.slice = clicked_stack->pop();
    m_selected.stack = clicked_stack;

    moveSelectedSlice(event->pos());
}

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
void
GameView::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_selected.hasSelected() || m_game_state != GameState::Running
        || (event->pos().x() <= 0 || event->pos().y() <= 0)
        || (event->pos().x() >= width() || event->pos().y() >= height())) {
        return;
    }

    moveSelectedSlice(event->pos());
}

// on mouse release, if holding any slice, place the slice in
// the stack where the mouse was released, or just put it back.
void
GameView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_selected.hasSelected() || m_game_state != GameState::Running) {
        return;
    }

    HanoiStack* destination_stack
        = calculateStackByPos(event->position().toPoint());

    // checks:
    //  - if the destination stack is full -> cancel
    //  - if destination stack is the same with the source stack -> cancel
    //  - if the selected slice is bigger
    //     than the destination stack top value -> cancel

    if (moveIsValid(destination_stack)) {
        if (m_game_state == GameState::Running && !m_time.timer.isActive()) {
            m_time.timer.start(1);
        }
        destination_stack->push(m_selected.slice);
        m_move_count++;
    } else {
        m_selected.stack->push(m_selected.slice);
    }

    m_selected = { nullptr, nullptr };

    update();

#ifndef DISABLE_AUDIO
    if (!m_placement_fx->isPlaying()) { m_placement_fx->play(); }
#endif
}
