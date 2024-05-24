#include "gamewindow.h"

#include <QPushButton>
#include <QSizePolicy>

GameWindow::GameWindow(QWidget *parent) : QMainWindow{parent}
{
	setMinimumSize(QSize(1366, 768));
	setMaximumSize(QSize(1920, 1080));

	layout()->setGeometry(geometry());
	layout()->setAlignment(Qt::AlignHCenter & Qt::AlignVCenter);

	m_gameview = new GameView();
	m_reset_button = new QPushButton();

	connect(m_reset_button, &QPushButton::pressed, this,
		&GameWindow::resetGame);

	// layout()->addWidget(m_gameview);
	setCentralWidget(m_gameview);
	// layout()->addWidget(m_reset_button);
}

void
GameWindow::resetGame()
{
	m_gameview->resetGame();
}
