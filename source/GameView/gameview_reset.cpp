//-- Description -------------------------------------------------------------/
// reset the stacks/slices
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include <cassert>
#include <stdexcept>

void
GameView::resetStacks()
{
    //-------------------------------------------------------------------------

    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        m_hanoi.stacks[i].clearStack();
    }

    // scale the stacks
    scaleStack();
}

void
GameView::resetSlices()
{
    //-------------------------------------------------------------------------

    if (m_sprites.slice_tint != Config::get().Theme().slice_tint) {
        QPixmap sprite(Config::get().AssetFiles().SLICE);

        if (sprite.isNull()) {
            throw std::runtime_error(
                "GameView::resetSlices(): failed to load slice sprite.");
        }

        // tint the slice sprite
        colorizeSprite(&sprite, Config::get().Theme().slice_tint);

        // save tinted sprite
        m_sprites.slice = sprite;

        // save the color
        m_sprites.slice_tint = Config::get().Theme().slice_tint;
    }

    //-------------------------------------------------------------------------

    // populate the first stack
    HanoiStack::fillStack(&m_hanoi.stacks[0],
                          Config::get().Settings().slice_amount);

    if (m_hanoi.stacks[0].getSize() != Config::get().Settings().slice_amount) {
        std::runtime_error("clear(): failed to fill starting stack");
    }

    //-------------------------------------------------------------------------

    // allocate space for the slice storing array
    // check if the array should be reallocated or just be reused instead

    // fill the array with default values
    std::memset(&m_hanoi.slices, 0, Config::get().Settings().slice_amount);

    // save the slices to the array
    HanoiSlice *slice = m_hanoi.stacks[0].getTop();
    while (slice != nullptr) {
        m_hanoi.slices[slice->getValue()] = slice;
        slice                             = slice->next;
    }

    //-------------------------------------------------------------------------

    // scale the slices
    scaleSlices();
}

void
GameView::setGoalStack()
{
    // get randomly chosen stack label
    const size_t goalStackLabel = getRandomGoalStackIndex();

    if (goalStackLabel <= 0
        || goalStackLabel > Config::get().Settings().stack_amount) {
        std::runtime_error(
            "GameView::setGoalStack(): failed to generate goal stack label");
    }

    // save the address of the stack
    m_hanoi.goal_stack = &m_hanoi.stacks[goalStackLabel];

    if (m_hanoi.goal_stack == nullptr
        || m_hanoi.goal_stack->label() != goalStackLabel) {
        std::runtime_error(
            "GameView::setGoalStack(): failed to set goal stack");
    }
}
