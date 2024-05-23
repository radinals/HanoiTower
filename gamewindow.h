#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "gameview.h"

#include <QHBoxLayout>
#include <QMainWindow>

class GameWindow : public QMainWindow
{
        Q_OBJECT
      private:
        GameView* m_gameview = nullptr;
        QHBoxLayout* m_layout = nullptr;

      public:
        explicit GameWindow(QWidget* parent = nullptr);

      signals:
};

#endif // GAMEWINDOW_H
