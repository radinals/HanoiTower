#include "settingswindow.h"

#include "config.h"
#include "ui_settingswindow.h"
#include "utils.h"

#include <QSize>

SettingsWindow::SettingsWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    this->setStyleSheet(Config::get().getDefaultStylesheet());

    Settings.stack_amount       = Config::get().getStackAmount();
    Settings.slice_amount       = Config::get().getSliceAmount();
    Settings.slice_color        = Config::get().getSliceTint();
    Settings.stack_color        = Config::get().getStackTint();
    Settings.timer_ms           = Config::get().getTimerInterval();
    Settings.sfx_volume_level   = Config::get().getAudioFXVolumeLevel();
    Settings.music_volume_level = Config::get().getAudioMusicVolumeLevel();

    update_options();
    drawPreview();
}

SettingsWindow::~SettingsWindow()
{
    delete m_preview_scene;
    delete ui;
}

void
SettingsWindow::update_options()
{
    ui->GameSliceAmountSlider->setValue(Settings.slice_amount);
    ui->GameStackAmountSlider->setValue(Settings.stack_amount);

    ui->GameSliceAmountOut->setText(QString::number(Settings.slice_amount));
    ui->GameStackAmountOut->setText(QString::number(Settings.stack_amount));

    ui->AudioMusicVolOut->setText(
        QString::number(int(Settings.music_volume_level * 100)) + "%");

    if (Config::get().getBackgroundMusicInstance() != nullptr) {
        Config::get().getBackgroundMusicInstance()->setVolume(
            Settings.music_volume_level);
    }

    ui->AudioSFXVolOut->setText(
        QString::number(int(Settings.sfx_volume_level * 100)) + "%");

    ui->AudioMusicVolSlider->setValue(Settings.music_volume_level * 100);
    ui->AudioSFXVolSlider->setValue(Settings.sfx_volume_level * 100);

    ui->ThemeStackColorSettingsInput->setText(
        Settings.stack_color.toRgb().name());
    ui->ThemeSliceColorSettingsInput->setText(
        Settings.slice_color.toRgb().name());

    auto hh_mm_ss = Utils::extractTimeFromMs(Settings.timer_ms);

    ui->GameTimerInput->setTime(QTime(std::get<0>(hh_mm_ss),
                                      std::get<1>(hh_mm_ss),
                                      std::get<2>(hh_mm_ss)));
}

void
SettingsWindow::on_ThemeSliceColorSettingsInput_editingFinished()
{
    if (!ui->ThemeSliceColorSettingsInput->text().isEmpty()) {
        QString c = ui->ThemeSliceColorSettingsInput->text();
        if (QColor::isValidColorName(c)) { Settings.slice_color = c; }
    }
    update_options();
    drawPreview();
}

void
SettingsWindow::on_ThemeStackColorSettingsInput_editingFinished()
{
    if (!ui->ThemeStackColorSettingsInput->text().isEmpty()) {
        QString c = ui->ThemeStackColorSettingsInput->text();
        if (QColor::isValidColorName(c)) { Settings.stack_color = c; }
    }
    update_options();
    drawPreview();
}

void
SettingsWindow::on_GameTimerInput_userTimeChanged(const QTime& time)
{
    if (time.isValid()) {
        long long int ms;
        ms = Utils::extractMsFromTime(time.hour(),
                                      time.minute(),
                                      time.second());

        if (ms <= 0 || ms < Config::get().getTimerMin()) {
            update_options();
            return;
        }
    }
}

void
SettingsWindow::on_GameTimerInput_editingFinished()
{
    QTime time = ui->GameTimerInput->time();

    if (time.isValid()) {
        long long int ms;
        ms = Utils::extractMsFromTime(time.hour(),
                                      time.minute(),
                                      time.second());
        if (ms > 0 && ms >= Config::get().getTimerMin()) {
            Settings.timer_ms = (ms);
        }
    }

    update_options();
}

void
SettingsWindow::on_CancelButton_clicked()
{
    close();
}

void
SettingsWindow::on_SaveButton_clicked()
{
    Config::get().setStackAmount(Settings.stack_amount);
    Config::get().setSliceAmount(Settings.slice_amount);
    Config::get().setSliceTint(Settings.slice_color);
    Config::get().setStackTint(Settings.stack_color);
    Config::get().setTimerInterval(Settings.timer_ms);
    Config::get().setAudioFXVolumeLevel(Settings.sfx_volume_level);
    Config::get().setAudioMusicVolumeLevel(Settings.music_volume_level);
    Config::get().getBackgroundMusicInstance()->setVolume(
        Settings.music_volume_level);
    close();
}

void
SettingsWindow::drawPreview()
{
    // scenes seems to not update correctly if not deleted first
    if (m_preview_scene != nullptr) {
        delete m_preview_scene;
        m_preview_scene = new QGraphicsScene;
    }
    m_preview_scene = new QGraphicsScene;
    ui->PreviewOut->setScene(m_preview_scene);
    m_preview_scene->clear();

    // ----------------------------------------------------------------------

    static const QPen  pen;
    static const float slice_scale_factor = 0.9f;    // down scale
    static const int   hpadding           = 5;       // spacing
    // static const int   vpadding           = 0;

    const QSizeF gs(ui->PreviewOut->size());    // QGraphicScene geometry

    const QSizeF base_slice((float(gs.width()) / Settings.stack_amount) * 0.5f,
                            (float(gs.height()) / Settings.slice_amount)
                                * 0.3f);

    const QSizeF stack_pole(base_slice.width() * 0.1f,
                            base_slice.height() * (Settings.slice_amount + 2));

    const QSizeF stack_base(base_slice.width(), base_slice.height() * 0.5f);

    // Draw the base and the pole -------------------------------------------

    float x = (gs.width() * 0.5f);    // x render start
    for (size_t i = 0; i < Settings.stack_amount; i++) {
        m_preview_scene->addRect(x - (stack_pole.width() * 0.5f),
                                 gs.height() - stack_pole.height(),
                                 stack_pole.width(),
                                 stack_pole.height(),
                                 pen,
                                 Settings.stack_color);

        m_preview_scene->addRect(x - (stack_base.width() * 0.5f),
                                 gs.height() - stack_base.height(),
                                 stack_base.width(),
                                 stack_base.height(),
                                 pen,
                                 Settings.stack_color);

        x += base_slice.width() + hpadding;    // shifting to right
    }

    // Draw Slices on the first stack ---------------------------------------

    QSizeF ssize = base_slice;                                 // scale size
    float  y     = gs.height() - (stack_base.height() * 3);    // y render start

    for (size_t i = 0; i < Settings.slice_amount; i++) {
        m_preview_scene->addRect((gs.width() * 0.5f) - (ssize.width() * 0.5f),
                                 y,
                                 ssize.width(),
                                 ssize.height(),
                                 pen,
                                 Settings.slice_color);

        ssize *= slice_scale_factor;    // scale down
        y -= ssize.height();            // shift up
    }

    // ----------------------------------------------------------------------

    ui->PreviewOut->resetTransform();
    ui->PreviewOut->viewport()->update();
}

void
SettingsWindow::on_GameSliceAmountSlider_valueChanged(int value)
{
    if (value > 0 && value <= Config::get().getSliceMax()) {
        Settings.slice_amount = value;
    }
    update_options();
    drawPreview();
}

void
SettingsWindow::on_GameStackAmountSlider_valueChanged(int value)
{
    if (value > 0 && value <= Config::get().getStackMax()) {
        Settings.stack_amount = value;
    }
    update_options();
    drawPreview();
}

void
SettingsWindow::on_AudioSFXVolSlider_valueChanged(int value)
{
    Settings.sfx_volume_level = (value * 0.01f);
    update_options();
}

void
SettingsWindow::on_AudioMusicVolSlider_sliderMoved(int position)
{
    Settings.music_volume_level = (position * 0.01f);
    update_options();
}
