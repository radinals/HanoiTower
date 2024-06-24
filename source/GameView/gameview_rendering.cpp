//-- Description -------------------------------------------------------------/
// methods thar hanldes the rendering process, is called by the widget paint  /
// event.                                                                     /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include "../Utils/utils.h"

#include <QPainter>

// draws a single stack with all of it's slices.
void
GameView::drawStack(float             offset,
                    HanoiStack* const stack,
                    QPainter* const   painter)
{
    // draw the stack base
    drawStackBase(stack->getLabel(), offset, painter);

    if (stack->isEmpty()) { return; }

    float y_offset = Geometry::window.height() - Geometry::stack_base.height();

    HanoiSlice* slice = stack->getTail();
    while (slice != nullptr) {
        y_offset -= std::floor(slice->Geometry().height);

        painter->drawPixmap(
            offset - (slice->Geometry().width * 0.5F),
            y_offset,
            GameSprites::slice->scaled(slice->Geometry().width,
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

    const float pole_y
        = Geometry::window.height() - Geometry::stack_pole.height();

    // draw the pole
    painter->drawPixmap(
        offset - (Geometry::stack_pole.width() * 0.5F),
        pole_y,
        GameSprites::stack_pole->scaled(Geometry::stack_pole.toSize()));

    // draw the base
    painter->drawPixmap(
        offset - (Geometry::stack_base.width() * 0.5F),
        Geometry::window.height() - Geometry::stack_base.height(),
        GameSprites::stack_base->scaled(Geometry::stack_base.toSize()));

    //--Draw Stack Label--------------------------------------------------

    // set font color for drawing the label
    painter->setPen(Config::Theme().font_color);

    // highlight and draw the indicator if current stack is the goal stack
    if (label == (HanoiStacks::goal_stack->getLabel())) {
        // use the highlight color for the font
        painter->setPen(Config::Theme().highlight_tint);

        // draw the arrow if the timer is not running or
        // draw indicator instead
        if (!TimeInfo::timer.isActive() && m_game_state == GameState::Running) {
            const QPixmap arrow_sprite = GameSprites::arrow->scaled(
                offset - Geometry::stack_area.width() * 0.5F,
                Geometry::stack_base.width() * 0.1F);

            assert(!arrow_sprite.isNull());

            painter->drawPixmap(Geometry::stack_area.width() * 0.5F,
                                pole_y - (arrow_sprite.height()),
                                arrow_sprite);
        } else {
            const float box_size = (Geometry::stack_base.width() * 0.1F) + 4;
            painter->fillRect(offset - (box_size * 0.5F),    // x
                              pole_y - (box_size * 0.5F),    // y
                              box_size,                      // w
                              box_size * 0.2F,               // h
                              Config::Theme().highlight_tint);
        }
    }

    // set the font style and size
    painter->setFont(QFont(Config::Theme().font_name,             // fontname
                           Geometry::stack_base.width() * 0.1F    // size
                           ));

    // draw the stack label
    painter->drawText(offset - (painter->font().pointSizeF() * 0.5F),
                      pole_y - painter->font().pointSizeF(),
                      Utils::numToChar(label));
}

void
GameView::drawDialog(const QString&  text,
                     const QColor&   color,
                     QPainter* const painter)
{
    QPixmap dialog(Config::AssetsFiles::DIALOG);

    assert(!dialog.isNull());

    dialog = dialog.scaled(Geometry::dialog.toSize());

    colorizeSprite(&dialog, color);

    // setup font
    painter->setFont(QFont(Config::Theme().font_name,    // fontname
                           Geometry::dialog.width() / text.length()    // size
                           ));

    // set text color
    painter->setPen(Config::Theme().font_color);

    painter->drawPixmap(
        (Geometry::window.width() * 0.5F) - (dialog.width() * 0.5F),
        (Geometry::window.height() * 0.5F) - (Geometry::dialog.height() * 0.5F),
        dialog);

    painter->drawText(
        (Geometry::window.width() * 0.5F)
            - ((text.length() * painter->font().pointSizeF()) * 0.4f),
        (Geometry::window.height() * 0.5F) + Geometry::dialog.height() * 0.15F,
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
    float x_offset = Geometry::stack_area.width() * 0.5F;
    for (size_t i = 0; i < Config::Settings().stack_amount; i++) {
        drawStack(x_offset, getStack(i), &p);
        x_offset += Geometry::stack_area.width();    // shift to the right
    }

    // render the selected slice
    if (SelectedSlice::hasSelected()) {
        p.drawPixmap(SelectedSlice::x,
                     SelectedSlice::y,
                     GameSprites::slice->scaled(
                         SelectedSlice::slice->Geometry().width,
                         SelectedSlice::slice->Geometry().height));
    }

    // render the game over screens
    switch (m_game_state) {
        case GameState::GameOverLost:
            drawDialog("TIME's UP!", Config::Theme().lose_dialog_tint, &p);
            break;

        case GameState::GameOverWon:
            drawDialog("YOU WIN", Config::Theme().win_dialog_tint, &p);
            break;

        default:
            break;
    }
}
