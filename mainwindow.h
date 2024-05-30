#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "gameview.h"
#include "settingswindow.h"
#include "soundplayer.h"

#include <QMainWindow>

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT
      private:
        SettingsWindow *m_settings_window = nullptr;
        SoundPlayer m_bg_music;
        const QPixmap m_logo = QPixmap(Config::get().getProgramLogo());

      public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        GameView *m_game_view = nullptr;

      private slots:
        void dummy_btn(bool _) { on_BackToMenuBtn_clicked(); }
        void dummy_reset_btn(bool _) { on_ResetBtn_clicked(); }

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
