#include "settingswindow.h"

#include "config.h"
#include "ui_settingswindow.h"

#include <QFile>
#include <QSize>

SettingsWindow::SettingsWindow(QWidget* parent)
    : QWidget(parent), ui(new Ui::SettingsWindow)
{
        ui->setupUi(this);

	QFile file(Config::get().getDefaultStyleSheet());
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);

	Settings.stack_amount = Config::get().getStackAmount();
	Settings.slice_amount = Config::get().getSliceAmount();
	Settings.slice_color = Config::get().getSliceTint();
	Settings.stack_color = Config::get().getStackTint();
	Settings.timer_ms = Config::get().getTimerInterval();
	Settings.sfx_volume_level = Config::get().getAudioFXVolumeLevel();
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

	ui->AudioSFXVolOut->setText(
	    QString::number(int(Settings.sfx_volume_level * 100)) + "%");

	ui->AudioMusicVolSlider->setValue(Settings.music_volume_level * 100);
	ui->AudioSFXVolSlider->setValue(Settings.sfx_volume_level * 100);

	ui->ThemeStackColorSettingsInput->setText(
	    Settings.stack_color.toRgb().name());
	ui->ThemeSliceColorSettingsInput->setText(
	    Settings.slice_color.toRgb().name());

	long long int ms = Settings.timer_ms;
	long long hours = 0, minutes = 0, seconds = 0;

	// Calculate total seconds from milliseconds
	long long int total_seconds = ms / 1000;

	// Calculate hours
	hours = total_seconds / 3600;

	// Calculate remaining minutes
	minutes = (total_seconds % 3600) / 60;

	// Calculate remaining seconds
	seconds = (ms % 60000) / 1000.0;

	ui->GameTimerInput->setTime(QTime(hours, minutes, seconds));
}

void
SettingsWindow::on_ThemeSliceColorSettingsInput_editingFinished()
{
	if (!ui->ThemeSliceColorSettingsInput->text().isEmpty()) {
		QString c = ui->ThemeSliceColorSettingsInput->text();
		if (QColor::isValidColorName(c)) {
			Settings.slice_color = c;
		}
	}
	update_options();
	drawPreview();
}

void
SettingsWindow::on_ThemeStackColorSettingsInput_editingFinished()
{
	if (!ui->ThemeStackColorSettingsInput->text().isEmpty()) {
		QString c = ui->ThemeStackColorSettingsInput->text();
		if (QColor::isValidColorName(c)) {
			Settings.stack_color = c;
		}
	}
	update_options();
	drawPreview();
}

void
SettingsWindow::on_GameTimerInput_editingFinished()
{
	if (ui->GameTimerInput->time().isValid()) {
		unsigned long long int h, m, s;

		h = ui->GameTimerInput->time().hour();
		m = ui->GameTimerInput->time().minute();
		s = ui->GameTimerInput->time().second();

		Settings.timer_ms = ((h * 3600000) + (m * 60000) + (s * 1000));
	}
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
	Config::get().getBgMusicPlayerInstance()->getSound()->setVolume(
	    Settings.music_volume_level);
	close();
}

void
SettingsWindow::drawPreview()
{
	// QPainter painter(ui->PreviewOut);
	QSize Winsize(ui->PreviewOut->size());

	if (m_preview_scene != nullptr) {
		delete m_preview_scene;
		m_preview_scene = new QGraphicsScene;
	}

	m_preview_scene = new QGraphicsScene;
	ui->PreviewOut->setScene(m_preview_scene);

	const unsigned int vpadding = 0;
	const unsigned int hpadding = 5;

	// clang-format off
	QSize base_slice_size(
		(Winsize.width() / Settings.stack_amount) * 0.5f,
		(Winsize.height() / Settings.slice_amount) * 0.3f
	);

	QSize stack_pole_size(
		base_slice_size.width() * 0.1f,
		base_slice_size.height() * Settings.slice_amount
	);
	// clang-format on

	QPen pen;
	m_preview_scene->clear();

	float x_offset = Winsize.width() * 0.5f;
	for (int i = 0; i < Settings.stack_amount; i++) {

		QSize slice_size = base_slice_size;
		bool drawn_base = false;

		float y_offset = Winsize.height();
		for (int j = 0; j < Settings.slice_amount; j++) {

			// clang-format off

			// draw the base first
			if (!drawn_base) {
				m_preview_scene->addRect(
				    x_offset - (slice_size.width() * 0.5f),
				    y_offset, slice_size.width(),
				    slice_size.height() * 0.5f, pen,
				    Settings.stack_color
				);

				y_offset -= slice_size.height();

				m_preview_scene->addRect(
				    x_offset - (stack_pole_size.width() * 0.5f),
				    y_offset - (stack_pole_size.height() - slice_size.height()),
				    stack_pole_size.width(),
				    stack_pole_size.height(), pen,
				    Settings.stack_color
				);
				drawn_base = true;
			}

                        if (i == 0) {
                            m_preview_scene->addRect(
                               x_offset - (slice_size.width() * 0.5f),
                               y_offset, slice_size.width(),
                               slice_size.height(), pen,
                               Settings.slice_color
                            );
                        }

                        // clang-format on

			slice_size *= 0.9f;
			y_offset -= slice_size.height() + vpadding;
		}
		x_offset += base_slice_size.width() + hpadding;
	}

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
SettingsWindow::on_AudioMusicVolSlider_valueChanged(int value)
{
	Settings.music_volume_level = (value * 0.01f);
	update_options();
}
