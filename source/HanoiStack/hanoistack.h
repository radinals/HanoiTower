//-- Description -------------------------------------------------------------/
// The Stack Implementation for Hanoi Tower, is structured to be used also as /
// simplified double linked list.                                             /
// Stores the very basic information of a stack instance.                     /
//----------------------------------------------------------------------------/

#ifndef HANOISTACK_H
#define HANOISTACK_H

#include "hanoislice.h"

#include <cstddef>
#include <functional>

class HanoiStack {
public:
    HanoiStack() {};
    HanoiStack(size_t label) : m_label(label) {};
    ~HanoiStack() { clearStack(); };

    void clearStack();
    void push(HanoiSlice* slice);

    HanoiSlice*             pop();
    const HanoiSlice* const peek() const { return m_head; }

    inline HanoiSlice*& getTail() { return m_tail; }
    inline HanoiSlice*& getTop() { return m_head; }

    inline size_t getSize() const { return m_size; };
    inline bool   isEmpty() const { return m_size <= 0; }

    // initialize a stack with 'slice_amount' of slices
    static void fillStack(HanoiStack* stack, size_t slice_amount);

    inline const size_t& getLabel() const { return m_label; };

    void forEverySlice(const std::function<void(HanoiSlice*&)>& func);
    void forEverySliceReversed(const std::function<void(HanoiSlice*&)>& func);

private:
    size_t m_size  = 0;
    size_t m_label = 0;

    HanoiSlice *m_head = nullptr, *m_tail = nullptr;
};

#endif    // HANOISTACK_H
