#include "gamewindow.h"

#include <QSizePolicy>
GameWindow::GameWindow(QWidget *parent) : QMainWindow{parent}
{
	setGeometry(0, 0, 1366, 768);
	m_gameview = new GameView(this);
	m_gameview->setGeometry(geometry());
}
