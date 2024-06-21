//-- Description -------------------------------------------------------------/
// methods thar hanldes the rendering process, is called by the widget paint  /
// event.                                                                     /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include "../Utils/utils.h"

#include <QPainter>

// - draw the stack's slices
// - configure each slice placement in the screen, starting from the bottom
//   of the stack
// - set Slices size to be 90% of the base slice size and the size keeps
//   getting smaller for each slice
void
GameView::drawStack(float             offset,
                    HanoiStack* const stack,
                    QPainter* const   painter)
{
    // draw the stack base
    drawStackBase(stack->getLabel(), offset, painter);

    if (stack->isEmpty()) { return; }

    float y = height() - m_geometry.stack_base.height();

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
GameView::drawStackBase(size_t label, float offset, QPainter* const painter)
{
    assert(painter != nullptr);
    assert(painter->isActive());

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
    if (label == (m_hanoi.goal_stack->getLabel())) {
        font_color = Config::get().Theme().highlight_tint;

        if (!m_time.timer.isActive() && m_game_state == GameState::Running) {
            const QPixmap arrow_sprite = m_sprites.arrow.scaled(
                offset - m_geometry.stack_area.width() * 0.5f,
                m_geometry.stack_base.width() * 0.1f);

            assert(!arrow_sprite.isNull());

            painter->drawPixmap(m_geometry.stack_area.width() * 0.5f,
                                pole_y - (arrow_sprite.height()),
                                arrow_sprite);
        } else {
            const float box_size = (m_geometry.stack_base.width() * 0.1f) + 4;
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
                      Utils::numToChar(label));
}

void
GameView::drawDialog(const QString&  text,
                     const QColor&   color,
                     QPainter* const painter)
{
    QPixmap dialog(Config::get().AssetFiles().DIALOG);

    assert(!dialog.isNull());

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

// add tint to a pixmap, by using masks
void
GameView::colorizeSprite(QPixmap* const sprite, const QColor& color)
{
    assert(sprite != nullptr);

    if (sprite->isNull()) { return; }

    QPixmap mask(*sprite);

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
GameView::paintEvent(QPaintEvent* event)
{
    if (m_game_state == GameState::NotRunning) return;

    QPainter p(this);

    assert(p.isActive());

    updateInfo();

    // render the stacks and slices
    float x_offset = m_geometry.stack_area.width() * 0.5f;
    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        drawStack(x_offset, &m_hanoi.stacks[i], &p);
        x_offset += m_geometry.stack_area.width();    // shift to the right
    }

    // render the selected slice
    if (m_selected.hasSelected()) {
        p.drawPixmap(
            m_selected.slice->Geometry().x,
            m_selected.slice->Geometry().y,
            m_sprites.slice.scaled(m_selected.slice->Geometry().width,
                                   m_selected.slice->Geometry().height));
    }

    // render the game over screens
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
