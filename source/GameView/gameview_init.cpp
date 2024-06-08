//-- Description -------------------------------------------------------------/
// initializing methods
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"
#include <cassert>
#include <stdexcept>

void
GameView::init_stacks()
{
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

    assert(m_hanoi.stacks != nullptr);
    assert(m_hanoi.stacks_arr_size == Config::get().Settings().stack_amount);

    if (Config::get().Settings().slice_amount <= 0) {
        std::runtime_error("clear(): invalid slice amount");
    }

    // populate the first stack
    HanoiStack::fillStack(&m_hanoi.stacks[0],
                          Config::get().Settings().slice_amount);

    if (m_hanoi.stacks[0].getSize() != Config::get().Settings().slice_amount) {
        std::runtime_error("clear(): failed to fill starting stack");
    }

    const size_t goalStackLabel = getRandomGoalStackIndex();
    m_hanoi.goal_stack          = nullptr;
    m_hanoi.goal_stack          = &m_hanoi.stacks[goalStackLabel];
    if (m_hanoi.goal_stack == nullptr
        || m_hanoi.goal_stack->label() != goalStackLabel) {
        std::runtime_error("clear(): failed to init goal stack");
    }

    // scale the stacks
    scaleStack();
}

void
GameView::init_slices()
{
    if (m_hanoi.slices_arr_size != Config::get().Settings().slice_amount) {
        if (m_hanoi.slices != nullptr) { delete[] m_hanoi.slices; }
        m_hanoi.slices
            = new HanoiSlice *[Config::get().Settings().slice_amount];
        m_hanoi.slices_arr_size = Config::get().Settings().slice_amount;
    }

    assert(m_hanoi.slices != nullptr);
    assert(m_hanoi.slices_arr_size == Config::get().Settings().slice_amount);

    std::memset(m_hanoi.slices, 0, Config::get().Settings().slice_amount);

    // tint the slice sprite
    if (m_sprites.slice_tint != Config::get().Theme().slice_tint) {
        QPixmap sprite(Config::get().AssetFiles().SLICE);
        colorizeSprite(&sprite, Config::get().Theme().slice_tint);
        m_sprites.slice      = sprite;
        m_sprites.slice_tint = Config::get().Theme().slice_tint;
    }

    HanoiSlice *slice = m_hanoi.stacks[0].getHead();
    while (slice != nullptr) {
        // save the slices for ease of access
        m_hanoi.slices[slice->getValue()] = slice;
        slice                             = slice->next;
    }

    // scale the slices
    scaleSlices();
}
