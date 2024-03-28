#ifndef SLICE_H
#define SLICE_H

class Slice
{
      private:
        int val = 0;
        Slice *next = nullptr;

      public:
        Slice(int val) : val(val) {}
        void setVal(int val);
        int getVal();
        void setNext(Slice *next);
        Slice *getNext();
        bool nextIsEmpty();
};

#endif // SLICE_H
