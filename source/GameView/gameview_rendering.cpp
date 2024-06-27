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
GameView::drawStack(float                   offset,
                    const HanoiStack* const stack,
                    QPainter* const         painter)
{
    // draw the stack base
    drawStackBase(stack->getLabel(), offset, painter);

    if (stack->isEmpty()) { return; }

    float y_offset = Geometry::window.height() - Geometry::stack_base.height();

    const HanoiSlice* slice = stack->getTail();
    while (slice != nullptr) {
        y_offset -= std::floor(slice->Geometry().height);

        painter->drawPixmap(
            offset - (slice->Geometry().width * 0.5F),
            y_offset,
            GameSprites::slice->scaled(slice->Geometry().width,
                                       slice->Geometry().height));
        slice = slice->prev();
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

    const QSize box_size(Geometry::stack_pole.width(),
                         Geometry::stack_pole.width());

    // highlight and draw the indicator if current stack is the goal stack
    if (label == (HanoiStacks::goal_stack->getLabel())) {
        // use the highlight color for the font
        painter->setPen(Config::Theme().highlight_tint);

        // draw the arrow if the timer is not running or
        // draw indicator instead
        if (m_game_state == GameState::GAME_RUNNING
            && !TimeInfo::timer.isActive()) {
            const QPixmap arrow_sprite = GameSprites::arrow->scaled(
                offset - Geometry::stack_area.width() * 0.5F,
                Geometry::stack_base.width() * 0.1F);

            assert(!arrow_sprite.isNull());

            painter->drawPixmap(Geometry::stack_area.width() * 0.5F,
                                pole_y - (arrow_sprite.height()),
                                arrow_sprite);
        } else {
            painter->fillRect(offset - (box_size.width() * 0.5F),     // x
                              pole_y - (box_size.height() * 0.5F),    // y
                              box_size.width(),                       // w
                              box_size.height() * 0.2F,               // h
                              Config::Theme().highlight_tint);
        }
    }

    // setup font for drawing the label
    painter->setFont(QFont(Config::Theme().font_name,    // fontname
                           box_size.width() * 0.9F));
    painter->setPen(Config::Theme().font_color);

    const QPoint box_point(offset - (Geometry::stack_pole.width() * 0.5F),
                           pole_y - (box_size.height() * 2));

    const QRect label_bounds = painter->boundingRect(QRect(box_point, box_size),
                                                     Qt::AlignHCenter,
                                                     Utils::numToChar(label));
    // draw the stack label
    painter->drawText(label_bounds, Utils::numToChar(label));
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
    painter->setFont(QFont(Config::Theme().font_name,        // fontname
                           dialog.width() / text.length()    // size
                           ));
    painter->setPen(Config::Theme().font_color);

    // setup bounds to make sure the text is centered
    const QRect dialog_rect(
        QPoint(((Geometry::window.width() * 0.5F) - (dialog.width() * 0.5F)),
               ((Geometry::window.height() * 0.5F) - (dialog.height() * 0.5F))),
        dialog.size());

    const QRect text_rect = painter->boundingRect(dialog_rect.x(),
                                                  dialog_rect.y(),
                                                  dialog_rect.width(),
                                                  dialog.rect().height() * 0.9F,
                                                  Qt::AlignCenter,
                                                  text);

    // render the dialog
    painter->drawPixmap(dialog_rect, dialog);
    painter->drawText(text_rect, text);
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
    if (m_game_state == GameState::GAME_INACTIVE) return;

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
        case GameState::GAME_OVER_LOST:
            drawDialog("TIME's UP!", Config::Theme().lose_dialog_tint, &p);
            break;

        case GameState::GAME_OVER_WON:
            drawDialog("YOU WIN", Config::Theme().win_dialog_tint, &p);
            break;

        default:
            break;
    }
}
