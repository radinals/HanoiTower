//-- Description -------------------------------------------------------------/
// method that define the operations on a Hanoi Stack                         /
//----------------------------------------------------------------------------/

#include "hanoistack.h"

#include "hanoislice.h"
#include <cassert>
#include <functional>
#include <stdexcept>

void
HanoiStack::clearStack()
{
    while (!isEmpty()) { delete pop(); }
    m_head = nullptr;
    m_tail = nullptr;
}

void
HanoiStack::push(HanoiSlice* slice)
{
    assert(slice != nullptr);

    if (isEmpty()) {
        m_head = slice;
        m_tail = m_head;
    } else if (slice->getLabel() < peek()->getLabel()) {
        throw std::runtime_error(
            "HanoiStack::push(): tried to move a larger slice on top a smaller "
            "slice.");
    } else {
        slice->Next()  = m_head;
        slice->Prev()  = nullptr;
        m_head->Prev() = slice;
        m_head         = slice;

        assert(m_head != nullptr);
        assert(m_head->Next() != nullptr);
    }
    m_size++;
}

void
HanoiStack::fillStack(HanoiStack* const stack, size_t amount)
{
    for (size_t i = 0; i < amount; i++) { stack->push(new HanoiSlice(i)); }
}

HanoiSlice*
HanoiStack::pop()
{
    if (isEmpty()) {
        throw std::out_of_range("HanoiStack::pop(): Stack Is Empty");
    }

    HanoiSlice* popped = m_head;

    m_head = m_head->Next();

    if (m_head != nullptr) { m_head->Prev() = nullptr; }

    --m_size;

    popped->Next() = popped->Prev() = nullptr;

    return popped;
}

void
HanoiStack::forEverySlice(const std::function<void(HanoiSlice*&)>& func)
{
    HanoiSlice* slice = m_head;
    while (slice != nullptr) {
        func(slice);
        slice = slice->Next();
    }
}

void
HanoiStack::forEverySliceReversed(const std::function<void(HanoiSlice*&)>& func)
{
    HanoiSlice* slice = m_tail;
    while (slice != nullptr) {
        func(slice);
        slice = slice->Prev();
    }
}
