#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QGraphicsScene>
#include <QWidget>

namespace Ui
{
	class SettingsWindow;
}

class SettingsWindow : public QWidget
{
	Q_OBJECT

      private:
	struct Settings_t {
		size_t stack_amount = 0, slice_amount = 0;
		QColor slice_color, stack_color;
		long long int timer_ms = 0;

		float sfx_volume_level = 0;
		float music_volume_level = 0;
	} Settings;

	QGraphicsScene *m_preview_scene = nullptr;

	void drawPreview();
	void update_options();

      public:
	explicit SettingsWindow(QWidget *parent = nullptr);
	~SettingsWindow();

      private slots:
	void on_ThemeSliceColorSettingsInput_editingFinished();

	void on_ThemeStackColorSettingsInput_editingFinished();

	void on_GameTimerInput_userTimeChanged(const QTime &time);

	void on_GameTimerInput_editingFinished();

	void on_CancelButton_clicked();

	void on_SaveButton_clicked();

	void on_GameSliceAmountSlider_valueChanged(int value);

	void on_GameStackAmountSlider_valueChanged(int value);

	void on_AudioSFXVolSlider_valueChanged(int value);

	void on_AudioMusicVolSlider_valueChanged(int value);

	void on_AudioMusicVolSlider_sliderMoved(int position);

      private:
	Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
