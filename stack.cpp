#include "stack.h"

#include "slice.h"

#include <stdexcept>

Stack::Stack()
{
}

Stack::~Stack()
{
	clearStack();
}

size_t
Stack::getSliceCount() // amount of slice in the stack
{
	return stack_count;
}

Slice *
Stack::topSlice() // return the ptr to the top slice
{
	return top;
}

int
Stack::pop() // pop the top slice, get value, delete top slice, return value
{
	Slice *popped = topSlice();

	if (popped == nullptr)
		throw std::out_of_range("Stack: nothing on the stack!");

	int popped_val = popped->getVal();

	top = topSlice()->getNext();
	delete popped;

	--stack_count;

	return popped_val;
}

void
Stack::clearStack() // delete all slice
{
	if (topSlice() == nullptr) // cek stack kosong
		return;

	Slice *slice = topSlice();
	while (slice != nullptr) {
		Slice *tmp = slice;
		slice = slice->getNext();
		if (tmp != nullptr)
			delete tmp;
	}
	stack_count = 0;
}

int
Stack::peek() // 'peek' value at top slice
{
	if (stack_count <= 0)
		throw std::out_of_range("Stack: nothing on the stack!");

	return topSlice()->getVal();
}

void
Stack::push(int n) // create a new slice, push to the stack
{
	Slice *new_slice = new Slice(n);
	new_slice->setNext(top);
	top = new_slice;
	stack_count++;
}
