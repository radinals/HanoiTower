//-- Description -------------------------------------------------------------/
// singleton class that holds all the global variables of the game            /
//----------------------------------------------------------------------------/

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
    struct AssetsFiles_t;
    struct AudioFiles_t;
    struct Theme_t;
    struct GameSettings_t;

public:
    static constexpr int           SOLVER_DELAY       = 10;    // ms
    static constexpr size_t        SLICE_MAX          = 10;
    static constexpr size_t        STACK_MAX          = 5;
    static constexpr long long int TIMER_MIN          = 60000 * 3;
    static constexpr float         H_SCALE_FACTOR     = 0.95F;
    static constexpr float         W_SCALE_FACTOR     = 0.9F;
    static const inline QColor     DEFAULT_STACK_TINT = "#71391c";
    static const inline QColor     DEFAULT_SLICE_TINT = "#7e1313";

public:
    //------------------------------------------------------------------------

    static const AssetsFiles_t& AssetFiles() { return get().m_Assetsfiles; }
    static Theme_t&             Theme() { return get().m_theme; }
    static GameSettings_t&      Settings() { return get().m_settings; }

    //------------------------------------------------------------------------

    // load and get stylesheet
    static const QString& getDefaultStylesheet()
    {
        static QString style_sheet;
        if (style_sheet.isEmpty()) {
            QFile file(m_defaultStylesheet);
            file.open(QFile::ReadOnly);
            style_sheet = QLatin1String(file.readAll());
        }
        return style_sheet;
    }

    //------------------------------------------------------------------------

#ifndef DISABLE_AUDIO
    static const AudioFiles_t& AudioFiles() { return get().m_Audiofiles; }

    static void setBackgroundMusicInstance(QAudioOutput* i)
    {
        get().m_backgroundMusicInstance = i;
    }

    static QAudioOutput* const getBackgroundMusicInstance()
    {
        return get().m_backgroundMusicInstance;
    }
#endif    // !DISABLE_AUDIO

private:
    // clang-format off

    struct AssetsFiles_t {
        static const inline QString SLICE       = ":/sprites/base_slice_sprite.png";
        static const inline QString STACK_BASE  = ":/sprites/stack_base_sprite.png";
        static const inline QString STACK_POLE  = ":/sprites/stack_pole_sprite.png";
        static const inline QString ARROW       = ":/sprites/arrow.png";
        static const inline QString LOGO        = ":/ui/logo.png";
        static const inline QString DIALOG      = ":/sprites/dialog_base.png";

        static const inline QString QUIT_BTN_ICON = ":/ui/quit_game_btn_icon.png";
        static const inline QString RESET_BTN_ICON = ":/ui/reset_btn_icon.png";
        static const inline QString SETTINGS_BTN_ICON = ":/ui/open_settings_btn_icon.png";
        static const inline QString MENU_BTN_ICON = ":/ui/open_menu_btn_icon.png";
        static const inline QString PAUSE_BTN_ICON = ":/ui/pause_btn_icon.png";
        static const inline QString PLAY_BTN_ICON = ":/ui/play_btn_icon.png";
        static const inline QString AUTO_SOLVE_BTN_ICON = ":/ui/auto_solve_btn_icon.png";
    } m_Assetsfiles;

    // clang-format on

    static const inline QString m_defaultStylesheet = ":/style/default.qss";

    struct Theme_t {
        static inline QString font_name        = "monospace";
        static inline QColor  font_color       = "#fffeee";
        static inline QColor  highlight_tint   = "#e8d81c";
        static inline QColor  stack_tint       = DEFAULT_STACK_TINT;
        static inline QColor  slice_tint       = DEFAULT_SLICE_TINT;
        static inline QColor  win_dialog_tint  = "#28ab2b";
        static inline QColor  lose_dialog_tint = "#ab282f";
    } m_theme;

    struct GameSettings_t {
        static inline size_t        slice_amount   = 5;
        static inline size_t        stack_amount   = 3;
        static inline float         fx_volume      = 1.0f;
        static inline float         music_volume   = 1.0f;
        static inline long long int time_length_ms = 60000 * 5;
    } m_settings;

#ifndef DISABLE_AUDIO
    static inline QAudioOutput* m_backgroundMusicInstance = nullptr;

    struct AudioFiles_t {
        static const inline QString PLACEMENT_FX = ":/audio/placement_fx.wav";
        static const inline QString BACKGROUND_MUSIC = ":/audio/bg_music.wav";
    } m_Audiofiles;
#endif    // !DISABLE_AUDIO

    // get static instance
    static Config& get()
    {
        static Config instance;
        return instance;
    }
};

#endif    // CONFIG_H
