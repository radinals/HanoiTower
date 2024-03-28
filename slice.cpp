#include "slice.h"

void
Slice::setVal(int val)
{
	this->val = val;
}

int
Slice::getVal()
{
	return this->val;
}

void
Slice::setNext(Slice *next)
{
	this->next = next;
}

Slice *
Slice::getNext()
{
	return this->next;
}

bool
Slice::nextIsEmpty()
{
	return (this->next == nullptr);
}
