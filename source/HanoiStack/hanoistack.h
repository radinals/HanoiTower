#ifndef HANOISTACK_H
#define HANOISTACK_H

#include "hanoislice.h"

#include <QPixmap>
#include <QString>

class HanoiStack {
public:
    HanoiStack() {};
    HanoiStack(size_t label) : m_label(label) {};
    ~HanoiStack() { clearStack(); };

    void clearStack();
    void push(HanoiSlice* slice);

    HanoiSlice*             pop();
    const HanoiSlice* const peek() const;

    inline HanoiSlice* const getTail() { return m_tail; }
    inline HanoiSlice* const getHead() { return m_head; }
    inline size_t            getSize() const { return m_size; };
    inline bool              isEmpty() const { return m_size <= 0; }

    // initialize a stack with 'slice_amount' of slices
    static void initializeStack(HanoiStack* stack, size_t slice_amount);

    size_t m_label = 0;

private:
    size_t m_size = 0;

    HanoiSlice *m_head = nullptr, *m_tail = nullptr;
};

#endif    // HANOISTACK_H
