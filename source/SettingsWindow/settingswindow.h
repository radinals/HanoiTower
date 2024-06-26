//-- Description -------------------------------------------------------------/
// widget that gives the interface to modify game configurations              /
//----------------------------------------------------------------------------/

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QEvent>
#include <QGraphicsScene>
#include <QWidget>

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIO

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    struct Settings_t {
        size_t        stack_amount = 0, slice_amount = 0;
        long long int timer_ms         = 0;
        float         sfx_volume_level = 0, music_volume_level = 0;
        QColor        slice_color, stack_color;
    } Settings;

    QGraphicsScene *m_preview_scene = nullptr;

    void drawPreview();
    void updateDisplays();

    // used for handling exiting from setting window
    void hideEvent(QHideEvent *event) override
    {
        emit(s_hidden());
        QWidget::hideEvent(event);
    }

    // make sure to update the preview window everytime is 'un-hide/shown'
    void showEvent(QShowEvent *event) override
    {
        loadDefaults();
        updateDisplays();
        drawPreview();
        QWidget::showEvent(event);
    }

    void loadDefaults();

    void resizeEvent(QResizeEvent *event) override { drawPreview(); }

#ifndef DISABLE_AUDIO
    QSoundEffect *m_sfx_preview = nullptr;
#endif    // !DISABLE_AUDIO

private slots:
    void on_ThemeSliceColorSettingsInput_editingFinished();

    void on_ThemeStackColorSettingsInput_editingFinished();

    void on_GameTimerInput_userTimeChanged(const QTime &time);

    void on_GameTimerInput_editingFinished();

    void on_CancelButton_clicked();

    void on_SaveButton_clicked();

    void on_GameSliceAmountSlider_valueChanged(int value);

    void on_GameStackAmountSlider_valueChanged(int value);

    void on_AudioMusicVolSlider_sliderMoved(int position);

    void on_AudioSFXVolSlider_sliderMoved(int position);

private:
    Ui::SettingsWindow *ui;
signals:
    void s_hidden();
    void s_bg_audio_settings_changed();
};

#endif    // SETTINGSWINDOW_H
