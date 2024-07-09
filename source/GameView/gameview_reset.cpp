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
    // reset the stacks/slices
    for (size_t i = 0; i < Config::Settings::stack_amount; i++) {
        getStack(i)->clearStack();
    }

    // populate the first stack
    HanoiStack::fillStack(getStack(0), Config::Settings::slice_amount);

    assert(getStack(0)->getSize() == Config::Settings::slice_amount);

    // scale the sprites
    scaleStack();
    scaleSlices();

    m_game_state = GameState::GAME_INACTIVE;
}

void
GameView::setGoalStack()
{
    // get randomly chosen stack label
    HanoiStacks::goal_stack_index
        = Utils::randomRange<size_t>(1, Config::Settings::stack_amount - 1);

    assert(HanoiStacks::goal_stack_index > 0);
    assert(HanoiStacks::goal_stack_index < Config::Settings::stack_amount);

    // save the address of the stack
    HanoiStacks::goal_stack = getStack(HanoiStacks::goal_stack_index);

    assert(HanoiStacks::goal_stack != nullptr);
}
