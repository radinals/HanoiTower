#include "gameview.h"

#include "config.h"
#include "hanoistack.h"
#include "linkedlist.h"
#include "utils.h"

#include <QFont>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <cassert>
#include <cmath>
#include <qaccessible_base.h>
#include <random>

GameView::GameView(QWidget* parent) : QWidget { parent }
{
    // initialize all possible stack
    for (int i = Config::get().Settings().STACK_MAX; i >= 0; i--) {
        m_stack_data.stacks.pushBack(std::make_pair(i, HanoiStack()));
    }

    // init timer.
    // timer will call checkWinState every tick (should be every 1ms).
    connect(&m_time.timer, &QTimer::timeout, this, &GameView::checkWinState);

    // load the placement sound effect
    m_placement_fx = new QSoundEffect(this);
    m_placement_fx->setSource("qrc" + Config::get().AudioFiles().PLACEMENT_FX);
    m_placement_fx->setVolume(Config::get().Settings().fx_volume);

    m_sprites.stack_pole = QPixmap(Config::get().AssetFiles().STACK_POLE);
    m_sprites.stack_base = QPixmap(Config::get().AssetFiles().STACK_BASE);
    m_sprites.arrow      = QPixmap(Config::get().AssetFiles().ARROW);

    // tint the sprites
    colorizeSprite(&m_sprites.stack_pole, Config::get().Theme().stack_tint);
    colorizeSprite(&m_sprites.stack_base, Config::get().Theme().stack_tint);
    colorizeSprite(&m_sprites.arrow, Config::get().Theme().highlight_tint);
}

void
GameView::hanoiIterativeSolver()
{
    const size_t n = Config::get().Settings().slice_amount;

    size_t possible_moves = (1 << n) - 1;

    size_t source = 0,
           aux    = ((m_stack_data.goal_stack->first == 1)
                         ? m_stack_data.goal_stack->first + 1
                         : 1),
           dest   = m_stack_data.goal_stack->first;

    if (n % 2 == 0) {
        size_t tmp;
        tmp  = dest;
        dest = aux;
        aux  = tmp;
    }

    for (int i = 1; i <= possible_moves; i++) {
        if (i % 3 == 0) {
            moveTopSlice(getStack(aux), getStack(dest));
        } else if (i % 3 == 1) {
            moveTopSlice(getStack(source), getStack(dest));
        } else {
            moveTopSlice(getStack(source), getStack(aux));
        }
        ++m_move_count;
        repaint();
        while (m_placement_fx->isPlaying()) delay(1);
        m_placement_fx->play();
    }
}

void
GameView::solve()
{
    if (m_game_state != GameState::Running || m_time.timer.isActive()) {
        return;
    }
    m_game_state = GameState::AutoSolving;
    m_sidebar_widgets.timer_out->setText("...");
    hanoiIterativeSolver();
    checkWinState();
}

void
GameView::moveTopSlice(HanoiStack* source, HanoiStack* dest)
{
    if (source == nullptr || dest == nullptr) { return; }

    if (moveIsValid(source, dest)) {
        dest->push(source->pop());
    } else if (!moveIsValid(source, dest) && moveIsValid(dest, source)) {
        source->push(dest->pop());
    }
}

HanoiStack*
GameView::getStack(size_t label)
{
    auto* node = m_stack_data.stacks.m_head;
    while (node != nullptr) {
        if (node->data.first == label) { return &node->data.second; }
        node = node->next;
    }
    return nullptr;
}

void
GameView::pause()
{
    switch (m_game_state) {
        case GameState::Paused:
            m_time.timer.start(1);
            updateInfo();
            m_game_state = GameState::Running;
            break;
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
    if (m_game_state == GameState::AutoSolving) return;
    clear();
    updateInfo();
    m_placement_fx->setVolume(Config::get().Settings().fx_volume);
    m_game_state = GameState::Running;
    repaint();
}

void
GameView::clear()
{
    calculateBaseSizes();

    m_time.elapsed = m_move_count = 0;

    m_game_state = GameState::NotRunning;
    m_time.timer.stop();
    m_stack_data.slices.clear();

    const size_t goalStackLabel = getRandomGoalStackIndex();

    m_stack_data.goal_stack = nullptr;

    // clear all stack
    // search for the goal stack node
    auto* node = m_stack_data.stacks.m_head;
    while (node != nullptr) {
        if (node->data.first == goalStackLabel) {
            m_stack_data.goal_stack = &node->data;
        }
        node->data.second.clearStack();
        node = node->next;
    }

    assert(m_stack_data.goal_stack != nullptr);
    assert(m_stack_data.goal_stack->first == goalStackLabel);

    m_sidebar_widgets.info_msg_out->setText(
        "Move All Slice to Stack " + numToChar(m_stack_data.goal_stack->first));

    m_sidebar_widgets.info_msg_out->setAlignment(Qt::AlignCenter);

    assert(Config::get().Settings().slice_amount > 0);

    // populate the first stack
    HanoiStack::initializeStack(&m_stack_data.stacks.m_head->data.second,
                                Config::get().Settings().slice_amount);

    assert(m_stack_data.stacks.m_head->data.second.getSize()
           == Config::get().Settings().slice_amount);

    HanoiSlice* slice = m_stack_data.stacks.m_head->data.second.getHead();
    while (slice != nullptr) {
        colorizeSprite(&slice->getPixmap(), Config::get().Theme().slice_tint);
        m_stack_data.slices.pushBack(
            slice);    // save the slices for ease of access
        slice = slice->next;
    }

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
    if (stack == nullptr || stack->isEmpty()) { return; }

    float y = height() - m_geometry.slice.height();

    HanoiSlice* slice = stack->getTail();
    while (slice != nullptr) {
        y -= std::floor(slice->Geometry().height);

        slice->Geometry().x = offset - (slice->Geometry().width * 0.5f);
        slice->Geometry().y = y;

        painter->drawPixmap(QPointF(slice->Geometry().x, slice->Geometry().y),
                            slice->getScaledPixmap());

        slice = slice->prev;
    }
}

// render the stack base
void
GameView::drawStackBase(size_t label, float offset, QPainter* painter)
{
    if (painter == nullptr || !painter->isActive()) { return; }

    //--Draw Stack Base+Pole----------------------------------------------

    const float pole_height
        = m_geometry.stack_base.height() * Config::get().Settings().SLICE_MAX,
        pole_y = height() - pole_height;

    // scale the sprites
    const QPixmap pole_sprite
        = m_sprites.stack_pole.scaled(m_geometry.stack_base.width() * 0.1f,
                                      pole_height);

    const QPixmap stack_base_sprite
        = m_sprites.stack_base.scaled(m_geometry.stack_base.width(),
                                      m_geometry.stack_base.height());

    const QPixmap arrow_sprite
        = m_sprites.arrow.scaled(offset - m_geometry.stack_area.width() * 0.5f,
                                 m_geometry.stack_base.width() * 0.1f);

    // draw the pole
    painter->drawPixmap(offset - (pole_sprite.width() * 0.5f),
                        pole_y,
                        pole_sprite);

    // draw the base
    painter->drawPixmap(offset - (m_geometry.stack_base.width() * 0.5f),
                        height() - m_geometry.stack_base.height(),
                        stack_base_sprite);

    //--Draw Stack Label--------------------------------------------------

    const QFont font(Config::get().Theme().font_name,        // fontname
                     m_geometry.stack_base.width() * 0.1f    // size
    );

    QColor font_color = Config::get().Theme().font_color;

    // highlight + draw the indicator if current stack is the goal stack
    if (label == (m_stack_data.goal_stack->first)) {
        const float box_size = (m_geometry.stack_base.width() * 0.1f) + 4;

        font_color = Config::get().Theme().highlight_tint;

        if (!m_time.timer.isActive() && m_game_state == GameState::Running) {
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
GameView::scaleSlices()
{
    float width  = m_geometry.slice.width(),
          height = m_geometry.stack_base.height();

    auto node = m_stack_data.slices.m_head;
    while (node != nullptr) {
        node->data->Geometry().height = (height *= m_slice_scale_factor);
        node->data->Geometry().width  = (width *= m_slice_scale_factor);

        node = node->next;
    }
}

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
std::pair<size_t, HanoiStack*>
GameView::calculateStackByPos(const QPointF& point)
{
    const float stack_area_height = m_geometry.stack_area.height();
    const float stack_area_width  = m_geometry.stack_area.width();

    float area_width = stack_area_width;
    auto* node       = m_stack_data.stacks.m_head;

    while (node != nullptr) {
        // normalized vector
        const float x = (point.x() != 0) ? (point.x() / area_width) : 0;
        const float y = (point.y() != 0) ? (point.y() / stack_area_height) : 0;

        if ((x >= 0 && x <= 1) && (y >= 0 && y <= 1)) {
            return { node->data.first, &node->data.second };
        }

        // shift to the right, by stack area width
        area_width += stack_area_width;

        node = node->next;
    }

    return { 0, nullptr };
}

// add tint to a pixmap, by using masks
void
GameView::colorizeSprite(QPixmap* sprite, const QColor& color)
{
    if (sprite == nullptr) { return; }

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
    if (m_sidebar_widgets.timer_out != nullptr
        && m_game_state != GameState::AutoSolving) {
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

    auto* node = m_stack_data.stacks.m_head;
    while (node != nullptr
           && node->data.first < Config::get().Settings().stack_amount) {
        // draw the stack base
        drawStackBase(node->data.first, offset, &p);

        // render the stack
        drawStack(offset, &node->data.second, &p);

        // shift to the right for the next stack
        offset += m_geometry.stack_area.width();

        node = node->next;
    }

    // render the selected stack
    if (m_selected.valid()) {
        p.drawPixmap(m_selected.slice->Geometry().x,
                     m_selected.slice->Geometry().y,
                     m_selected.slice->getScaledPixmap());
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
    if (m_game_state != GameState::NotRunning) { scaleSlices(); }
}

// on mouse press, pop the slice of the stack below the mouse click,
// and store it.
void
GameView::mousePressEvent(QMouseEvent* event)
{
    if (m_selected.valid() || m_game_state != GameState::Running
        || (event->pos().x() <= 0 || event->pos().y() <= 0)
        || (event->pos().x() >= width() || event->pos().y() >= height())) {
        return;
    }

    std::pair<size_t, HanoiStack*> clicked_stack;
    switch (event->button()) {
        case Qt::LeftButton:
            clicked_stack = calculateStackByPos(event->position().toPoint());
            break;
        default:
            return;
    }

    if (clicked_stack.second == nullptr || clicked_stack.second->isEmpty()) {
        return;
    }

    m_selected.slice = clicked_stack.second->pop();
    m_selected.stack = clicked_stack.second;

    moveSelectedSlice(event->pos());
}

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
void
GameView::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_selected.valid() || m_game_state != GameState::Running
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
    if (!m_selected.valid() || m_game_state != GameState::Running) { return; }

    std::pair<size_t, HanoiStack*> destination_stack
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
        destination_stack.second->push(m_selected.slice);
        m_move_count++;
    } else {
        m_selected.stack->push(m_selected.slice);
    }

    m_selected = { nullptr, nullptr };

    update();

    if (!m_placement_fx->isPlaying()) { m_placement_fx->play(); }
}
