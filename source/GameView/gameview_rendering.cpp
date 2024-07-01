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
GameView::drawStack(float x_axis, HanoiStack* stack, QPainter* const painter)
{
    assert(painter != nullptr);
    assert(painter->isActive());

    if (stack->isEmpty()) { return; }

    float y_axis = Geometry::window.height() - Geometry::stack_base.height();

    stack->forEverySliceReversed([&](HanoiSlice*& slice) {
        y_axis -= std::floor(slice->Height());

        painter->drawPixmap(
            x_axis - (slice->Width() * 0.5F),
            y_axis,
            GameSprites::slice->scaled(slice->Width(), slice->Height()));
    });
}

// render the stack base
void
GameView::drawStackBase(float x_axis, QPainter* const painter)
{
    assert(painter != nullptr);
    assert(painter->isActive());

    // draw the pole
    painter->drawPixmap(
        x_axis - (Geometry::stack_pole.width() * 0.5F),
        Geometry::window.height() - Geometry::stack_pole.height(),
        GameSprites::stack_pole->scaled(Geometry::stack_pole.toSize()));

    // draw the base
    painter->drawPixmap(
        x_axis - (Geometry::stack_base.width() * 0.5F),
        Geometry::window.height() - Geometry::stack_base.height(),
        GameSprites::stack_base->scaled(Geometry::stack_base.toSize()));
}

// render the stack label
void
GameView::drawStackLabel(size_t label, float x_axis, QPainter* const painter)
{
    assert(painter != nullptr);
    assert(painter->isActive());

    const float pole_y
        = Geometry::window.height() - Geometry::stack_pole.height();

    const QRect label_box(x_axis
                              - (Geometry::stack_pole.width() * 0.5F),    // x
                          pole_y - (Geometry::stack_pole.width() * 2),
                          Geometry::stack_pole.width(),
                          Geometry::stack_pole.width());

    // highlight and draw the indicator if current stack is the goal stack
    if (label == (HanoiStacks::goal_stack->getLabel())) {
        // use the highlight color for the font
        painter->setPen(Config::Theme().highlight_tint);

        // draw the arrow if the timer is not running or
        // draw indicator instead
        if (m_game_state == GameState::GAME_RUNNING
            && !TimeInfo::timer.isActive() && !has_solver_task()) {
            const QPixmap arrow_sprite = GameSprites::arrow->scaled(
                x_axis - Geometry::stack_area.width() * 0.5F,    // w
                Geometry::stack_base.width() * 0.1F);            // h

            assert(!arrow_sprite.isNull());

            painter->drawPixmap(Geometry::stack_area.width() * 0.5F,    // x
                                pole_y - (arrow_sprite.height()),       // y
                                arrow_sprite);
        } else {
            painter->fillRect(label_box.x(),                           // x
                              pole_y - (label_box.height() * 0.5F),    // y
                              label_box.width(),                       // w
                              label_box.height() * 0.2F,               // h
                              Config::Theme().highlight_tint);
        }
    }

    // setup font for drawing the label
    painter->setFont(
        QFont(Config::Theme().font_name, label_box.width() * 0.9F));
    painter->setPen(Config::Theme().font_color);

    const QRect bounds = painter->boundingRect(label_box,
                                               Qt::AlignHCenter,
                                               Utils::numToChar(label));
    // draw the stack label
    painter->drawText(bounds, Utils::numToChar(label));
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
        drawStackBase(x_offset, &p);
        drawStackLabel(getStack(i)->getLabel(), x_offset, &p);
        drawStack(x_offset, getStack(i), &p);
        x_offset += Geometry::stack_area.width();    // shift to the right
    }

    // render the selected slice
    if (SelectedSlice::hasSelected()) {
        p.drawPixmap(
            SelectedSlice::x,
            SelectedSlice::y,
            GameSprites::slice->scaled(SelectedSlice::slice->Width(),
                                       SelectedSlice::slice->Height()));
    }

    // render the game over screens
    switch (m_game_state) {
        case GameState::GAME_OVER_LOST:
            drawDialog("TIME's UP!", Config::Theme::lose_dialog_tint, &p);
            break;

        case GameState::GAME_OVER_WON:
            drawDialog("YOU WIN", Config::Theme::win_dialog_tint, &p);
            break;

        case GameState::GAME_OVER_SOLVER_DONE:
            drawDialog("SOLVER DONE",
                       Config::Theme::solver_done_dialog_tint,
                       &p);
            break;

        default:
            break;
    }
}
