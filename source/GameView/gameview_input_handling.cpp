//-- Description -------------------------------------------------------------/
// methods that handles the click events                                      /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include <QMouseEvent>
#include <stdexcept>
#include <utility>

#include "../Config/config.h"

// on mouse press, pop the slice of the stack below the mouse click,
// and store it.
void
GameView::mousePressEvent(QMouseEvent* const event)
{
    if (has_solver_task()) return;

    if (SelectedSlice::hasSelected() || m_game_state != GameState::GAME_RUNNING
        || !clickInBounds(event->pos())) {
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

    SelectedSlice::slice = clicked_stack->pop();
    SelectedSlice::stack = clicked_stack;

    SelectedSlice::move(event->pos());
    update();
}

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
void
GameView::mouseMoveEvent(QMouseEvent* const event)
{
    if (has_solver_task()) return;

    if (!SelectedSlice::hasSelected() || m_game_state != GameState::GAME_RUNNING
        || !clickInBounds(event->pos())) {
        return;
    }

    SelectedSlice::move(event->pos());
    update();
}

// on mouse release, if holding any slice, place the slice in
// the stack where the mouse was released, or just put it back.
void
GameView::mouseReleaseEvent(QMouseEvent* const event)
{
    if (has_solver_task()) return;

    if (!SelectedSlice::hasSelected()
        || m_game_state != GameState::GAME_RUNNING) {
        return;
    }

    HanoiStack* destination_stack = nullptr;
    try {
        destination_stack = calculateStackByPos(event->position().toPoint());
        destination_stack->push(SelectedSlice::slice);
    } catch (...) {
        SelectedSlice::stack->push(SelectedSlice::slice);
        SelectedSlice::stack = nullptr;
        SelectedSlice::slice = nullptr;
        update();
        return;
    }

    // increment the move counter
    m_move_count++;

    if (!m_redo_history.isEmpty()) { m_redo_history.clear(); }

    // save the move (source, dest)
    m_move_history.push(
        std::make_pair(SelectedSlice::stack, destination_stack));

    // start the timer
    if (m_game_state == GameState::GAME_RUNNING
        && !TimeInfo::timer.isActive()) {
        TimeInfo::timer.start(1);
        emit(s_game_started());
    }

    SelectedSlice::stack = nullptr;
    SelectedSlice::slice = nullptr;

    update();

    emit(s_slice_moved());
}

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
HanoiStack*
GameView::calculateStackByPos(const QPointF& point)
{
    const float stack_area_height = Geometry::window.height();
    const float stack_area_width  = Geometry::stack_area.width();

    float area_width = stack_area_width;

    for (size_t i = 0; i < Config::Settings::stack_amount; i++) {
        const float x = (point.x() != 0) ? (point.x() / area_width) : 0;
        const float y = (point.y() != 0) ? (point.y() / stack_area_height) : 0;

        if ((x >= 0 && x <= 1) && (y >= 0 && y <= 1)) { return getStack(i); }

        area_width += stack_area_width;
    }

    throw std::runtime_error(
        "GameView::calculateStackByPos(): point is out of bounds");
}

bool
GameView::clickInBounds(const QPoint& p)
{
    return (p.x() >= 0 && p.y() >= 0)
           && (p.x() < Geometry::window.width()
               && p.y() < Geometry::window.height());
}
