#include "mainwindow.h"

#include "gameview.h"
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
	m_game_view->setGameInfoOutputs(ui->GameTimer, ui->GameMoveCountOut,
	                                ui->GameInfoBox);
	m_settings_window = new SettingsWindow;

	ui->GameViewFrame->layout()->addWidget(m_game_view);

	connect(m_game_view->m_gameover_dialog_no_btn, &QPushButton::clicked,
	        this, &MainWindow::resetGameAction);

	connect(ui->ResetBtn, &QPushButton::clicked, this,
	        &MainWindow::resetGameAction);

	connect(m_game_view->m_gameover_dialog_yes_btn, &QPushButton::clicked,
	        this, &MainWindow::backToMainMenuAction);

	connect(ui->BackToMenuBtn, &QPushButton::clicked, this,
	        &MainWindow::backToMainMenuAction);

	connect(ui->ExitBtn, &QPushButton::clicked, this,
	        &MainWindow::exitAction);

	connect(ui->StartExitBtn, &QPushButton::clicked, this,
	        &MainWindow::exitAction);

	ui->GameTitle->setPixmap(m_logo);

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
}

void
MainWindow::resetGameAction()
{
	m_game_view->reset();
}

void
MainWindow::on_StartBtn_clicked()
{
	if (m_settings_window->isVisible()) {
		return;
	}

	ui->GameMenuFrame->hide();
	ui->GameSideBarFrame->show();
	ui->GameViewFrame->show();
	m_game_view->reset();
}

void
MainWindow::on_SettingsBtn_clicked()
{
	if (m_settings_window->isVisible()) {
		return;
	}

	m_settings_window->show();
}

void
MainWindow::exitAction()
{
	if (m_settings_window->isVisible()) {
		m_settings_window->close();
	}
	close();
}

void
MainWindow::backToMainMenuAction()
{
	m_game_view->clear();
	ui->LeaderboardsFrame->hide();
	ui->GameSideBarFrame->hide();
	ui->GameViewFrame->hide();
	ui->GameMenuFrame->show();
}

void
MainWindow::on_PauseBtn_clicked()
{
	m_game_view->pause();
	if (m_game_view->isPaused()) {
		ui->PauseBtn->setText("UN-PAUSE");
	} else {
		ui->PauseBtn->setText("PAUSE");
	}
}
