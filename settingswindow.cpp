#include "settingswindow.h"

#include "config.h"
#include "ui_settingswindow.h"
#include "utils.h"

#include <QSize>

SettingsWindow::SettingsWindow(QWidget* parent)
    : QWidget(parent), ui(new Ui::SettingsWindow)
{
	ui->setupUi(this);

	this->setStyleSheet(Config::get().getDefaultStylesheet());

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
SettingsWindow::on_GameTimerInput_userTimeChanged(const QTime& time)
{
	if (time.isValid()) {
		long long int ms;
		ms = Utils::extractMsFromTime(time.hour(), time.minute(),
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
		// clang-format off
		long long int ms;
		ms = Utils::extractMsFromTime(time.hour(),
					      time.minute(),
					      time.second());
		// clang-format on
		//
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
	// QPainter painter(ui->PreviewOut);
	QSize Winsize(ui->PreviewOut->size());

	if (m_preview_scene != nullptr) {
		delete m_preview_scene;
		m_preview_scene = new QGraphicsScene;
	}

	m_preview_scene = new QGraphicsScene;
	ui->PreviewOut->setScene(m_preview_scene);

	static const unsigned int vpadding = 0;
	static const unsigned int hpadding = 5;

	// clang-format off
	QSizeF base_slice_size(
		(float(Winsize.width()) / Settings.stack_amount) * 0.5f,
		(float(Winsize.height()) / Settings.slice_amount) * 0.3f
	);

	QSizeF stack_pole_size(
		base_slice_size.width() * 0.1f,
		base_slice_size.height() * Settings.slice_amount
	);
	// clang-format on

	QPen pen;
	m_preview_scene->clear();

	float x_offset = Winsize.width() * 0.5f;
	for (int i = 0; i < Settings.stack_amount; i++) {

		QSizeF slice_size = base_slice_size;
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

void
SettingsWindow::on_AudioMusicVolSlider_sliderMoved(int position)
{
	Settings.music_volume_level = (position * 0.01f);
	update_options();
}
