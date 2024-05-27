#include "mainwindow.h"

#include "gameview.h"
#include "leaderboardswindow.h"
#include "settingswindow.h"
#include "ui_mainwindow.h"

#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
        ui->setupUi(this);

	QFile file(":/style/default.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);

	m_game_view = new GameView(ui->GameViewFrame);
	m_game_view->setGameInfoOutputs(ui->GameTimer, ui->GameScoreOut,
					ui->GameMoveCountOut);
	m_settings_window = new SettingsWindow;
	m_leaderboards_window = new LeaderboardsWindow();

	ui->GameViewFrame->layout()->addWidget(m_game_view);
	ui->LeaderboardsFrame->layout()->addWidget(m_leaderboards_window);

	connect(m_leaderboards_window->m_back_btn, &QPushButton::clicked, this,
		&MainWindow::dummy_btn);

	m_settings_window->hide();
	ui->GameSideBarFrame->hide();
	ui->LeaderboardsFrame->hide();
	ui->GameViewFrame->hide();
	ui->GameMenuFrame->show();
}

MainWindow::~MainWindow()
{
	delete ui;
	delete m_game_view;
	delete m_settings_window;
	delete m_leaderboards_window;
}

void
MainWindow::on_ResetBtn_clicked()
{
	if (!m_game_started) {
		m_game_view->init();
		m_game_started = true;
	}
}

void
MainWindow::on_ExitBtn_clicked()
{
	exit(1);
}

void
MainWindow::on_StartBtn_clicked()
{
	ui->GameMenuFrame->hide();
	ui->GameSideBarFrame->show();
	ui->GameViewFrame->show();
	m_game_view->init();
}

void
MainWindow::on_SettingsBtn_clicked()
{
	m_settings_window->show();
}

void
MainWindow::on_StartExitBtn_clicked()
{
	exit(1);
}

void
MainWindow::on_BackToMenuBtn_clicked()
{
	m_game_view->init();
	ui->LeaderboardsFrame->hide();
	ui->GameSideBarFrame->hide();
	ui->GameViewFrame->hide();
	ui->GameMenuFrame->show();
}

void
MainWindow::on_LeaderboardsBtn_clicked()
{
	ui->GameSideBarFrame->hide();
	ui->GameViewFrame->hide();
	ui->GameMenuFrame->hide();

	ui->LeaderboardsFrame->show();
}
