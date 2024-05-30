#include "mainwindow.h"

#include "gameview.h"
#include "leaderboardswindow.h"
#include "settingswindow.h"
#include "soundplayer.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <qpushbutton.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QFile file(Config::get().getDefaultStyleSheet());
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

	connect(m_game_view->m_gameover_dialog_no_btn, &QPushButton::clicked,
	        this, &MainWindow::dummy_reset_btn);

	connect(m_game_view->m_gameover_dialog_yes_btn, &QPushButton::clicked,
	        this, &MainWindow::dummy_btn);

	m_bg_music.setSource(Config::get().getBgMusicAudioPath());
	m_bg_music.getSound()->setVolume(
	    Config::get().getAudioMusicVolumeLevel());
	m_bg_music.getSound()->setLoopCount(QSoundEffect::Infinite);

	Config::get().setBgMusicPlayerInstance(&m_bg_music);

	m_settings_window->hide();
	ui->GameSideBarFrame->hide();
	ui->LeaderboardsFrame->hide();
	ui->GameViewFrame->hide();
	ui->GameMenuFrame->show();

	m_bg_music.getSound()->play();
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
	m_game_view->init();
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
