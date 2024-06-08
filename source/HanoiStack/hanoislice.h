#ifndef HANOISLICE_H
#define HANOISLICE_H

#include "../Config/config.h"

#include <QPixmap>
#include <QString>

class HanoiSlice {
private:
    struct Geometry_t;

public:
    HanoiSlice(HanoiSlice&&) = delete;
    HanoiSlice() {};
    HanoiSlice(unsigned int value) : m_value(value) {};

    inline unsigned int getValue() const { return m_value; }
    inline Geometry_t&  Geometry() { return m_geometry; }

    HanoiSlice* next = nullptr;
    HanoiSlice* prev = nullptr;

private:
    unsigned int m_value = 0;

    struct Geometry_t {
        float x = 0, y = 0, width = 0, height = 0;
    } m_geometry;
};

#endif    // HANOISLICE_H
