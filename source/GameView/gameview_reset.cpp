//-- Description -------------------------------------------------------------/
// reset the stacks/slices
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include <cassert>

// clears hanoi's stack
void
GameView::clear()
{
    // get the base sizes for rendering
    calculateBaseSizes();

    // reset the stacks/slices
    resetStacks();
    resetSlices();

    m_game_state = GameState::NotRunning;
}

void
GameView::resetStacks()
{
    //-------------------------------------------------------------------------

    for (size_t i = 0; i < Config::Settings().stack_amount; i++) {
        m_hanoi.stacks[i].clearStack();
    }

    // scale the stacks
    scaleStack();
}

void
GameView::resetSlices()
{
    //-------------------------------------------------------------------------

    if (m_sprites.slice_tint != Config::Theme().slice_tint) {
        QPixmap sprite(Config::AssetFiles().SLICE);

        assert(!sprite.isNull());

        // tint the slice sprite
        colorizeSprite(&sprite, Config::Theme().slice_tint);

        // save tinted sprite
        m_sprites.slice = sprite;

        // save the color
        m_sprites.slice_tint = Config::Theme().slice_tint;
    }

    //-------------------------------------------------------------------------

    // populate the first stack
    HanoiStack::fillStack(&m_hanoi.stacks[0], Config::Settings().slice_amount);

    assert(m_hanoi.stacks[0].getSize() == Config::Settings().slice_amount);

    //-------------------------------------------------------------------------

    // allocate space for the slice storing array
    // check if the array should be reallocated or just be reused instead

    // fill the array with default values
    std::memset(&m_hanoi.slices, 0, Config::Settings().slice_amount);

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

    assert(goalStackLabel > 0);
    assert(goalStackLabel < Config::Settings().stack_amount);

    // save the address of the stack
    m_hanoi.goal_stack = getStack(goalStackLabel);

    assert(m_hanoi.goal_stack != nullptr);
    assert(m_hanoi.goal_stack->getLabel() == goalStackLabel);
}
