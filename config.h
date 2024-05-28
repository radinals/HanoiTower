#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QString>

class Config
{

      private:
	QString m_SliceSpritePath = ":/sprites/base_slice_sprite.png";
	QString m_StackBaseSpritePath = ":/sprites/stack_base_sprite.png";
	QString m_StackPoleSpritePath = ":/sprites/stack_pole_sprite.png";
	QString m_PlacementFXAudioPath = ":/audio/placement_fx.mp3";
	QString m_BgMusicAudioPath = ":/audio/placement_fx.mp3";

	QColor m_StackTint = "#483333";
	QColor m_SliceTint = "#a62e2e";

	size_t m_slice_amount = 5;
	size_t m_stack_amount = 3;

	static const size_t m_slice_max = 10;
	static const size_t m_stack_max = 10;

	float m_AudioFXVolumeLevel = 100.0f;
	float m_AudioMusicVolumeLevel = 100.0f;

	unsigned long long int m_timer_interval_ms = 60000 * 60;

      public:
	static Config& get()
	{
		static Config instance;
		return instance;
	}

	// clang-format off
	unsigned long long int getTimerInterval() {return m_timer_interval_ms; }
	QString getSliceSpritePath() { return m_SliceSpritePath; }
	QString getStackBaseSpritePath() { return m_StackBaseSpritePath; }
	QString getStackPoleSpritePath() { return m_StackPoleSpritePath; }
	QColor getStackTint() { return m_StackTint; }
	QColor getSliceTint() { return m_SliceTint; }
	QString getPlacementFXAudioPath() { return m_PlacementFXAudioPath; }
	QString getBgMusicAudioPath() { return m_BgMusicAudioPath; }

	float getAudioFXVolumeLevel() { return m_AudioFXVolumeLevel; }
	float getAudioMusicVolumeLevel() { return m_AudioMusicVolumeLevel; }

	void getAudioFXVolumeLevel(float l) { m_AudioFXVolumeLevel = l; }
	void getAudioMusicVolumeLevel(float l) { m_AudioMusicVolumeLevel = l; }

	void setBgMusicAudioPath(const QString& p) { m_BgMusicAudioPath = p; }
	void setPlacementFXAudioPath(const QString& p) { m_PlacementFXAudioPath = p; }
	void setTimerInterval(unsigned long long int ms) {m_timer_interval_ms = ms; }
	void setSliceSpritePath(const QString& p) { m_SliceSpritePath = p; }
	void setStackBaseSpritePath(const QString& p) { m_StackBaseSpritePath = p; }
	void setStackPoleSpritePath(const QString& p) { m_StackPoleSpritePath = p; }
	void setStackTint(const QColor& c) { m_StackTint = c; }
	void setSliceTint(const QColor& c) { m_SliceTint = c; }

        size_t getSliceMax() {return m_slice_max;}
        size_t getStackMax() {return m_stack_max;}

        size_t getSliceAmount() {return m_slice_amount;}
        size_t getStackAmount() {return m_stack_amount;}
        void setSliceAmount(size_t n) { m_slice_amount = n;}
        void setStackAmount(size_t n) { m_stack_amount = n;}
	// clang-format on
};

#endif // CONFIG_H
