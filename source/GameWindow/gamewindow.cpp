#include "gamewindow.h"

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
    ui->GameDisplayFrame->layout()->addWidget(m_game_view);
}

GameWindow::~GameWindow()
{
    delete m_game_view;
    delete ui;
}

void
GameWindow::on_AutoSolveBtn_clicked()
{
    if (!m_game_view->isAutoSolving()) { m_game_view->solve(); }
}

void
GameWindow::on_ResetBtn_clicked()
{
    m_game_view->reset();
}

void
GameWindow::on_PauseBtn_clicked()
{
    m_game_view->pause();
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
