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

    Settings.stack_amount       = Config::get().Settings().stack_amount;
    Settings.slice_amount       = Config::get().Settings().slice_amount;
    Settings.slice_color        = Config::get().Theme().slice_tint;
    Settings.stack_color        = Config::get().Theme().stack_tint;
    Settings.timer_ms           = Config::get().Settings().time_length_ms;
    Settings.sfx_volume_level   = Config::get().Settings().fx_volume;
    Settings.music_volume_level = Config::get().Settings().music_volume;

    m_preview_scene = new QGraphicsScene;
    ui->PreviewOut->setScene(m_preview_scene);

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

        if (ms <= 0 || ms < Config::get().Settings().TIMER_MIN) {
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
        if (ms > 0 && ms >= Config::get().Settings().TIMER_MIN) {
            Settings.timer_ms = (ms);
        }
    }

    update_options();
}

void
SettingsWindow::on_CancelButton_clicked()
{
    hide();
}

void
SettingsWindow::on_SaveButton_clicked()
{
    Config::get().Settings().stack_amount   = Settings.stack_amount;
    Config::get().Settings().slice_amount   = Settings.slice_amount;
    Config::get().Theme().slice_tint        = Settings.slice_color;
    Config::get().Theme().stack_tint        = Settings.stack_color;
    Config::get().Settings().time_length_ms = Settings.timer_ms;
    Config::get().Settings().fx_volume      = Settings.sfx_volume_level;
    Config::get().Settings().music_volume   = Settings.music_volume_level;
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

    static const QPen  pen(QBrush("#000000"), 4);
    static const float slice_scale_factor = 0.9f;    // down scale
    static const int   hpadding           = 5;       // spacing

    const float sceneW = ui->PreviewOut->width();
    const float sceneH = ui->PreviewOut->height();

    // static const int   vpadding           = 0;

    const QSizeF gs(ui->PreviewOut->size());    // QGraphicScene geometry

    const QSizeF base_size(
        (float(sceneW) / (Settings.stack_amount + hpadding)) * 1.1f,
        (float(sceneH) / Config::get().Settings().SLICE_MAX));

    const QSizeF base_slice((base_size.width() * 0.8),
                            (base_size.height() * 0.8f));

    const QSizeF stack_pole(base_size.width() * 0.1f,
                            base_slice.height()
                                * (Config::get().Settings().SLICE_MAX));

    const QSizeF stack_base(base_size.width(), base_slice.height() * 0.2f);

    const float total_stacks_area
        = ((stack_base.width()) * Settings.stack_amount);

    // Draw the base and the pole -------------------------------------------

    const float start_x
        = ((sceneW * 0.5f) - (total_stacks_area * 0.5f)) + hpadding;

    float x = start_x;
    for (size_t i = 0; i < Settings.stack_amount; i++) {
        ui->PreviewOut->scene()->addRect(x - (stack_pole.width() * 0.5f),
                                         (sceneH) -stack_pole.height(),
                                         stack_pole.width(),
                                         stack_pole.height(),
                                         pen,
                                         Settings.stack_color);

        ui->PreviewOut->scene()->addRect(x - (stack_base.width() * 0.5f),
                                         (sceneH) -stack_base.height(),
                                         stack_base.width(),
                                         stack_base.height(),
                                         pen,
                                         Settings.stack_color);

        x += base_size.width() + hpadding;    // shifting to right
    }

    // Draw Slices on the first stack ---------------------------------------

    QSizeF ssize = base_slice;    // scale size

    float y
        = sceneH - (stack_base.height() + base_slice.height());    // bottom y

    for (size_t i = 0; i < Settings.slice_amount; i++) {
        ui->PreviewOut->scene()->addRect(start_x - (ssize.width() * 0.5f),
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
    if (value > 0 && value <= Config::get().Settings().SLICE_MAX) {
        Settings.slice_amount = value;
    }
    update_options();
    drawPreview();
}

void
SettingsWindow::on_GameStackAmountSlider_valueChanged(int value)
{
    if (value > 0 && value <= Config::get().Settings().STACK_MAX) {
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
