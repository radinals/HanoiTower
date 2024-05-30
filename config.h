#ifndef CONFIG_H
#define CONFIG_H

#include "soundplayer.h"

#include <QColor>
#include <QString>
#include <string>

class Config
{

      private:
	std::string m_program_data_pathname = "TowerOfHanoi";
	std::string m_leaderboard_data_filename = "leaderboards.csv";
	QString m_defaultStylesheet = ":/style/default.qss";

	QString m_SliceSpritePath = ":/sprites/base_slice_sprite.png";
	QString m_StackBaseSpritePath = ":/sprites/stack_base_sprite.png";
	QString m_StackPoleSpritePath = ":/sprites/stack_pole_sprite.png";
	QString m_PlacementFXAudioPath = ":/audio/placement_fx.wav";
	QString m_BgMusicAudioPath = ":/audio/bg_music.wav";
	QString m_stack_label_font = "monospace";
	QColor m_stack_label_font_color = "#fffeee";

	QColor m_StackTint = "#483333";
	QColor m_SliceTint = "#a62e2e";

	size_t m_slice_amount = 5;
	size_t m_stack_amount = 3;

	static const size_t m_slice_max = 10;
	static const size_t m_stack_max = 10;

	float m_AudioFXVolumeLevel = 1.0f;
	float m_AudioMusicVolumeLevel = 1.0f;

	unsigned long long int m_timer_interval_ms = 60000 * 5;

	// FIXME: shouldn't be here
	SoundPlayer* m_bg_music_player_instance = nullptr;

      public:
	static Config& get()
	{
		QString s;
		static Config instance;
		return instance;
	}

	// clang-format off
	SoundPlayer* const getBgMusicPlayerInstance() { return m_bg_music_player_instance; }
	void setBgMusicPlayerInstance(SoundPlayer* player_instance) { m_bg_music_player_instance = player_instance; }

	const QString& getDefaultStyleSheet() { return m_defaultStylesheet; }
	void setDefaultStyleSheet(const QString& f ) { m_defaultStylesheet = f; }

	const QString& getStackLabelFont() { return m_stack_label_font; }
	void setStackLabelFont(const QString& f) { m_stack_label_font = f; }

        const QColor& getStackLabelFontColor() { return m_stack_label_font_color; }
        void setStackLabelFontColor(const QColor& c) { m_stack_label_font_color = c; }

	unsigned long long int getTimerInterval() { return m_timer_interval_ms; }
	const QString& getSliceSpritePath() { return m_SliceSpritePath; }
	const QString& getStackBaseSpritePath() { return m_StackBaseSpritePath; }
	const QString& getStackPoleSpritePath() { return m_StackPoleSpritePath; }
	const QColor& getStackTint() { return m_StackTint; }
	const QColor& getSliceTint() { return m_SliceTint; }
	const QString& getPlacementFXAudioPath() { return m_PlacementFXAudioPath; }
	const QString& getBgMusicAudioPath() { return m_BgMusicAudioPath; }

	float getAudioFXVolumeLevel() { return m_AudioFXVolumeLevel; }
	float getAudioMusicVolumeLevel() { return m_AudioMusicVolumeLevel; }

	void setAudioFXVolumeLevel(float l) { m_AudioFXVolumeLevel = l; }
	void setAudioMusicVolumeLevel(float l) { m_AudioMusicVolumeLevel = l; }

	void setBgMusicAudioPath(const QString& p) { m_BgMusicAudioPath = p; }
	void setPlacementFXAudioPath(const QString& p) { m_PlacementFXAudioPath = p; }
	void setTimerInterval(unsigned long long int ms) {m_timer_interval_ms = ms; }
	void setSliceSpritePath(const QString& p) { m_SliceSpritePath = p; }
	void setStackBaseSpritePath(const QString& p) { m_StackBaseSpritePath = p; }
	void setStackPoleSpritePath(const QString& p) { m_StackPoleSpritePath = p; }
	void setStackTint(const QColor& c) { m_StackTint = c; }
	void setSliceTint(const QColor& c) { m_SliceTint = c; }

        size_t getSliceMax() { return m_slice_max; }
        size_t getStackMax() { return m_stack_max; }

        size_t getSliceAmount() { return m_slice_amount; }
        size_t getStackAmount() { return m_stack_amount; }
        void setSliceAmount(size_t n) { m_slice_amount = n; }
        void setStackAmount(size_t n) { m_stack_amount = n; }
	// clang-format on
};

#endif // CONFIG_H
