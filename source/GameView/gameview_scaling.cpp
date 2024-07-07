//-- Description -------------------------------------------------------------/
// methods that determines the size of the sprites                            /
// to be rendered.                                                            /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Common/utils.h"
#include "../Config/config.h"
#include <QResizeEvent>

// generate the base sizes to be used to render the sprites and etc.
void
GameView::calculateBaseSizes()
{
    using namespace Utils;

    // stack area  -----------------------------------------------------------
    Geometry::stack_area
        = QSizeF(Geometry::window.width() / Config::Settings::stack_amount,
                 percent(80, Geometry::window.height()));

    // slice -----------------------------------------------------------------
    Geometry::slice = QSizeF(
        percent(80, Geometry::stack_area.width()),
        percent(110,
                (Geometry::stack_area.height() / Config::Settings::SLICE_MAX)));

    // stack base ------------------------------------------------------------
    Geometry::stack_base = QSizeF(percent(90, Geometry::stack_area.width()),
                                  percent(50, Geometry::slice.height()));

    Geometry::stack_pole = QSizeF(percent(10, Geometry::stack_base.width()),
                                  Geometry::stack_area.height());

    // dialog ----------------------------------------------------------------
    Geometry::dialog = QSizeF(percent(50, Geometry::window.width()),
                              percent(25, Geometry::window.height()));
}

void
GameView::scaleStack()
{
    // check for sprite tint change
    if (GameSprites::stack_tint != Config::Theme::stack_tint) {
        // reload the sprites
        GameSprites::stack_pole->load(Config::AssetsFiles::STACK_POLE);
        GameSprites::stack_base->load(Config::AssetsFiles::STACK_BASE);

        assert(!GameSprites::stack_pole->isNull());
        assert(!GameSprites::stack_base->isNull());

        // tint the sprites
        colorizeSprite(GameSprites::stack_base, Config::Theme::stack_tint);
        colorizeSprite(GameSprites::stack_pole, Config::Theme::stack_tint);

        // save the color
        GameSprites::stack_tint = Config::Theme::stack_tint;
    }
}

void
GameView::scaleSlices()
{
    // load and tint a new slice sprite if needed
    if (GameSprites::slice_tint != Config::Theme::slice_tint) {
        GameSprites::slice->load(Config::AssetsFiles::SLICE);

        assert(!GameSprites::slice->isNull());

        // tint the slice sprite
        colorizeSprite(GameSprites::slice, Config::Theme::slice_tint);

        // save the color
        GameSprites::slice_tint = Config::Theme::slice_tint;
    }

    float width  = Geometry::slice.width();
    float height = Geometry::slice.height();

    // every slice has a different size
    for (size_t i = 0; i < Config::Settings::slice_amount; i++) {
        HanoiStacks::slices[i]->Width()  = (width);
        HanoiStacks::slices[i]->Height() = (height);
        height *= Config::Settings::H_SCALE_FACTOR;
        width *= Config::Settings::W_SCALE_FACTOR;
    }
}

void
GameView::resizeEvent(QResizeEvent *const event)
{
    Geometry::window = event->size();
    calculateBaseSizes();
    if (m_game_state != GameState::GAME_INACTIVE) {
        scaleStack();
        scaleSlices();
    }
}
