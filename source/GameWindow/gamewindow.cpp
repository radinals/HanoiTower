//-- Description -------------------------------------------------------------/
// the game's view widget, holds the GameView and also it's sidebar           /
//----------------------------------------------------------------------------/

#include "gamewindow.h"
#include <QPushButton>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);
    m_game_view = new GameView(ui->GameDisplayFrame);
    ui->InfoOut->setReadOnly(true);
    m_game_view->setGameInfoOutputs(ui->TimerOut,
                                    ui->MoveCountOut,
                                    ui->InfoLabel,
                                    ui->InfoOut);

    // clang-format off
    connect(ui->AutoSolveBtn, &QPushButton::clicked, m_game_view, &GameView::solve);
    connect(ui->ResetBtn, &QPushButton::clicked, m_game_view, &GameView::reset);
    connect(ui->PauseBtn, &QPushButton::clicked, m_game_view, &GameView::pause);
    // clang-format on

    ui->GameDisplayFrame->layout()->addWidget(m_game_view);
}

GameWindow::~GameWindow()
{
    delete m_game_view;
    delete ui;
}

void
GameWindow::on_BackToMenuBtn_clicked()
{
    m_game_view->reset();
    emit(back_to_menu());
}

void
GameWindow::on_QuitGameBtn_clicked()
{
    emit(quit_game());
}

void
GameWindow::on_OpenSettingsBtn_clicked()
{
    m_settings_btn_pressed = true;
    m_game_view->reset();
    emit(open_settings());
}
