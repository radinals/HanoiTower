#include "gamepeg.h"

#include "GameSlice.h"
#include "sprite.h"

GamePeg::GamePeg()
{
	stack = new Stack();
}

GamePeg::~GamePeg()
{
	delete stack;
}

void
GamePeg::pushToStack(GameSliceColor slice_color) // push slice to stack
{
	stack->push(slice_color);
	stack_sum += slice_color;
}

void
GamePeg::setPegFull() // push all possible value to the stack
{
	pushToStack(SLCOLOR_YELLOW);
	pushToStack(SLCOLOR_GREEN);
	pushToStack(SLCOLOR_BLUE);
	pushToStack(SLCOLOR_RED);

	stack_sum =
	    (SLCOLOR_RED + SLCOLOR_BLUE + SLCOLOR_GREEN + SLCOLOR_YELLOW);

	updateSprite();
}

int
GamePeg::pegVal() // get the total value of the stack
{
	return stack_sum;
}

int
GamePeg::pegTopVal() // get the value of the top slice
{
	return stack->peek();
}

void
GamePeg::setPegEmpty() // clear the stack
{
	if (stack->getSliceCount() > 0)
		stack->clearStack();
	updateSprite();
	stack_sum = 0;
}

bool
GamePeg::pegIsFull() // compare stack value with the sum of all possible value
{
	return (stack_sum ==
		(SLCOLOR_RED + SLCOLOR_BLUE + SLCOLOR_GREEN + SLCOLOR_YELLOW));
}

bool
GamePeg::pegIsEmpty() // check if the stack is empty
{
	return (stack_sum <= 0);
}

void
GamePeg::updateSprite() // set the sprite based of the stack value
{
	switch (stack_sum) {
	case SLCOLOR_RED:
		sprite.setSpritePixmap(":/sprite/sprites/sprite_red_only.png");
		break;
	case SLCOLOR_BLUE:
		sprite.setSpritePixmap(":/sprite/sprites/sprite_blue_only.png");
		break;
	case SLCOLOR_GREEN:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_green_only.png");
		break;
	case SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_yellow_only.png");
		break;
	case SLCOLOR_RED + SLCOLOR_GREEN:
		sprite.setSpritePixmap(":/sprite/sprites/sprite_red_green.png");
		break;
	case SLCOLOR_RED + SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_red_yellow.png");
		break;
	case SLCOLOR_RED + SLCOLOR_BLUE:
		sprite.setSpritePixmap(":/sprite/sprites/sprite_red_blue.png");
		break;
	case SLCOLOR_BLUE + SLCOLOR_GREEN:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_blue_green.png");
		break;
	case SLCOLOR_GREEN + SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_green_yellow.png");
		break;
	case SLCOLOR_BLUE + SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_blue_yellow.png");
		break;
	case SLCOLOR_BLUE + SLCOLOR_GREEN + SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_blue_green_yellow.png");
		break;
	case SLCOLOR_RED + SLCOLOR_BLUE + SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_red_blue_yellow.png");
		break;
	case SLCOLOR_RED + SLCOLOR_BLUE + SLCOLOR_GREEN:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_red_blue_green.png");
		break;
	case SLCOLOR_RED + SLCOLOR_GREEN + SLCOLOR_YELLOW:
		sprite.setSpritePixmap(
		    ":/sprite/sprites/sprite_red_green_yellow.png");
		break;
	default:
		if (pegIsFull())
			sprite.setSpritePixmap(
			    ":/sprite/sprites/sprite_full.png");
		else
			sprite.setSpritePixmap(
			    ":/sprite/sprites/sprite_empty.png");
	};
}

GameSliceColor
GamePeg::popFromStack() // pop from the stack, decrement the stack value
{
	GameSliceColor color = GameSliceColor(stack->pop());
	stack_sum -= color;
	return color;
}

Sprite&
GamePeg::getSprite() // update and return the stack sprite
{
	updateSprite();
	return sprite;
}
