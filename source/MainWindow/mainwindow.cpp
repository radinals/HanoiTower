//-- Description -------------------------------------------------------------/
// Defines the game's main window, and handles most of the buttons and other  /
// interactions with the user.                                                /
//----------------------------------------------------------------------------/

#include "mainwindow.h"

#include "../GameView/gameview.h"
#include "../SettingsWindow/settingswindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet(Config::get().getDefaultStylesheet());

    m_game_view = new GameView(ui->GameViewFrame);
    m_game_view->setGameInfoOutputs(ui->GameTimer,
                                    ui->GameMoveCountOut,
                                    ui->GameInfoBox);
    m_settings_window = new SettingsWindow;

    ui->SettingsViewFrame->layout()->addWidget(m_settings_window);
    ui->GameViewFrame->layout()->addWidget(m_game_view);

    connect(ui->ResetBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::resetGameAction);

    connect(ui->BackToMenuBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::backToMainMenuAction);

    connect(ui->ExitBtn, &QPushButton::clicked, this, &MainWindow::exitAction);

    connect(ui->StartExitBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::exitAction);

    connect(m_settings_window,
            &SettingsWindow::hidden,
            this,
            &MainWindow::backToMainMenuAction);

    ui->GameTitle->setPixmap(m_logo);

    ui->GameSideBarFrame->hide();
    ui->SettingsViewFrame->hide();
    ui->GameViewFrame->hide();
    ui->GameMenuFrame->show();

#ifndef DISABLE_AUDIO
    Config::get().setBackgroundMusicInstance(m_background_music.m_audio_output);
    m_background_music.m_media_player->play();
#endif
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
    if (m_game_view->isPaused()) { ui->PauseBtn->setText("PAUSE"); }
    m_game_view->reset();
}

void
MainWindow::on_StartBtn_clicked()
{
    ui->GameMenuFrame->hide();
    ui->GameSideBarFrame->show();
    ui->GameViewFrame->show();
    m_game_view->reset();
}

void
MainWindow::on_SettingsBtn_clicked()
{
    ui->GameSideBarFrame->hide();
    ui->GameViewFrame->hide();
    ui->GameMenuFrame->hide();
    ui->SettingsViewFrame->show();
    m_settings_window->show();
}

void
MainWindow::exitAction()
{
    if (m_game_view->isAutoSolving()) return;
    close();
}

void
MainWindow::backToMainMenuAction()
{
    if (m_game_view->isAutoSolving()) return;
    m_game_view->clear();
    ui->SettingsViewFrame->hide();
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

void
MainWindow::on_AutoSolveBtn_clicked()
{
    if (m_game_view->isAutoSolving() || m_game_view->isTimerRunning()) return;
    m_game_view->solve();
}
