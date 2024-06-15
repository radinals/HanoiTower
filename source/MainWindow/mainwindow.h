//-- Description -------------------------------------------------------------/
// main window that holds all other widgets together                          /
//----------------------------------------------------------------------------/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../Config/config.h"
#include "../GameWindow/gamewindow.h"
#include "../SettingsWindow/settingswindow.h"

#include <QMainWindow>

#ifndef DISABLE_AUDIO
    #include <QAudioOutput>
    #include <QMediaPlayer>
#endif    // !DISABLE_AUDIO

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    SettingsWindow *m_settings_window = nullptr;
    GameWindow     *m_game_window     = nullptr;

    const QPixmap m_logo = QPixmap(Config::get().AssetFiles().LOGO);

#ifndef DISABLE_AUDIO
    struct BackgroundAudio_t {
        QMediaPlayer *m_media_player = nullptr;
        QAudioOutput *m_audio_output = nullptr;

        // void setSource(const QString &path)
        // {
        // 	m_media_player->setSource("qrc" + path);
        // }

        BackgroundAudio_t()
        {
            m_media_player = new QMediaPlayer;
            m_audio_output = new QAudioOutput;

            m_media_player->setAudioOutput(m_audio_output);
            m_media_player->setSource(
                "qrc" + Config::get().AudioFiles().BACKGROUND_MUSIC);
            m_audio_output->setVolume(Config::get().Settings().music_volume);
            m_media_player->setLoops(QMediaPlayer::Infinite);
        };

        ~BackgroundAudio_t()
        {
            delete m_audio_output;
            delete m_media_player;
        }
    } m_background_music;

#endif

private slots:
    void on_SettingsShowEvent();

    void exitAction();

    void backToMainMenuAction();

    void on_StartBtn_clicked();

    void on_SettingsBtn_clicked();

private:
    Ui::MainWindow *ui;
};

#endif    // MAINWINDOW_H
