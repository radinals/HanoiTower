#include "../Config/config.h"
#include "gameview.h"
#include <random>

HanoiStack *
GameView::getStack(size_t label)
{
    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        if (m_hanoi.stacks[i].label() == label) { return &m_hanoi.stacks[i]; }
    }
    throw std::runtime_error("getStack(): failed to find stack");
}

void
GameView::moveTopSlice(HanoiStack *source, HanoiStack *dest)
{
    if (source == nullptr || dest == nullptr) {
        std::invalid_argument("moveTopSlice(): null stack was passed");
    }

    if (moveIsValid(source, dest)) {
        dest->push(source->pop());
    } else if (!moveIsValid(source, dest) && moveIsValid(dest, source)) {
        source->push(dest->pop());
    }
}

size_t
GameView::getRandomGoalStackIndex()
{
    const size_t min = 1, max = Config::get().Settings().stack_amount - 1;

    std::random_device rd;
    std::mt19937       gen(rd());

    std::uniform_int_distribution<size_t> distr(min, max);

    return distr(gen);
}

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

// overloaded game movement rule check,
// this checks the slice in m_selected
bool
GameView::moveIsValid(HanoiStack *&dest)
{
    return (dest != nullptr)
           && (dest->isEmpty()
               || m_selected.slice->getValue() > dest->peek()->getValue());
}

// overloaded game movement rule check,
// this checks the top slice in two stacks
bool
GameView::moveIsValid(HanoiStack *source, HanoiStack *dest)
{
    return (!source->isEmpty())
           && (dest->isEmpty()
               || source->peek()->getValue() > dest->peek()->getValue());
}
