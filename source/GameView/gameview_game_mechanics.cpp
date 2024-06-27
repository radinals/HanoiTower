//-- Description -------------------------------------------------------------/
// methods is related to the gameplay logic, and also the modifying of stacks /
// or slices.                                                                 /
//----------------------------------------------------------------------------/

#include "../Config/config.h"
#include "gameview.h"
#include <random>

// get the pointer to a stack
HanoiStack *
GameView::getStack(size_t label)
{
    assert(label >= 0);
    assert(label < Config::Settings::stack_amount);

    return &HanoiStacks::stacks[label];
}

// move the top slice between two stacks
void
GameView::makeLegalMove(HanoiStack *const source, HanoiStack *const dest)
{
    assert(source != nullptr);
    assert(dest != nullptr);
    assert((!dest->isEmpty()) || (!source->isEmpty()));

    if (moveisLegal(*source, *dest)) {
        dest->push(source->pop());
    } else if (moveisLegal(*dest, *source)) {
        source->push(dest->pop());
    }
}

// check a move from source to dest is legal/possible
bool
GameView::moveisLegal(const HanoiStack &source, const HanoiStack &dest)
{
    return !source.isEmpty()
           && (dest.isEmpty()
               || source.peek()->getValue() > dest.peek()->getValue());
}

// generate a random stack label for the goal stack
size_t
GameView::getRandomGoalStackIndex()
{
    const size_t min = 1;
    const size_t max = Config::Settings::stack_amount - 1;

    std::random_device rdev;
    std::mt19937       gen(rdev());

    std::uniform_int_distribution<size_t> distr(min, max);

    return distr(gen);
}

// check the win state, is called by the timer every 1ms
void
GameView::checkWinState()
{
    if (TimeInfo::timer.isActive()) {
        TimeInfo::elapsed++;
        updateInfo();
    }

    if (goalStackIsComplete()) {
        m_game_state = GameState::GAME_OVER_WON;
        TimeInfo::timer.stop();
        emit(s_game_over());
        repaint();
    } else if (TimeInfo::elapsed >= Config::Settings::time_length_ms) {
        m_game_state = GameState::GAME_OVER_LOST;
        TimeInfo::timer.stop();
        emit(s_game_over());
        repaint();
    }
}

// check if the goal stack has all valid slices in it
bool
GameView::goalStackIsComplete()
{
    return (HanoiStacks::goal_stack->getSize()
            == Config::Settings::slice_amount);
}
