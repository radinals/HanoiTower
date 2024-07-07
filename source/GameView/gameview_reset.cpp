//-- Description -------------------------------------------------------------/
// reset the stacks/slices
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include <cassert>
#include <cstring>

// clears hanoi's stack
void
GameView::clear()
{
    // reset the stacks/slices
    resetStacks();
    resetSlices();

    m_game_state = GameState::GAME_INACTIVE;
}

void
GameView::resetStacks()
{
    // clear the stacks
    for (size_t i = 0; i < Config::Settings::stack_amount; i++) {
        getStack(i)->clearStack();
    }

    // populate the first stack
    HanoiStack::fillStack(getStack(0), Config::Settings::slice_amount);

    assert(getStack(0)->getSize() == Config::Settings::slice_amount);

    // setup the sprite scaling
    scaleStack();
}

void
GameView::resetSlices()
{
    // reset the slice array
    std::memset(&HanoiStacks::slices, 0, Config::Settings::slice_amount);

    // clang-format off
    // save the slices to the array
    getStack(0)->forEverySlice(HanoiStack::IterStart::HEAD,
       [&](HanoiSlice *const &slice) {
           HanoiStacks::slices[slice->getLabel()] = slice;
       }
    );
    // clang-format on

    // setup the sprite scaling
    scaleSlices();
}

void
GameView::setGoalStack()
{
    // get randomly chosen stack label
    HanoiStacks::goal_stack_index = getRandomGoalStackIndex();

    assert(HanoiStacks::goal_stack_index > 0);
    assert(HanoiStacks::goal_stack_index < Config::Settings::stack_amount);

    // save the address of the stack
    HanoiStacks::goal_stack = getStack(HanoiStacks::goal_stack_index);

    assert(HanoiStacks::goal_stack != nullptr);
}
