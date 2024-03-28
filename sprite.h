#ifndef SPRITE_H
#define SPRITE_H

#include <QPixmap>

class Sprite
{
      private:
        QPixmap sprite_file;

      public:
        Sprite();
        void setSpritePixmap(QString path);
        QPixmap& getSpritePixmap();
};

#endif // SPRITE_H
