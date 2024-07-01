//-- Description -------------------------------------------------------------/
// defines methods that give the user access to modify the game configurations/
//----------------------------------------------------------------------------/

#include "../SettingsWindow/settingswindow.h"

#include "../Config/config.h"
#include "../Utils/utils.h"
#include "ui_settingswindow.h"

#ifndef DISABLE_AUDIO
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIOLN

#include <QSizeF>

SettingsWindow::SettingsWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    // copy the global variables
    loadDefaults();

#ifndef DISABLE_AUDIO
    m_sfx_preview = new QSoundEffect(this);
    m_sfx_preview->setSource(QString(Config::AudioFiles::PLACEMENT_FX));
    assert(m_sfx_preview->status() != QSoundEffect::Error);
#else
    // hide all audio setting options
    ui->AudioSFXVolOut->hide();
    ui->AudioMusicVolOut->hide();
    ui->AudioMusicVolLabel->hide();
    ui->AudioSFXVolLabel->hide();
    ui->AudioMusicVolSlider->hide();
    ui->AudioSFXVolSlider->hide();
    ui->GameAudioSetupGroupLabel->hide();
    ui->AudioSettingGroupLine->hide();
#endif    // !DISABLE_AUDIO

    ui->GameSliceAmountSlider->setMaximum(Config::SLICE_MAX);
    ui->GameStackAmountSlider->setMaximum(Config::STACK_MAX);

    // init the preview scene
    m_preview_scene = new QGraphicsScene;
    ui->PreviewOut->setScene(m_preview_scene);

    // load the values
    updateDisplays();

    // update the preview
    drawPreview();
}

SettingsWindow::~SettingsWindow()
{
#ifndef DISABLE_AUDIO
    delete m_sfx_preview;
#endif    // !DISABLE_AUDIO
    delete m_preview_scene;
    delete ui;
}

void
SettingsWindow::loadDefaults()
{
    Settings.stack_amount = Config::Settings::stack_amount;
    Settings.slice_amount = Config::Settings::slice_amount;
    Settings.slice_color  = Config::Theme::slice_tint;
    Settings.stack_color  = Config::Theme::stack_tint;
    Settings.timer_ms     = Config::Settings::time_length_ms;
#ifndef DISABLE_AUDIO
    Settings.sfx_volume_level   = Config::Settings::fx_volume;
    Settings.music_volume_level = Config::Settings::music_volume;
#endif    // !DISABLE_AUDIO
}

// updates the setting menu option values
void
SettingsWindow::updateDisplays()
{
    //  Updates Game Setting Output
    ui->GameSliceAmountSlider->setValue(Settings.slice_amount);
    ui->GameStackAmountSlider->setValue(Settings.stack_amount);

    ui->GameSliceAmountOut->setText(QString::number(Settings.slice_amount));
    ui->GameStackAmountOut->setText(QString::number(Settings.stack_amount));

#ifndef DISABLE_AUDIO
    //  Updates Audio Setting Output
    ui->AudioMusicVolOut->setText(
        QString::number(int(Settings.music_volume_level * 100)) + "%");

    ui->AudioSFXVolOut->setText(
        QString::number(int(Settings.sfx_volume_level * 100)) + "%");

    ui->AudioMusicVolSlider->setValue(Settings.music_volume_level * 100);
    ui->AudioSFXVolSlider->setValue(Settings.sfx_volume_level * 100);

#endif    // !DISABLE_AUDIO

    //  Updates Game Timer Setting Output
    ui->ThemeStackColorSettingsInput->setText(
        Settings.stack_color.toRgb().name());
    ui->ThemeSliceColorSettingsInput->setText(
        Settings.slice_color.toRgb().name());

    const auto hh_mm_ss = Utils::extractTimeFromMs(Settings.timer_ms);

    ui->GameTimerInput->setTime(QTime(std::get<0>(hh_mm_ss),
                                      std::get<1>(hh_mm_ss),
                                      std::get<2>(hh_mm_ss)));
}

void
SettingsWindow::on_ThemeSliceColorSettingsInput_editingFinished()
{
    if (!ui->ThemeSliceColorSettingsInput->text().isEmpty()) {
        const QString input_str = ui->ThemeSliceColorSettingsInput->text();

        Settings.slice_color = (QColor::isValidColorName(input_str))
                                   ? input_str
                                   : Config::DEFAULT_SLICE_TINT;
    }
    updateDisplays();
    drawPreview();
}

void
SettingsWindow::on_ThemeStackColorSettingsInput_editingFinished()
{
    if (!ui->ThemeStackColorSettingsInput->text().isEmpty()) {
        const QString input_str = ui->ThemeStackColorSettingsInput->text();

        Settings.stack_color = (QColor::isValidColorName(input_str))
                                   ? input_str
                                   : Config::DEFAULT_STACK_TINT;
    }
    updateDisplays();
    drawPreview();
}

void
SettingsWindow::on_GameTimerInput_userTimeChanged(const QTime& time)
{
    if (time.isValid()) {
        const long long int ms = Utils::extractMsFromTime(time.hour(),
                                                          time.minute(),
                                                          time.second());

        if (ms <= 0 || ms < Config::TIMER_MIN) {
            updateDisplays();
            return;
        }
    }
}

void
SettingsWindow::on_GameTimerInput_editingFinished()
{
    const QTime time = ui->GameTimerInput->time();

    if (time.isValid()) {
        const long long int ms = Utils::extractMsFromTime(time.hour(),
                                                          time.minute(),
                                                          time.second());
        if (ms > 0 && ms >= Config::TIMER_MIN) { Settings.timer_ms = (ms); }
    }

    updateDisplays();
}

void
SettingsWindow::on_CancelButton_clicked()
{
#ifndef DISABLE_AUDIO
    Config::m_bg_music_output->setVolume(Config::Settings::music_volume);
#endif
    hide();
}

void
SettingsWindow::on_SaveButton_clicked()
{
    if (Settings.stack_amount != Config::Settings::stack_amount) {
        Config::Settings::stack_amount = Settings.stack_amount;
    }

    if (Settings.slice_amount != Config::Settings::slice_amount) {
        Config::Settings::slice_amount = Settings.slice_amount;
    }

    if (QColor(Settings.slice_color) != Config::Theme::slice_tint) {
        Config::Theme::slice_tint = Settings.slice_color;
    }

    if (QColor(Settings.stack_color) != Config::Theme::stack_tint) {
        Config::Theme::stack_tint = Settings.stack_color;
    }

    if (QColor(Settings.timer_ms) != Config::Settings::time_length_ms) {
        Config::Settings::time_length_ms = Settings.timer_ms;
    }

#ifndef DISABLE_AUDIO
    if (Settings.sfx_volume_level != Config::Settings::fx_volume) {
        Config::Settings::music_volume = Settings.sfx_volume_level;
        Config::Settings::fx_volume    = Settings.sfx_volume_level;
    }

    if (Settings.music_volume_level != Config::Settings::music_volume) {
        Config::Settings::music_volume = Settings.music_volume_level;
    }
#endif
    hide();
}

void
SettingsWindow::drawPreview()
{
    ui->PreviewOut->scene()->clear();
    ui->PreviewOut->scene()->setBackgroundBrush(QBrush(QColor("#59637c")));
    ui->PreviewOut->setSceneRect(0,
                                 0,
                                 ui->PreviewOut->viewport()->width(),
                                 ui->PreviewOut->viewport()->height());
    ui->PreviewOut->setAlignment(Qt::AlignCenter);
    ui->PreviewOut->centerOn(m_preview_scene->sceneRect().center());
    // ----------------------------------------------------------------------

    static const QPen    pen(QBrush("#000000"), 4);
    static constexpr int hpadding = 5;    // spacing

    const float sceneW = ui->PreviewOut->width();
    const float sceneH = ui->PreviewOut->height();

    const QSizeF stack_area(float(sceneW) / Settings.stack_amount,
                            float(sceneH) * 0.9F);

    const QSizeF base_slice(stack_area.width() * 0.9F,
                            stack_area.height() / Config::SLICE_MAX);

    const QSizeF stack_pole(base_slice.width() * 0.1F, stack_area.height());

    const QSizeF stack_base(stack_area.width() - hpadding, base_slice.height());

    // Draw the base and the pole -------------------------------------------

    {
        float x = 0;
        for (size_t i = 0; i < Settings.stack_amount; i++) {
            ui->PreviewOut->scene()->addRect(x + (stack_area.width() * 0.5F)
                                                 - (stack_pole.width() * 0.5f),
                                             sceneH - stack_pole.height(),
                                             stack_pole.width(),
                                             stack_pole.height(),
                                             pen,
                                             Settings.stack_color);

            ui->PreviewOut->scene()->addRect(x,
                                             sceneH - stack_base.height(),
                                             stack_base.width(),
                                             stack_base.height(),
                                             pen,
                                             Settings.stack_color);

            x += stack_area.width() + hpadding;    // shifting to right
        }
    }

    // Draw Slices on the first stack ---------------------------------------
    {
        QSizeF slice = base_slice;    // scale size

        const float x = stack_area.width() * 0.5F;

        float y = (sceneH - stack_base.height());    // bottom y

        for (size_t i = 0; i < Settings.slice_amount; i++) {
            y -= slice.height();    // shift up

            ui->PreviewOut->scene()->addRect(x - slice.width() * 0.5F,
                                             y,
                                             slice.width(),
                                             slice.height(),
                                             pen,
                                             Settings.slice_color);
            // scale down
            slice.setHeight(slice.height()
                            * Config::H_SCALE_FACTOR);    // scale down
            slice.setWidth(slice.width()
                           * Config::W_SCALE_FACTOR);    // scale down
        }
    }

    // ----------------------------------------------------------------------

    ui->PreviewOut->resetTransform();
    ui->PreviewOut->viewport()->update();
}

void
SettingsWindow::on_GameSliceAmountSlider_valueChanged(int value)
{
    if (value > 0 && value <= Config::SLICE_MAX) {
        Settings.slice_amount = value;
    }
    updateDisplays();
    drawPreview();
}

void
SettingsWindow::on_GameStackAmountSlider_valueChanged(int value)
{
    if (value > 0 && value <= Config::STACK_MAX) {
        Settings.stack_amount = value;
    }
    updateDisplays();
    drawPreview();
}

void
SettingsWindow::on_AudioMusicVolSlider_sliderMoved(int position)
{
#ifndef DISABLE_AUDIO
    Settings.music_volume_level = (position * 0.01f);
    updateDisplays();
    if (Config::m_bg_music_output != nullptr) {
        Config::m_bg_music_output->setVolume(Settings.music_volume_level);
    }
#endif
}

void
SettingsWindow::on_AudioSFXVolSlider_sliderMoved(int position)
{
#ifndef DISABLE_AUDIO
    Settings.sfx_volume_level = (position * 0.01f);
    updateDisplays();
    if (this->isVisible()) {
        m_sfx_preview->setVolume(Settings.sfx_volume_level);
        if (!m_sfx_preview->isPlaying()) { m_sfx_preview->play(); }
    }
#endif
}
