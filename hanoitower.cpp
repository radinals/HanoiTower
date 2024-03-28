#include "hanoitower.h"

#include "GameSlice.h"
#include "gamepeg.h"

HanoiTower::HanoiTower()
{
	gamePegA = new GamePeg();
	gamePegB = new GamePeg();
	gamePegC = new GamePeg();

	initPegs();
}

HanoiTower::~HanoiTower()
{
	delete gamePegA;
	delete gamePegB;
	delete gamePegC;
}

// compare two top slice of the x and y stacks
bool
HanoiTower::TopSlice_Peg_X_MoreThan_Y(GamePeg *&x, GamePeg *&y)
{
	if ((!x->pegIsEmpty()) && y->pegIsEmpty())
		return false;

	if (x->pegIsEmpty() && (!y->pegIsEmpty()))
		return false;

	return (x->pegTopVal() > y->pegTopVal());
}

// pop the top value from x and push it to y
void
HanoiTower::PopTop_X_To_Y(GamePeg *&x, GamePeg *&y)
{
	if (TopSlice_Peg_X_MoreThan_Y(x, y))
		throw std::runtime_error(
		    "HanoiTower: Cannot Pop to lesser Slice!");
	GameSliceColor tmp = x->popFromStack();
	y->pushToStack(tmp);
}

// win state: peg c is full
bool
HanoiTower::gameIsWon()
{
	return gamePegC->pegIsFull();
}

// initialize the pegs, a = full, b and c == empty.
void
HanoiTower::initPegs()
{
	gamePegA->setPegFull();
	gamePegB->setPegEmpty();
	gamePegC->setPegEmpty();
}

// send command to operate the stack
void
HanoiTower::doAction(GameActions action)
{
	switch (action) {
	case POP_A_TO_B:
		PopTop_X_To_Y(gamePegA, gamePegB);
		break;
	case POP_A_TO_C:
		PopTop_X_To_Y(gamePegA, gamePegC);
		break;
	case POP_B_TO_A:
		PopTop_X_To_Y(gamePegB, gamePegA);
		break;
	case POP_B_TO_C:
		PopTop_X_To_Y(gamePegB, gamePegC);
		break;
	case POP_C_TO_A:
		PopTop_X_To_Y(gamePegC, gamePegA);
		break;
	case POP_C_TO_B:
		PopTop_X_To_Y(gamePegC, gamePegB);
		break;
	case RESTART_GAME:
		initPegs();
		break;
	default:
		throw std::runtime_error("HanoiTower: Unknown Action!");
	}
}

// return peg A pixmap
QPixmap &
HanoiTower::getPegAPixmap()
{
	return gamePegA->getSprite().getSpritePixmap();
}

// return peg B pixmap
QPixmap &
HanoiTower::getPegBPixmap()
{
	return gamePegB->getSprite().getSpritePixmap();
}

// return peg C pixmap
QPixmap &
HanoiTower::getPegCPixmap()
{
	return gamePegC->getSprite().getSpritePixmap();
}
