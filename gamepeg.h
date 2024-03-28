#ifndef GAMEPEG_H
#define GAMEPEG_H

#include "GameSlice.h"
#include "sprite.h"
#include "stack.h"

class GamePeg
{
      private:
        Stack* stack = nullptr;
        int stack_sum = 0;
        Sprite sprite;
        void updateSprite();

      public:
        GamePeg();
        ~GamePeg();
        void pushToStack(GameSliceColor);
        void setPegFull();
        void setPegEmpty();
        int pegVal();
        int pegTopVal();
        bool pegIsFull();
        bool pegIsEmpty();
        GameSliceColor popFromStack();
        Sprite& getSprite();
};

#endif // GAMEPEG_H
