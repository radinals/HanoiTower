//-- Description -------------------------------------------------------------/
// The Slice of the Hanoi Stack, tracks the slice's sprites on the screen and /
// size.
//----------------------------------------------------------------------------/

#ifndef HANOISLICE_H
#define HANOISLICE_H

#include <QString>

class HanoiSlice {
private:
    struct Geometry_t;

public:
    HanoiSlice(HanoiSlice&&) = delete;
    HanoiSlice() {};
    HanoiSlice(unsigned int value) : m_value(value) {};

    inline unsigned int getValue() const { return m_value; }

    inline const Geometry_t& Geometry() const { return m_geometry; }
    inline Geometry_t&       Geometry() { return m_geometry; }

    const HanoiSlice* const& next() const { return m_next; }
    HanoiSlice*&             next() { return m_next; }

    const HanoiSlice* const& prev() const { return m_prev; }
    HanoiSlice*&             prev() { return m_prev; }

private:
    unsigned int m_value = 0;

    struct Geometry_t {
        float width = 0, height = 0;
    } m_geometry;

    HanoiSlice* m_next = nullptr;
    HanoiSlice* m_prev = nullptr;
};

#endif    // HANOISLICE_H
