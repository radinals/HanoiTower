//-- Description -------------------------------------------------------------/
// methods is related to the gameplay logic, and also the modifying of stacks /
// or slices.                                                                 /
//----------------------------------------------------------------------------/

#include "../Config/config.h"
#include "gameview.h"
#include <random>
#include <stdexcept>

// get the pointer to a stack
HanoiStack *
GameView::getStack(size_t label)
{
    if (label < 0 || label >= Config::get().Settings().slice_amount) {
        throw std::range_error(
            "GameView:;getStack(): the label out of the stack array's range");
    }

    return &m_hanoi.stacks[label];
}

// move the top slice between two stacks
void
GameView::makeLegalMove(HanoiStack *const source, HanoiStack *const dest)
{
    if (source == nullptr || dest == nullptr) {
        std::invalid_argument("moveTopSlice(): null stack was passed");
    }

    if (source->isEmpty() && dest->isEmpty()) {
        throw std::out_of_range("GameView::makeLegalMove(): there is no "
                                "possible move between two empty stack");
    };

    if (source->isEmpty()) {
        source->push(dest->pop());
    } else if (dest->isEmpty()) {
        dest->push(source->pop());
    } else if (source->peek()->getValue() > dest->peek()->getValue()) {
        dest->push(source->pop());
    } else {
        source->push(dest->pop());
    }
}

// generate a random stack label for the goal stack
size_t
GameView::getRandomGoalStackIndex()
{
    const size_t min = 1, max = Config::get().Settings().stack_amount - 1;

    std::random_device rd;
    std::mt19937       gen(rd());

    std::uniform_int_distribution<size_t> distr(min, max);

    return distr(gen);
}

// check the win state, is called by the timer every 1ms
void
GameView::checkWinState()
{
    if (m_time.timer.isActive()) {
        m_time.elapsed++;
        updateInfo();
    }

    if (goalStackIsComplete()) {
        m_game_state = GameState::GameOverWon;
        m_time.timer.stop();
        repaint();
    } else if (m_time.elapsed >= Config::get().Settings().time_length_ms) {
        m_game_state = GameState::GameOverLost;
        m_time.timer.stop();
        repaint();
    }
}

// move slice stored by m_selected, to the QPoints x and y values
void
GameView::moveSelectedSlice(const QPoint &p)
{
    m_selected.slice->Geometry().x
        = (p.x() - (m_selected.slice->Geometry().width * 0.5f));
    m_selected.slice->Geometry().y
        = (p.y() - (m_selected.slice->Geometry().height * 0.5f));
    update();
}

// check if the goal stack has all valid slices in it
bool
GameView::goalStackIsComplete()
{
    return (m_hanoi.goal_stack->getSize()
            == Config::get().Settings().slice_amount);
}
