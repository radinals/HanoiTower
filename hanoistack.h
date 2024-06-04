#ifndef HANOISTACK_H
#define HANOISTACK_H

#include "config.h"

#include <QPixmap>
#include <QString>
#include <QVector2D>

class HanoiSlice {
private:
    QPixmap m_pixmap = QPixmap(Config::get().getSliceSpritePath());

    unsigned int m_value = 0;
    float        m_x = 0, m_y = 0;
    float        m_w = 0, m_h = 0;

public:
    HanoiSlice(HanoiSlice&&) = delete;
    HanoiSlice() {};
    HanoiSlice(unsigned int value) : m_value(value) {};

    inline QPixmap& getPixmap() { return m_pixmap; }
    inline QPixmap getScaledPixmap() const { return m_pixmap.scaled(m_w, m_h); }

    inline float getWidth() const { return m_w; }
    inline float getHeight() const { return m_h; }
    inline void  setWidth(float w) { m_w = w; }
    inline void  setHeight(float h) { m_h = h; }

    inline float getX() const { return m_x; }
    inline float getY() const { return m_y; }
    inline void  setX(float x) { m_x = x; }
    inline void  setY(float y) { m_y = y; }

    inline unsigned int getValue() const { return m_value; }

    HanoiSlice* next = nullptr;
    HanoiSlice* prev = nullptr;
};

class HanoiStack {
private:
    unsigned int m_stack_colors      = 0;
    size_t       m_stack_slice_count = 0;

    HanoiSlice *m_head = nullptr, *m_tail = nullptr;

public:
    HanoiStack() {};
    // HanoiStack(HanoiStack&&) = delete;
    ~HanoiStack() { clearStack(); };

    void clearStack();
    void push(HanoiSlice* slice);

    HanoiSlice*             pop();
    const HanoiSlice* const peek() const;

    inline HanoiSlice* const getTail() { return m_tail; }
    inline HanoiSlice* const getHead() { return m_head; }
    inline size_t getSliceCount() const { return m_stack_slice_count; };
    inline bool   isEmpty() const { return m_stack_slice_count <= 0; }

    static void initializeStack(HanoiStack* stack, size_t slice_amount);
};

#endif    // HANOISTACK_H
