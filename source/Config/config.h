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
    static constexpr int           SOLVER_DELAY         = 10;    // ms
    static constexpr size_t        SLICE_MAX            = 10;
    static constexpr size_t        STACK_MAX            = 5;
    static constexpr long long int TIMER_MIN            = 60000 * 3;
    static constexpr float         H_SCALE_FACTOR       = 0.95F;
    static constexpr float         W_SCALE_FACTOR       = 0.9F;
    static constexpr char          DEFAULT_STACK_TINT[] = "#71391c";
    static constexpr char          DEFAULT_SLICE_TINT[] = "#7e1313";

    // clang-format off

    struct AssetsFiles {
        static constexpr char SLICE[]               = ":/sprites/base_slice_sprite.png";
        static constexpr char STACK_BASE[]          = ":/sprites/stack_base_sprite.png";
        static constexpr char STACK_POLE[]          = ":/sprites/stack_pole_sprite.png";
        static constexpr char ARROW[]               = ":/sprites/arrow.png";
        static constexpr char LOGO[]                = ":/ui/logo.png";
        static constexpr char DIALOG[]              = ":/sprites/dialog_base.png";
        static constexpr char QUIT_BTN_ICON[]       = ":/ui/quit_game_btn_icon.png";
        static constexpr char RESET_BTN_ICON[]      = ":/ui/reset_btn_icon.png";
        static constexpr char SETTINGS_BTN_ICON[]   = ":/ui/open_settings_btn_icon.png";
        static constexpr char MENU_BTN_ICON[]       = ":/ui/open_menu_btn_icon.png";
        static constexpr char PAUSE_BTN_ICON[]      = ":/ui/pause_btn_icon.png";
        static constexpr char PLAY_BTN_ICON[]       = ":/ui/play_btn_icon.png";
        static constexpr char AUTO_SOLVE_BTN_ICON[] = ":/ui/auto_solve_btn_icon.png";
        static constexpr char UNDO_BTN_ICON[]       = ":/ui/undo_btn_icon.png";
        static constexpr char REDO_BTN_ICON[]       = ":/ui/redo_btn_icon.png";
    } ;

    // clang-format on

    struct Theme {
        static inline QString font_name               = "monospace";
        static inline QColor  font_color              = "#fffeee";
        static inline QColor  highlight_tint          = "#e8d81c";
        static inline QColor  stack_tint              = DEFAULT_STACK_TINT;
        static inline QColor  slice_tint              = DEFAULT_SLICE_TINT;
        static inline QColor  win_dialog_tint         = "#28ab2b";
        static inline QColor  lose_dialog_tint        = "#ab282f";
        static inline QColor  solver_done_dialog_tint = "#DEDE00";
        static constexpr char DEFAULT_STYLESHEET[]    = ":/style/default.qss";
    };

    struct Settings {
        static inline size_t        slice_amount   = 5;
        static inline size_t        stack_amount   = 3;
        static inline float         fx_volume      = 1.0F;
        static inline float         music_volume   = 1.0F;
        static inline long long int time_length_ms = 60000 * 5;
    };

#ifndef DISABLE_AUDIO
    static inline QAudioOutput* m_bg_music_output = nullptr;

    struct AudioFiles {
        static constexpr char PLACEMENT_FX[] = "qrc:/audio/placement_fx.wav";
        static constexpr char BACKGROUND_MUSIC[] = "qrc:/audio/bg_music.wav";
    };
#endif    // !DISABLE_AUDIO
};

#endif    // CONFIG_H
