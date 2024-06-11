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
    if (stack == nullptr) {
        throw std::invalid_argument("drawStack(): null stack was passed");
    }

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
                      Utils::numToChar(label));
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
