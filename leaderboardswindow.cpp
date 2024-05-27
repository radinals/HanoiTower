#include "leaderboardswindow.h"

#include "ui_leaderboardswindow.h"

LeaderboardsWindow::LeaderboardsWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::LeaderboardsWindow)
{
        ui->setupUi(this);

        m_back_btn = ui->BackToMenuBtn;
}

LeaderboardsWindow::~LeaderboardsWindow()
{
        delete ui;
}
