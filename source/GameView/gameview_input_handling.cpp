#include "gameview.h"

#include <QMouseEvent>

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
