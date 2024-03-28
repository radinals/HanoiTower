#include "sprite.h"

Sprite::Sprite()
{
}

void
Sprite::setSpritePixmap(QString path)
{
	sprite_file = QPixmap(path);
}

QPixmap&
Sprite::getSpritePixmap()
{
	return sprite_file;
}
