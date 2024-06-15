//-- Description -------------------------------------------------------------/
// Defines the game's main window, and handles most of the buttons and other  /
// interactions with the user.                                                /
//----------------------------------------------------------------------------/

#include "mainwindow.h"

#include "../GameWindow/gamewindow.h"
#include "../SettingsWindow/settingswindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet(Config::get().getDefaultStylesheet());

    m_settings_window = new SettingsWindow(this);
    m_game_window     = new GameWindow(this);

    ui->SettingsViewFrame->layout()->addWidget(m_settings_window);
    ui->GameViewFrame->layout()->addWidget(m_game_window);

    connect(m_game_window,
            &GameWindow::back_to_menu,
            this,
            &MainWindow::backToMainMenuAction);

    connect(m_game_window,
            &GameWindow::quit_game,
            this,
            &MainWindow::exitAction);

    connect(ui->StartExitBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::exitAction);

    connect(m_settings_window,
            &SettingsWindow::hidden,
            this,
            &MainWindow::on_SettingsShowEvent);

    connect(m_game_window,
            &GameWindow::open_settings,
            this,
            &MainWindow::on_SettingsBtn_clicked);

    ui->GameTitle->setPixmap(m_logo);
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
    delete m_game_window;
    delete m_settings_window;
    delete ui;
}

void
MainWindow::on_SettingsShowEvent()
{
    if (m_game_window->isSettingsBtnPressed()) {
        on_StartBtn_clicked();
        m_game_window->isSettingsBtnPressed() = false;
    } else {
        backToMainMenuAction();
    }
}

void
MainWindow::on_StartBtn_clicked()
{
    ui->GameMenuFrame->hide();
    ui->GameViewFrame->show();
}

void
MainWindow::on_SettingsBtn_clicked()
{
    ui->GameViewFrame->hide();
    ui->GameMenuFrame->hide();
    ui->SettingsViewFrame->show();
    m_settings_window->show();
}

void
MainWindow::exitAction()
{
    close();
}

void
MainWindow::backToMainMenuAction()
{
    ui->SettingsViewFrame->hide();
    ui->GameViewFrame->hide();
    ui->GameMenuFrame->show();
}
