//-- Description -------------------------------------------------------------/
// the game's view widget events                                              /
//----------------------------------------------------------------------------/

#include "gameview.h"
#include <QHideEvent>
#include <QShowEvent>

// hide() event
void
GameView::hideEvent(QHideEvent *event)
{
    reset();
    m_game_state = GameState::GAME_INACTIVE;
    emit(s_hidden());
    QWidget::hideEvent(event);
}

// show() event
void
GameView::showEvent(QShowEvent *event)
{
    reset();
    QWidget::showEvent(event);
}
