#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gameview.h"
#include "settingswindow.h"

#include <QMainWindow>

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT
      private:
        bool m_game_started = false;
        SettingsWindow *m_settings_window;

      public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        GameView *m_game_view = nullptr;

      private slots:
        void on_ResetBtn_clicked();

        void on_ExitBtn_clicked();

        void on_StartBtn_clicked();

        void on_SettingsBtn_clicked();

        void on_StartExitBtn_clicked();

        void on_BackToMenuBtn_clicked();

      private:
        Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
