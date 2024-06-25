//-- Description -------------------------------------------------------------/
// the game's view widget, holds the GameView and also it's sidebar           /
//----------------------------------------------------------------------------/

#include "gamewindow.h"
#include <QMessageBox>
#include <QPushButton>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    m_game_view = GameView::getInstance();

    GameView::setGameInfoOutputs(ui->TimerOut,
                                 ui->MoveCountOut,
                                 ui->InfoLabel,
                                 ui->InfoOut);

    ui->GameDisplayFrame->layout()->addWidget(m_game_view);

    ui->InfoOut->setReadOnly(true);

    //========================================================================

    // clang-format off

    ui->QuitGameBtn->setIcon(QIcon(QPixmap(Config::AssetsFiles::QUIT_BTN_ICON)));
    ui->QuitGameBtn->setIconSize(QSize(ui->QuitGameBtn->size().height(),
                                       ui->QuitGameBtn->size().height()));

    ui->ResetBtn->setIcon(QIcon(QPixmap(Config::AssetsFiles::RESET_BTN_ICON)));
    ui->ResetBtn->setIconSize(QSize(ui->ResetBtn->size().height(),
                                    ui->ResetBtn->size().height()));

    ui->BackToMenuBtn->setIcon(QIcon(QPixmap(Config::AssetsFiles::MENU_BTN_ICON)));
    ui->BackToMenuBtn->setIconSize(QSize(ui->BackToMenuBtn->size().height(),
                                         ui->BackToMenuBtn->size().height()));

    ui->OpenSettingsBtn->setIcon(QIcon(QPixmap(Config::AssetsFiles::SETTINGS_BTN_ICON)));
    ui->OpenSettingsBtn->setIconSize(QSize(ui->OpenSettingsBtn->size().height(),
                                           ui->OpenSettingsBtn->size().height()));

    ui->TimerOut->setIcon(QIcon(QPixmap(Config::AssetsFiles::PAUSE_BTN_ICON)));
    ui->TimerOut->setIconSize(QSize(ui->TimerOut->size().height(),
                                    ui->TimerOut->size().height()));

    ui->AutoSolveBtn->setIcon(QIcon(QPixmap(Config::AssetsFiles::AUTO_SOLVE_BTN_ICON)));
    ui->AutoSolveBtn->setIconSize(QSize(ui->AutoSolveBtn->size().height(),
                                        ui->AutoSolveBtn->size().height()));

    //========================================================================

    connect(ui->AutoSolveBtn, &QPushButton::clicked, m_game_view, &GameView::solve);
    connect(ui->ResetBtn, &QPushButton::clicked, m_game_view, &GameView::reset);
    connect(ui->TimerOut, &QPushButton::clicked, m_game_view, &GameView::pause);

    // clang-format on

    connect(ui->QuitGameBtn, &QPushButton::clicked, m_game_view, [&]() {
        emit(s_exit_game());
    });

    connect(ui->BackToMenuBtn, &QPushButton::clicked, m_game_view, [&]() {
        m_game_view->reset();
        emit(s_back_to_main_menu());
    });

    connect(ui->OpenSettingsBtn, &QPushButton::clicked, m_game_view, [&]() {
        m_settings_btn_pressed = true;
        m_game_view->reset();
        emit(s_open_settings());
    });

    // hide the buttons
    connect(m_game_view, &GameView::s_game_inactive, this, [&]() {
        ui->AutoSolveBtn->setDisabled(false);
    });

    connect(m_game_view, &GameView::s_solver_activated, this, [&]() {
        ui->AutoSolveBtn->setDisabled(true);
    });

    connect(m_game_view, &GameView::s_game_over, this, [&]() {
        ui->AutoSolveBtn->setDisabled(true);
    });

    // show the game paused status
    connect(m_game_view, &GameView::s_unpaused, this, [&]() {
        ui->TimerOut->setIcon(
            QIcon(QPixmap(Config::AssetsFiles::PAUSE_BTN_ICON)));
    });

    connect(m_game_view, &GameView::s_paused, this, [&]() {
        ui->TimerOut->setIcon(
            QIcon(QPixmap(Config::AssetsFiles::PLAY_BTN_ICON)));
    });

    //========================================================================
}

GameWindow::~GameWindow()
{
    GameView::destroyInstance();
    delete ui;
}
