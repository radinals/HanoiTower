#ifndef LEADERBOARDSWINDOW_H
#define LEADERBOARDSWINDOW_H

#include <QPushButton>
#include <QWidget>

namespace Ui
{
	class LeaderboardsWindow;
}

class LeaderboardsWindow : public QWidget
{
        Q_OBJECT

      public:
        explicit LeaderboardsWindow(QWidget *parent = nullptr);
        ~LeaderboardsWindow();
        QPushButton *m_back_btn = nullptr;

      private slots:

      private:
        Ui::LeaderboardsWindow *ui;
};

#endif // LEADERBOARDSWINDOW_H
