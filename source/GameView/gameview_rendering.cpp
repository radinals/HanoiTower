#include "gameview.h"

#include "../Config/config.h"

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
