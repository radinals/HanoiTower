//-- Description -------------------------------------------------------------/
// methods that handles the click events                                      /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include <QMouseEvent>
#include <stdexcept>

#include "../Config/config.h"

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

    HanoiStack* clicked_stack = nullptr;
    if (event->button() == Qt::LeftButton) {
        try {
            clicked_stack = calculateStackByPos(event->pos().toPointF());
            if (clicked_stack->isEmpty()) { return; }
        } catch (...) {
            return;
        }
    } else {
        return;
    }

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

    try {
        HanoiStack* destination_stack
            = calculateStackByPos(event->position().toPoint());
        destination_stack->push(m_selected.slice);
        m_move_count++;
    } catch (...) {
        m_selected.stack->push(m_selected.slice);
    }

    if (m_game_state == GameState::Running && !m_time.timer.isActive()) {
        m_time.timer.start(1);
        emit(s_game_started());
    }

    m_selected.clear();
    update();

#ifndef DISABLE_AUDIO
    if (!m_placement_fx->isPlaying()) { m_placement_fx->play(); }
#endif
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

    throw std::runtime_error(
        "GameView::calculateStackByPos(): point is out of bounds");
}
