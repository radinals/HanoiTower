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
GameView::mousePressEvent(QMouseEvent* const event)
{
    if (SelectedSlice::hasSelected() || m_game_state != GameState::Running
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

    SelectedSlice::slice = clicked_stack->pop();
    SelectedSlice::stack = clicked_stack;

    moveSelectedSlice(event->pos());
    update();
}

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
void
GameView::mouseMoveEvent(QMouseEvent* const event)
{
    if (!SelectedSlice::hasSelected() || m_game_state != GameState::Running
        || (event->pos().x() <= 0 || event->pos().y() <= 0)
        || (event->pos().x() >= width() || event->pos().y() >= height())) {
        return;
    }

    moveSelectedSlice(event->pos());
    update();
}

// on mouse release, if holding any slice, place the slice in
// the stack where the mouse was released, or just put it back.
void
GameView::mouseReleaseEvent(QMouseEvent* const event)
{
    if (!SelectedSlice::hasSelected() || m_game_state != GameState::Running) {
        return;
    }

    try {
        HanoiStack* destination_stack
            = calculateStackByPos(event->position().toPoint());
        destination_stack->push(SelectedSlice::slice);
        m_move_count++;
    } catch (...) {
        SelectedSlice::stack->push(SelectedSlice::slice);
    }

    if (m_game_state == GameState::Running && !TimeInfo::timer.isActive()) {
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
    const float stack_area_height = height();
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
