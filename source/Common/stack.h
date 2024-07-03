#ifndef STACK_H
#define STACK_H

#include <stdexcept>
#include <utility>

template<typename T> class Stack {
public:
    Stack() {};
    ~Stack() { clear(); };

    const T& getTop()
    {
        if (isEmpty()) { throw std::out_of_range("Stack Is Empty"); }

        return m_head->m_data;
    }

    bool isEmpty() const { return m_head == nullptr; }

    void pop()
    {
        if (isEmpty()) throw std::out_of_range("Stack Is Empty");

        Slice_t* popped = m_head;

        m_head = m_head->m_next;

        delete popped;

        popped = nullptr;
    };

    void push(const T& data)
    {
        Slice_t* nw_slice = new Slice_t;
        nw_slice->m_data  = data;

        if (isEmpty()) {
            m_head = nw_slice;
        } else {
            nw_slice->m_next = m_head;
            m_head           = nw_slice;
        }
    }

    void push(const T&& data)
    {
        Slice_t* nw_slice = new Slice_t;
        nw_slice->m_data  = std::move(data);

        if (isEmpty()) {
            m_head = nw_slice;
        } else {
            nw_slice->m_next = m_head;
            m_head           = nw_slice;
        }
    }

    void clear()
    {
        Slice_t* slice = m_head;
        while (slice != nullptr) {
            Slice_t* tmp = slice;
            slice        = slice->m_next;
            delete tmp;
            tmp = nullptr;
        }
        m_head = nullptr;
    }

private:
    struct Slice_t {
        T        m_data;
        Slice_t* m_next = nullptr;
    };

    Slice_t* m_head = nullptr;
};

#endif    // !STACK_H
