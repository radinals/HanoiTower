//-- Description -------------------------------------------------------------/
// methods that determines the size of the sprites                            /
// to be rendered.                                                            /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"

// generate the base sizes to be used to render the sprites and etc.
void
GameView::calculateBaseSizes()
{
    Geometry::window = this->size();

    Geometry::stack_area.setWidth(float(width())
                                  / Config::Settings::stack_amount);

    Geometry::stack_area.setHeight(height() * 0.8F);

    Geometry::slice.setHeight(
        (Geometry::stack_area.height() / Config::SLICE_MAX) * 1.1F);

    Geometry::slice.setWidth(Geometry::stack_area.width() * 0.9F);

    Geometry::stack_base.setWidth(Geometry::slice.width() * 1.1F);
    Geometry::stack_base.setHeight(Geometry::slice.height() * 0.5F);

    Geometry::dialog.setWidth(width() * 0.4F);
    Geometry::dialog.setHeight(height() * 0.2F);

    Geometry::stack_pole.setHeight(Geometry::stack_area.height());
    Geometry::stack_pole.setWidth(Geometry::stack_base.width() * 0.1F);
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

    float width = Geometry::slice.width(), height = Geometry::slice.height();

    // every slice has a different size
    for (size_t i = 0; i < Config::Settings::slice_amount; i++) {
        HanoiStacks::slices[i]->Geometry().height
            = (height *= Config::H_SCALE_FACTOR);
        HanoiStacks::slices[i]->Geometry().width
            = (width *= Config::W_SCALE_FACTOR);
    }
}

void
GameView::resizeEvent(QResizeEvent* event)
{
    calculateBaseSizes();
    if (m_game_state != GameState::NotRunning) {
        scaleStack();
        scaleSlices();
    }
}
