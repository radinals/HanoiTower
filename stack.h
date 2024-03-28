#ifndef STACK_H
#define STACK_H

#include "slice.h"

#include <cstddef>

class Stack
{
      private:
        Slice* top = nullptr;
        int stack_count = 0;

        Slice* topSlice();

      public:
        Stack();
        ~Stack();
        void push(int n);
        int pop();
        int peek();
        void clearStack();
        size_t getSliceCount();
};

#endif // STACK_H
