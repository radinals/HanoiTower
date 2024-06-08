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
    m_geometry.stack_area.setWidth(float(width())
                                   / Config::get().Settings().stack_amount);

    m_geometry.stack_area.setHeight(height() * 0.9f);

    m_geometry.slice.setHeight(m_geometry.stack_area.height()
                               / Config::get().Settings().SLICE_MAX);

    m_geometry.slice.setWidth(m_geometry.stack_area.width() * 0.9f);

    m_geometry.stack_base.setWidth(m_geometry.slice.width() * 1.1f);
    m_geometry.stack_base.setHeight(m_geometry.slice.height());

    m_geometry.dialog.setWidth(width() * 0.4f);
    m_geometry.dialog.setHeight(height() * 0.2f);
}

void
GameView::scaleStack()
{
    // fetch the originals
    m_sprites.stack_pole = QPixmap(Config::get().AssetFiles().STACK_POLE);
    m_sprites.stack_base = QPixmap(Config::get().AssetFiles().STACK_BASE);

    // scale the sprites
    m_sprites.stack_pole = m_sprites.stack_pole.scaled(
        m_geometry.stack_base.width() * 0.1f,
        m_geometry.stack_base.height() * Config::get().Settings().SLICE_MAX);

    m_sprites.stack_base
        = m_sprites.stack_base.scaled(m_geometry.stack_base.width(),
                                      m_geometry.stack_base.height());

    // tint the sprites
    colorizeSprite(&m_sprites.stack_base, Config::get().Theme().stack_tint);
    colorizeSprite(&m_sprites.stack_pole, Config::get().Theme().stack_tint);
}

void
GameView::scaleSlices()
{
    float width  = m_geometry.slice.width(),
          height = m_geometry.stack_base.height();

    // every slice has a different size
    for (size_t i = 0; i < Config::get().Settings().slice_amount; i++) {
        m_hanoi.slices[i]->Geometry().height
            = (height *= Config::get().Settings().SCALE_FACTOR);
        m_hanoi.slices[i]->Geometry().width
            = (width *= Config::get().Settings().SCALE_FACTOR);
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
