//-- Description -------------------------------------------------------------/
// method that define the operations on a Hanoi Stack                         /
//----------------------------------------------------------------------------/

#include "hanoistack.h"

#include "hanoislice.h"
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
    } else if (slice->getValue() < peek()->getValue()) {
        throw HanoiException::IllegalStackMove(
            "HanoiStack::push(): tried to move a larger slice on top a smaller "
            "slice.");
    } else {
        slice->next()  = m_head;
        slice->prev()  = nullptr;
        m_head->prev() = slice;
        m_head         = slice;

        assert(m_head != nullptr);
        assert(m_head->next() != nullptr);
    }
    m_size++;
}

void
HanoiStack::fillStack(HanoiStack* stack, size_t amount)
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

    m_head = m_head->next();

    if (m_head != nullptr) { m_head->prev() = nullptr; }

    --m_size;

    popped->next() = popped->prev() = nullptr;

    return popped;
}
