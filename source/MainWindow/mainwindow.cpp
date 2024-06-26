//-- Description -------------------------------------------------------------/
// Defines the game's main window, and handles most of the buttons and other  /
// interactions with the user.                                                /
//----------------------------------------------------------------------------/

#include "mainwindow.h"

#include "../GameWindow/gamewindow.h"
#include "../SettingsWindow/settingswindow.h"
#include "../Utils/utils.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet(Utils::getDefaultStylesheet());

    m_settings_window = new SettingsWindow(this);
    m_game_window     = new GameWindow(this);

    ui->WidgetFrame->layout()->addWidget(m_settings_window);
    ui->WidgetFrame->layout()->addWidget(m_game_window);

    ui->GameTitle->setPixmap(m_logo);

    ui->WidgetFrame->hide();
    ui->GameMenuFrame->show();
    m_game_window->hide();
    m_settings_window->hide();

#ifndef DISABLE_AUDIO
    Config::m_bg_music_output = m_background_music.m_audio_output;
    m_background_music.m_media_player->play();
#endif

    // game window slots
    connect(m_game_window,
            &GameWindow::s_back_to_main_menu,
            this,
            &MainWindow::openMainMenu);

    connect(m_game_window,
            &GameWindow::s_open_settings,
            this,
            &MainWindow::openSettingsMenu);

    connect(m_game_window,
            &GameWindow::s_exit_game,
            this,
            &MainWindow::exitGame);

    // setting window slots
    connect(m_settings_window,
            &SettingsWindow::s_hidden,
            this,
            &MainWindow::settingsWindowCloseEvent);

    // main window slots
    connect(ui->StartExitBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::exitGame);

    connect(ui->StartBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::openGameView);

    connect(ui->SettingsBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::openSettingsMenu);
}

MainWindow::~MainWindow()
{
    delete m_game_window;
    delete m_settings_window;
    delete ui;
}

void
MainWindow::settingsWindowCloseEvent()
{
    if (m_game_window->isSettingsBtnPressed()) {
        openGameView();
        m_game_window->isSettingsBtnPressed() = false;
    } else {
        openMainMenu();
    }
}

void
MainWindow::exitGame()
{
    close();
}

void
MainWindow::openMainMenu()
{
    ui->GameMenuFrame->show();
    ui->WidgetFrame->hide();
    m_settings_window->hide();
    m_game_window->hide();
}

void
MainWindow::openGameView()
{
    ui->GameMenuFrame->hide();
    ui->WidgetFrame->show();
    m_game_window->show();
}

void
MainWindow::openSettingsMenu()
{
    ui->GameMenuFrame->hide();
    ui->WidgetFrame->show();
    m_game_window->hide();
    m_settings_window->show();
}
