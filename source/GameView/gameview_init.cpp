//-- Description -------------------------------------------------------------/
// initializing methods
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include <cassert>
#include <stdexcept>

void
GameView::initStacks()
{
    //-------------------------------------------------------------------------

    // check if the array should be reallocated or just be reused instead

    if (m_hanoi.stacks_arr_size != Config::get().Settings().stack_amount) {
        // delete the stack first if already exists
        if (m_hanoi.stacks != nullptr) { delete[] m_hanoi.stacks; }

        // allocate space and save the size for comparison in the next reset
        m_hanoi.stacks = new HanoiStack[Config::get().Settings().stack_amount];
        m_hanoi.stacks_arr_size = Config::get().Settings().stack_amount;

        // initialize all possible stack
        for (int i = Config::get().Settings().stack_amount - 1; i >= 0; i--) {
            m_hanoi.stacks[i] = (HanoiStack(i));
        }
    } else if (m_hanoi.stacks_arr_size
               == Config::get().Settings().stack_amount) {
        // clear all stack
        for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
            m_hanoi.stacks[i].clearStack();
        }
    }

    if (m_hanoi.stacks == nullptr
        || m_hanoi.stacks_arr_size != Config::get().Settings().stack_amount) {
        throw std::runtime_error(
            "GameView::initStacks(): failed to initialize stacks");
    }

    //-------------------------------------------------------------------------

    // scale the stacks
    scaleStack();
}

void
GameView::initSlices()
{
    //-------------------------------------------------------------------------

    // make sure the stack is initialized before the slices

    if (m_hanoi.stacks == nullptr) {
        std::runtime_error("GameView::initSlices(): Error, attempted to "
                           "initialize slices before stacks");
    }

    //-------------------------------------------------------------------------

    // tint the slice sprite
    if (m_sprites.slice_tint != Config::get().Theme().slice_tint) {
        QPixmap sprite(Config::get().AssetFiles().SLICE);
        colorizeSprite(&sprite, Config::get().Theme().slice_tint);
        m_sprites.slice      = sprite;
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

    if (m_hanoi.slices_arr_size != Config::get().Settings().slice_amount) {
        if (m_hanoi.slices != nullptr) { delete[] m_hanoi.slices; }
        m_hanoi.slices
            = new HanoiSlice *[Config::get().Settings().slice_amount];
        m_hanoi.slices_arr_size = Config::get().Settings().slice_amount;
    }

    if (m_hanoi.slices == nullptr
        || m_hanoi.slices_arr_size != Config::get().Settings().slice_amount) {
        throw std::runtime_error(
            "GameView::initStacks(): failed to initialize slices");
    }

    // fill the array with default values
    std::memset(m_hanoi.slices, 0, Config::get().Settings().slice_amount);

    // save the slices to the array
    HanoiSlice *slice = m_hanoi.stacks[0].getHead();
    while (slice != nullptr) {
        m_hanoi.slices[slice->getValue()] = slice;
        slice                             = slice->next;
    }

    //-------------------------------------------------------------------------

    // scale the slices
    scaleSlices();
}

void
GameView::initGoalStack()
{
    const size_t goalStackLabel = getRandomGoalStackIndex();
    m_hanoi.goal_stack          = nullptr;
    m_hanoi.goal_stack          = &m_hanoi.stacks[goalStackLabel];
    if (m_hanoi.goal_stack == nullptr
        || m_hanoi.goal_stack->label() != goalStackLabel) {
        std::runtime_error("clear(): failed to init goal stack");
    }
}
