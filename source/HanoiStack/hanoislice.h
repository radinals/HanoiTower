//-- Description -------------------------------------------------------------/
// The Slice of the Hanoi Stack, tracks the slice's sprites on the screen and /
// size.
//----------------------------------------------------------------------------/

#ifndef HANOISLICE_H
#define HANOISLICE_H

#include <cstddef>

class HanoiSlice {
private:
    struct Geometry_t;

public:
    HanoiSlice() {};
    HanoiSlice(size_t value) : m_value(value) {};

    inline size_t       getLabel() const { return m_value; }
    inline float&       Height() { return m_width; }
    inline float&       Width() { return m_height; }
    inline HanoiSlice*& Next() { return m_next; }
    inline HanoiSlice*& Prev() { return m_prev; }

private:
    size_t m_value = 0;
    float  m_width = 0, m_height = 0;

    HanoiSlice* m_next = nullptr;
    HanoiSlice* m_prev = nullptr;
};

#endif    // HANOISLICE_H
