#ifndef LEADERBOARDSWINDOW_H
#define LEADERBOARDSWINDOW_H

#include <QPushButton>
#include <QWidget>
#include <list>

namespace Ui
{
	class LeaderboardsWindow;
}

class LeaderboardsWindow : public QWidget
{
        Q_OBJECT

      private:
        struct LeaderBoardData {
                std::string m_name;
                size_t m_slice_count = 0, m_stack_count = 0;
                unsigned int score = 0;
                std::string m_date;
        };

        std::list<LeaderBoardData> m_leaderboard_datas;

	void loadFromFile();
	void saveToFile();

      public:
        explicit LeaderboardsWindow(QWidget *parent = nullptr);
        ~LeaderboardsWindow();
        QPushButton *m_back_btn = nullptr;

      private slots:

      private:
        Ui::LeaderboardsWindow *ui;
};

#endif // LEADERBOARDSWINDOW_H
