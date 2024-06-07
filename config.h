#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QFile>
#include <QString>

#ifndef DISABLE_AUDIO
    #include <QAudioOutput>
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIO

class Config {
private:
    // clang-format off
    struct AssetsFiles_t {
        static const inline QString SLICE       = ":/sprites/base_slice_sprite.png";
        static const inline QString STACK_BASE  = ":/sprites/stack_base_sprite.png";
        static const inline QString STACK_POLE  = ":/sprites/stack_pole_sprite.png";
        static const inline QString ARROW       = ":/sprites/arrow.png";
        static const inline QString LOGO        = ":/style/logo.png";
        static const inline QString DIALOG      = ":/sprites/dialog_base.png";
    } m_Assetsfiles;

#ifndef DISABLE_AUDIO
    struct AudioFiles_t {
        static const inline QString PLACEMENT_FX     = ":/audio/placement_fx.wav";
        static const inline QString BACKGROUND_MUSIC = ":/audio/bg_music.wav";
    } m_Audiofiles;
#endif // !DISABLE_AUDIO
    // clang-format on

    static const inline QString m_defaultStylesheet = ":/style/default.qss";

    struct Theme_t {
        static inline QString font_name        = "monospace";
        static inline QColor  font_color       = "#fffeee";
        static inline QColor  highlight_tint   = "#e8d81c";
        static inline QColor  stack_tint       = "#71391c";
        static inline QColor  slice_tint       = "#7e1313";
        static inline QColor  win_dialog_tint  = "#28ab2b";
        static inline QColor  lose_dialog_tint = "#ab282f";
    } m_theme;

    struct GameSettings_t {
        static inline const size_t        SLICE_MAX      = 10;
        static inline const size_t        STACK_MAX      = 10;
        static inline const long long int TIMER_MIN      = 60000 * 3;
        static inline size_t              slice_amount   = 5;
        static inline size_t              stack_amount   = 3;
        static inline float               fx_volume      = 1.0f;
        static inline float               music_volume   = 1.0f;
        static inline long long int       time_length_ms = 60000 * 5;
    } m_settings;

#ifndef DISABLE_AUDIO
    QAudioOutput* m_backgroundMusicInstance = nullptr;
#endif    // !DISABLE_AUDIO

public:
#ifndef DISABLE_AUDIO
    void setBackgroundMusicInstance(QAudioOutput* i)
    {
        m_backgroundMusicInstance = i;
    }
    QAudioOutput* const getBackgroundMusicInstance()
    {
        return m_backgroundMusicInstance;
    }
#endif    // !DISABLE_AUDIO

    const QString& getDefaultStylesheet()
    {
        static QString style_sheet;
        if (style_sheet.isEmpty()) {
            QFile file(m_defaultStylesheet);
            file.open(QFile::ReadOnly);
            style_sheet = QLatin1String(file.readAll());
        }
        return style_sheet;
    }

    static Config& get()
    {
        static Config instance;
        return instance;
    }

    const AssetsFiles_t& AssetFiles() { return m_Assetsfiles; }
#ifndef DISABLE_AUDIO
    const AudioFiles_t& AudioFiles() { return m_Audiofiles; }
#endif    // !DISABLE_AUDIO
    Theme_t&        Theme() { return m_theme; }
    GameSettings_t& Settings() { return m_settings; }
};

#endif    // CONFIG_H
