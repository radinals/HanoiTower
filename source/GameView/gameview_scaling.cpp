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
}

void
GameView::scaleStack()
{
    // fetch the originals
    m_sprites.stack_pole = QPixmap(Config::AssetsFiles::STACK_POLE);
    m_sprites.stack_base = QPixmap(Config::AssetsFiles::STACK_BASE);

    assert(!m_sprites.stack_pole.isNull());
    assert(!m_sprites.stack_base.isNull());

    // scale the sprites
    m_sprites.stack_pole
        = m_sprites.stack_pole.scaled(Geometry::slice.width() * 0.1F,
                                      Geometry::stack_area.height());

    m_sprites.stack_base
        = m_sprites.stack_base.scaled(Geometry::stack_base.width(),
                                      Geometry::stack_base.height());

    // tint the sprites
    colorizeSprite(&m_sprites.stack_base, Config::Theme::stack_tint);
    colorizeSprite(&m_sprites.stack_pole, Config::Theme::stack_tint);
}

void
GameView::scaleSlices()
{
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
