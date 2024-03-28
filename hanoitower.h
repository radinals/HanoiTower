#ifndef HANOITOWER_H
#define HANOITOWER_H

#include "gamepeg.h"

enum GameActions {
	POP_A_TO_B,
	POP_A_TO_C,
	POP_B_TO_A,
	POP_B_TO_C,
	POP_C_TO_A,
	POP_C_TO_B,
	RESTART_GAME,
};

class HanoiTower
{
      private:
        GamePeg *gamePegA;
        GamePeg *gamePegB;
        GamePeg *gamePegC;
        static bool TopSlice_Peg_X_MoreThan_Y(GamePeg *&x, GamePeg *&y);
        static void PopTop_X_To_Y(GamePeg *&x, GamePeg *&y);

      public:
        HanoiTower();
        ~HanoiTower();
        void doAction(GameActions);
        void initPegs();
        bool gameIsWon();
        QPixmap &getPegAPixmap();
        QPixmap &getPegBPixmap();
        QPixmap &getPegCPixmap();
};

#endif // HANOITOWER_H
