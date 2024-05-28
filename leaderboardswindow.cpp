#include "leaderboardswindow.h"

#include "ui_leaderboardswindow.h"

// Leaderboards should be read from files, and saved to files.
// must have an api to quickly add a score, grouped by game setup
// and is sorted

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
