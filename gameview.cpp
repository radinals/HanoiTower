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
#include <cmath>
#include <qaccessible_base.h>
#include <random>

GameView::GameView(QWidget* parent) : QWidget { parent }
{
    // initialize all possible stack
    for (int i = Config::get().getStackMax(); i >= 0; i--) {
        m_stacks.append(std::make_pair(i, HanoiStack()));
    }

    // init timer.
    // timer will call checkWinState every tick (should be every 1ms).
    connect(&m_timer, &QTimer::timeout, this, &GameView::checkWinState);

    // load the placement sound effect
    m_placement_fx = new QSoundEffect(this);
    m_placement_fx->setSource("qrc" + Config::get().getPlacementFXAudioPath());
    m_placement_fx->setVolume(Config::get().getAudioFXVolumeLevel());

    m_pole_sprite       = QPixmap(Config::get().getStackPoleSpritePath());
    m_stack_base_sprite = QPixmap(Config::get().getStackBaseSpritePath());
    m_arrow_sprite      = QPixmap(Config::get().getArrowSpritePath());
    m_win_dialog        = QPixmap(Config::get().getWinScreen());
    m_lose_dialog       = QPixmap(Config::get().getLoseScreen());

    // tint the sprites
    colorizeSprite(&m_pole_sprite, Config::get().getStackTint());
    colorizeSprite(&m_stack_base_sprite, Config::get().getStackTint());
    colorizeSprite(&m_arrow_sprite, Config::get().getHighlightColor());
}

void
GameView::pause()
{
    switch (m_game_state) {
        case GameState::GamePaused:
            m_timer.start(1);
            updateInfo();
            m_game_state = GameState::GameRunning;
            break;
        case GameState::GameRunning:
            m_time_output->setText("PAUSED");
            m_timer.stop();
            m_game_state = GameState::GamePaused;
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
    m_placement_fx->setVolume(Config::get().getAudioFXVolumeLevel());
    m_game_state = GameState::GameRunning;
    repaint();
}

void
GameView::clear()
{
    calculatesSizes();

    m_goal_stack.second = nullptr;
    m_goal_stack.first  = getRandomGoalStackIndex();

    m_info_box->setText("Move All Slice to Stack "
                        + numToChar(m_goal_stack.first));
    m_info_box->setAlignment(Qt::AlignCenter);

    m_timer_elapsed = m_move_count = 0;

    m_game_state = GameState::GameNotRunning;
    m_timer.stop();
    m_slice_list.clear();

    // clear all stack
    auto* node = m_stacks.m_head;
    while (node != nullptr) {
        if (m_goal_stack.second == nullptr
            && node->data.first == m_goal_stack.first) {
            m_goal_stack.second = &node->data.second;
        }

        node->data.second.clearStack();
        node = node->next;
    }

    // populate the first stack
    HanoiStack::generate_stack(&m_stacks.m_head->data.second,
                               Config::get().getSliceAmount());

    HanoiSlice* slice = m_stacks.m_head->data.second.getHead();
    while (slice != nullptr) {
        colorizeSprite(&slice->getPixmap(), Config::get().getSliceTint());
        m_slice_list.append(slice);    // save the slices for ease of access
        slice = slice->next;
    }

    // scale the slices
    scaleSlices();
}

// generate the base sizes to be used to render the sprites and etc.
void
GameView::calculatesSizes()
{
    m_stack_area_size.setWidth(float(width()) / Config::get().getStackAmount());

    m_stack_area_size.setHeight(height() * 0.9f);

    m_slice_base_size.setHeight(m_stack_area_size.height()
                                / Config::get().getSliceMax());

    m_slice_base_size.setWidth(m_stack_area_size.width() * 0.9f);

    m_stack_base_size.setWidth(m_slice_base_size.width() * 1.1f);
    m_stack_base_size.setHeight(m_slice_base_size.height());

    m_gameover_dialog_size.setWidth(width() * 0.2f);
    m_gameover_dialog_size.setHeight(height() * 0.2f);
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

    float y_axis = (height() - m_slice_base_size.height());

    HanoiSlice* slice = stack->getTail();
    while (slice != nullptr) {
        y_axis -= std::floor(slice->getHeight());

        slice->setX(offset - (slice->getWidth() * 0.5f));
        slice->setY(y_axis);

        painter->drawPixmap(QPointF(slice->getX(), slice->getY()),
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
        = m_stack_base_size.height() * Config::get().getSliceMax(),
        pole_base_y_offset = height() - pole_height;

    // scale the sprites
    const QPixmap pole_sprite
        = m_pole_sprite.scaled(m_stack_base_size.width() * 0.1f, pole_height);

    const QPixmap stack_base_sprite
        = m_stack_base_sprite.scaled(m_stack_base_size.width(),
                                     m_stack_base_size.height());

    const QPixmap arrow_sprite
        = m_arrow_sprite.scaled(offset - m_stack_area_size.width() * 0.5f,
                                m_stack_base_size.width() * 0.1f);

    // draw the pole
    painter->drawPixmap(offset - (pole_sprite.width() * 0.5f),
                        pole_base_y_offset,
                        pole_sprite);

    // draw the base
    painter->drawPixmap(offset - (m_stack_base_size.width() * 0.5f),
                        height() - m_stack_base_size.height(),
                        stack_base_sprite);

    //--Draw Stack Label--------------------------------------------------

    const QFont font(Config::get().getStackLabelFont(),    // fontname
                     m_stack_base_size.width() * 0.1f      // size
    );

    const QString stack_label = numToChar(label);

    QColor font_color = Config::get().getStackLabelFontColor();

    // highlight + draw the indicator if current stack is the goal stack
    if (label == m_goal_stack.first) {
        const float box_size = (m_stack_base_size.width() * 0.1f) + 4;

        font_color = Config::get().getHighlightColor();

        if (!m_timer.isActive() && m_game_state == GameState::GameRunning) {
            painter->drawPixmap(m_stack_area_size.width() * 0.5f,
                                pole_base_y_offset - (arrow_sprite.height()),
                                arrow_sprite);
        } else {
            painter->fillRect(offset - (box_size * 0.5f),                // x
                              pole_base_y_offset - (box_size * 0.5f),    // y
                              box_size,                                  // w
                              box_size * 0.2f,                           // h
                              Config::get().getHighlightColor());
        }
    }

    // set font & color for drawing the label
    painter->setPen(font_color);
    painter->setFont(font);

    // draw the stack label
    painter->drawText(offset - (painter->font().pointSizeF() * 0.5f),
                      pole_base_y_offset - painter->font().pointSizeF(),
                      stack_label);
}

void
GameView::scaleSlices()
{
    float width  = m_slice_base_size.width(),
          height = m_stack_base_size.height();

    auto node = m_slice_list.m_head;
    while (node != nullptr) {
        node->data->setHeight(height *= m_slice_scale_factor);
        node->data->setWidth(width *= m_slice_scale_factor);
        node = node->next;
    }
}

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
std::pair<size_t, HanoiStack*>
GameView::calculateStackByPos(const QPointF& point)
{
    const float stack_h = m_stack_area_size.height();
    float       stack_w = m_stack_area_size.width();

    auto* node = m_stacks.m_head;
    while (node != nullptr) {
        // x and y should be 1 if the point is in the stack area
        float x_area = stack_w / point.x(), y_area = stack_h / point.y();

        if ((x_area >= 1) && (y_area >= 1)) {
            return { node->data.first, &node->data.second };
        }

        // shift to the right, by stack area width
        stack_w += m_stack_area_size.width();

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
GameView::checkWinState()
{
    m_timer_elapsed++;
    updateInfo();

    if (goalStackIsComplete()) {
        m_game_state = GameState::GameOverWon;
        m_timer.stop();
        repaint();
    } else if (m_timer_elapsed >= Config::get().getTimerInterval()) {
        m_game_state = GameState::GameOverLost;
        m_timer.stop();
        repaint();
    }
}

// update the Game's sidebar information
// the current time is calculated from subtracting the start time with a
// time elapsed counter.
void
GameView::updateInfo()
{
    if (m_time_output != nullptr) {
        auto hh_mm_ss = Utils::extractTimeFromMs(
            Config::get().getTimerInterval() - m_timer_elapsed);

        QString h, m, s;
        h = QString::number(std::get<0>(hh_mm_ss));
        m = QString::number(std::get<1>(hh_mm_ss));
        s = QString::number(std::get<2>(hh_mm_ss));

        if (std::get<0>(hh_mm_ss) < 10) { h = '0' + h; }
        if (std::get<1>(hh_mm_ss) < 10) { m = '0' + m; }
        if (std::get<2>(hh_mm_ss) < 10) { s = '0' + s; }

        m_time_output->setText(h + ":" + m + ":" + s);
    }

    if (m_move_count_output != nullptr) {
        m_move_count_output->setText(QString::number(m_move_count));
    }
}

size_t
GameView::getRandomGoalStackIndex()
{
    const size_t min = 1, max = Config::get().getStackAmount() - 1;

    std::random_device rd;
    std::mt19937       gen(rd());

    std::uniform_int_distribution<size_t> distr(min, max);

    return distr(gen);
}

void
GameView::paintEvent(QPaintEvent* event)
{
    if (m_game_state == GameState::GameNotRunning) return;

    QPainter p(this);

    updateInfo();

    float offset = m_stack_area_size.width() * 0.5f;

    auto* node = m_stacks.m_head;
    while (node != nullptr
           && node->data.first < Config::get().getStackAmount()) {
        // draw the stack base
        drawStackBase(node->data.first, offset, &p);

        // render the stack
        drawStack(offset, &node->data.second, &p);

        // shift to the right for the next stack
        offset += m_stack_area_size.width();

        node = node->next;
    }

    // render the selected stack
    if (m_selected_slice.valid()) {
        p.drawPixmap(m_selected_slice.slice->getX(),
                     m_selected_slice.slice->getY(),
                     m_selected_slice.slice->getScaledPixmap());
    }

    switch (m_game_state) {
        case GameState::GameOverLost: {
            const QPixmap lose_dialog
                = m_lose_dialog.scaled(m_gameover_dialog_size.toSize(),
                                       Qt::KeepAspectRatioByExpanding);
            p.drawPixmap(
                (width() * 0.5f) - m_gameover_dialog_size.width() * 0.5f,
                (height() * 0.5f) - m_gameover_dialog_size.height() * 0.5f,
                lose_dialog);

        } break;

        case GameState::GameOverWon: {
            const QPixmap win_dialog
                = m_win_dialog.scaled(m_gameover_dialog_size.toSize(),
                                      Qt::KeepAspectRatioByExpanding);
            p.drawPixmap(
                (width() * 0.5f) - m_gameover_dialog_size.width() * 0.5f,
                (height() * 0.5f) - m_gameover_dialog_size.height() * 0.5f,
                win_dialog);

        } break;

        default:
            break;
    }
}

void
GameView::resizeEvent(QResizeEvent* event)
{
    calculatesSizes();
    if (m_game_state != GameState::GameNotRunning) { scaleSlices(); }
}

// on mouse press, pop the slice of the stack below the mouse click,
// and store it.
void
GameView::mousePressEvent(QMouseEvent* event)
{
    if (m_selected_slice.valid() || m_game_state != GameState::GameRunning) {
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

    m_selected_slice.slice = clicked_stack.second->pop();
    m_selected_slice.stack = clicked_stack.second;

    moveSelectedSlice(event->pos());
}

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
void
GameView::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_selected_slice.valid() || m_game_state != GameState::GameRunning) {
        return;
    }

    moveSelectedSlice(event->pos());
}

// on mouse release, if holding any slice, place the slice in
// the stack where the mouse was released, or just put it back.
void
GameView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_selected_slice.valid() || m_game_state != GameState::GameRunning) {
        return;
    }

    std::pair<size_t, HanoiStack*> destination_stack
        = calculateStackByPos(event->position().toPoint());

    // checks:
    //  - if the destination stack is full -> cancel
    //  - if destination stack is the same with the source stack -> cancel
    //  - if the selected slice is bigger
    //     than the destination stack top value -> cancel

    if (moveIsValid(destination_stack)) {
        if (m_game_state == GameState::GameRunning && !m_timer.isActive()) {
            m_timer.start(1);
        }
        destination_stack.second->push(m_selected_slice.slice);
        m_move_count++;
    } else {
        m_selected_slice.stack->push(m_selected_slice.slice);
    }

    m_selected_slice = { .stack = nullptr, .slice = nullptr };

    update();

    if (!m_placement_fx->isPlaying()) { m_placement_fx->play(); }
}
