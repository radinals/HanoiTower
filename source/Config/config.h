//-- Description -------------------------------------------------------------/
// singleton class that holds all the global variables of the game            /
//----------------------------------------------------------------------------/

#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QString>

#ifndef DISABLE_AUDIO
    #include <QAudioOutput>
    #include <QSoundEffect>
#endif    // !DISABLE_AUDIO

struct Config {
    static constexpr int           SOLVER_DELAY       = 10;    // ms
    static constexpr size_t        SLICE_MAX          = 10;
    static constexpr size_t        STACK_MAX          = 5;
    static constexpr long long int TIMER_MIN          = 60000 * 3;
    static constexpr float         H_SCALE_FACTOR     = 0.95F;
    static constexpr float         W_SCALE_FACTOR     = 0.9F;
    static const inline QColor     DEFAULT_STACK_TINT = "#71391c";
    static const inline QColor     DEFAULT_SLICE_TINT = "#7e1313";

    // clang-format off

    struct AssetsFiles {
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
        static const inline QString UNDO_BTN_ICON = ":/ui/undo_btn_icon.png";
        static const inline QString REDO_BTN_ICON = ":/ui/redo_btn_icon.png";
    } ;

    // clang-format on

    struct Theme {
        static inline QString       font_name        = "monospace";
        static inline QColor        font_color       = "#fffeee";
        static inline QColor        highlight_tint   = "#e8d81c";
        static inline QColor        stack_tint       = DEFAULT_STACK_TINT;
        static inline QColor        slice_tint       = DEFAULT_SLICE_TINT;
        static inline QColor        win_dialog_tint  = "#28ab2b";
        static inline QColor        lose_dialog_tint = "#ab282f";
        static inline QColor        solver_done_dialog_tint = "#DEDE00";
        static const inline QString default_stylesheet = ":/style/default.qss";
    };

    struct Settings {
        static inline size_t        slice_amount   = 5;
        static inline size_t        stack_amount   = 3;
        static inline float         fx_volume      = 1.0f;
        static inline float         music_volume   = 1.0f;
        static inline long long int time_length_ms = 60000 * 5;
    };

#ifndef DISABLE_AUDIO
    static inline QAudioOutput* m_bg_music_output = nullptr;

    struct AudioFiles {
        static const inline QString PLACEMENT_FX = ":/audio/placement_fx.wav";
        static const inline QString BACKGROUND_MUSIC = ":/audio/bg_music.wav";
    };
#endif    // !DISABLE_AUDIO
};

#endif    // CONFIG_H
