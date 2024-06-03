#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "gameview.h"
#include "settingswindow.h"

#include <QAudioOutput>
#include <QMainWindow>
#include <QMediaPlayer>
#include <qmediaplayer.h>

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
      private:
	SettingsWindow *m_settings_window = nullptr;
	const QPixmap m_logo = QPixmap(Config::get().getProgramLogo());
	GameView *m_game_view = nullptr;

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
			    "qrc" +
			    Config::get().getBackgroundMusicAudioPath());
			m_audio_output->setVolume(
			    Config::get().getAudioMusicVolumeLevel());
		};

		~BackgroundAudio_t()
		{
			delete m_audio_output;
			delete m_media_player;
		}

	} m_background_music;

      public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

      private slots:
	void exitAction();

	void resetGameAction();

	void backToMainMenuAction();

	void on_StartBtn_clicked();

	void on_SettingsBtn_clicked();

	void on_PauseBtn_clicked();

      private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
