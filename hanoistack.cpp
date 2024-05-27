#include "hanoistack.h"

#include "config.h"

#include <QPainter>
#include <stdexcept>

void
HanoiStack::clearStack()
{
	while (!isEmpty()) {
		delete pop();
	}
	m_head = nullptr;
	m_tail = nullptr;
}

void
HanoiStack::push(HanoiSlice* slice)
{
	if (slice == nullptr) {
		throw std::invalid_argument("Attempted to push a null slice");
	}
	if (isEmpty()) {
		m_head = slice;
		m_tail = m_head;
	} else {
		HanoiSlice* old_head = m_head;

		slice->next = old_head;
		old_head->prev = slice;

		m_head = slice;
		m_head->prev = nullptr;
	}
	m_stack_slice_count++;
}

void
HanoiStack::generate_stack(HanoiStack* stack, size_t amount)
{
	for (size_t i = 1; i <= amount; i++) {
		stack->push(new HanoiSlice(i));
	}
}

const HanoiSlice* const
HanoiStack::peek() const
{
	return m_head;
}

HanoiSlice*
HanoiStack::pop()
{
	if (isEmpty()) {
		throw std::out_of_range("Stack is Empty");
	}

	HanoiSlice* popped = m_head;

	m_head = m_head->next;

	if (m_head != nullptr) {
		m_head->prev = nullptr;
	}

	--m_stack_slice_count;

	popped->next = popped->prev = nullptr;

	return popped;
}
