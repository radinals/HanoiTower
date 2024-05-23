#include "hanoistack.h"

#include <stdexcept>

void
HanoiStack::clearStack()
{
	while (!isEmpty()) {
		delete pop();
	}
}

QPixmap*
HanoiStack::generateSliceSprite(unsigned int color)
{
	return new QPixmap();
}

void
HanoiStack::push(HanoiSlice* slice)
{
	if (isFull()) {
		throw std::length_error("Stack Is Full");
	}

	if (isEmpty()) {
		m_head = slice;
	} else {
		HanoiSlice* old_head = m_head;
		slice->next = old_head;
		m_head = slice;
	}
	m_stack_slice_count++;
}

void
HanoiStack::push(HanoiStack* stack, unsigned int color, QVector2D coordinate)
{
	QPixmap* sprite_pixmap = generateSliceSprite(color);
	HanoiSlice* nw_slice = new HanoiSlice(sprite_pixmap, color);
	nw_slice->setCoordinate(coordinate);
	stack->push(nw_slice);
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
	--m_stack_slice_count;

	return popped;
}

bool
HanoiStack::isValidMove(HanoiSlice* src_top, HanoiSlice* dest_top)
{
	return dest_top->getValue() < src_top->getValue();
}

void
HanoiStack::setStackFull(HanoiStack* stack)
{
	push(stack, CLR_RED, QVector2D(0, 0));
	push(stack, CLR_GREEN, QVector2D(0, 0));
	push(stack, CLR_BLUE, QVector2D(0, 0));
	push(stack, CLR_YELLOW, QVector2D(0, 0));
	push(stack, CLR_PURPLE, QVector2D(0, 0));
}
