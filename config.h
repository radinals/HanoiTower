#ifndef CONFIG_H
#define CONFIG_H

#include "soundplayer.h"

#include <QColor>
#include <QString>

class Config
{
	// clang-format off
      private:
	const QString m_ProgramLogo         = ":/style/logo.png";
	const QString m_SliceSpritePath     = ":/sprites/base_slice_sprite.png";
	const QString m_StackBaseSpritePath = ":/sprites/stack_base_sprite.png";
	const QString m_StackPoleSpritePath = ":/sprites/stack_pole_sprite.png";
	const QString m_ArrowSpritePath     = ":/sprites/arrow.png";
	const QString m_WinScreenSpritePath = ":/sprites/win_screen.png";
	const QString m_LoseScreenSpritePath = ":/sprites/lose_screen.png";

	const QString m_PlacementFXAudioPath = ":/audio/placement_fx.wav";
	QString m_BgMusicAudioPath           = ":/audio/bg_music.wav";

	QString m_defaultStylesheet = ":/style/default.qss";

	QString m_stack_label_font = "monospace";

	QColor m_stack_label_font_color = "#fffeee";
	QColor m_highlight_color        = "#e8d81c";
	QColor m_StackTint              = "#71391c";
	QColor m_SliceTint              = "#892828";

	size_t m_slice_amount = 5;
	size_t m_stack_amount = 3;

	static const size_t m_slice_max = 10;
	static const size_t m_stack_max = 10;

	float m_AudioFXVolumeLevel    = 1.0f;
	float m_AudioMusicVolumeLevel = 1.0f;

	unsigned long long int m_timer_interval_ms = 60000 * 5;

	// FIXME: shouldn't be here
	SoundPlayer* m_bg_music_player_instance = nullptr;

      public:
	static Config& get() { static Config instance; return instance; }

	const QString& getProgramLogo() { return m_ProgramLogo; }
	const QString& getDefaultStyleSheet() { return m_defaultStylesheet; }
	const QString& getStackLabelFont() { return m_stack_label_font; }
	const QString& getSliceSpritePath() { return m_SliceSpritePath; }
	const QString& getStackBaseSpritePath() { return m_StackBaseSpritePath; }
	const QString& getStackPoleSpritePath() { return m_StackPoleSpritePath; }
	const QString& getArrowSpritePath() { return m_ArrowSpritePath; }
	const QString& getPlacementFXAudioPath() { return m_PlacementFXAudioPath; }
	const QString& getBgMusicAudioPath() { return m_BgMusicAudioPath; }
	const QString& getWinScreen() { return m_WinScreenSpritePath; };
	const QString& getLoseScreen() { return m_LoseScreenSpritePath; };
        const QColor& getStackLabelFontColor() { return m_stack_label_font_color; }
        const QColor& getHighlightColor() { return m_highlight_color; }
	const QColor& getStackTint() { return m_StackTint; }
	const QColor& getSliceTint() { return m_SliceTint; }
        size_t getSliceMax() { return m_slice_max; }
        size_t getStackMax() { return m_stack_max; }
        size_t getSliceAmount() { return m_slice_amount; }
        size_t getStackAmount() { return m_stack_amount; }
	float getAudioFXVolumeLevel() { return m_AudioFXVolumeLevel; }
	float getAudioMusicVolumeLevel() { return m_AudioMusicVolumeLevel; }
	SoundPlayer* const getBgMusicPlayerInstance() { return m_bg_music_player_instance; }
	unsigned long long int getTimerInterval() { return m_timer_interval_ms; }

	void setStackLabelFont(const QString& f) { m_stack_label_font = f; }
	void setBgMusicAudioPath(const QString& p) { m_BgMusicAudioPath = p; }
	void setDefaultStyleSheet(const QString& f ) { m_defaultStylesheet = f; }
        void setStackLabelFontColor(const QColor& c) { m_stack_label_font_color = c; }
        void setHighlightColor(const QColor& c) { m_highlight_color = c; }
	void setStackTint(const QColor& c) { m_StackTint = c; }
	void setSliceTint(const QColor& c) { m_SliceTint = c; }
	void setTimerInterval(unsigned long long int ms) {m_timer_interval_ms = ms; }
	void setAudioMusicVolumeLevel(float l) { m_AudioMusicVolumeLevel = l; }
	void setAudioFXVolumeLevel(float l) { m_AudioFXVolumeLevel = l; }
        void setSliceAmount(size_t n) { m_slice_amount = n; }
        void setStackAmount(size_t n) { m_stack_amount = n; }
	void setBgMusicPlayerInstance(SoundPlayer* i) { m_bg_music_player_instance = i; }
	// clang-format on
};

#endif // CONFIG_H
