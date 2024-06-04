#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QFile>
#include <QString>
#include <qaudiooutput.h>
#include <qsoundeffect.h>

class Config {
private:
    // clang-format off
    static const inline QString m_SliceSpritePath      = ":/sprites/base_slice_sprite.png";
    static const inline QString m_StackBaseSpritePath  = ":/sprites/stack_base_sprite.png";
    static const inline QString m_StackPoleSpritePath  = ":/sprites/stack_pole_sprite.png";
    static const inline QString m_ArrowSpritePath      = ":/sprites/arrow.png";
    static const inline QString m_WinScreenSpritePath  = ":/sprites/win_screen.png";
    static const inline QString m_LoseScreenSpritePath = ":/sprites/lose_screen.png";
    static const inline QString m_defaultStylesheet    = ":/style/default.qss";
    static const inline QString m_ProgramLogo          = ":/style/logo.png";
    static const inline QString m_PlacementFXAudioPath = ":/audio/placement_fx.wav";
    static const inline QString m_BgMusicAudioPath     = ":/audio/bg_music.wav";
    static const inline QString m_DialogBaseSpritePath = ":/sprites/dialog_base.png";
    // clang-format on

    QString m_stack_label_font       = "monospace";
    QColor  m_stack_label_font_color = "#fffeee";

    static const inline QColor m_win_dialog_tint  = "#28ab2b";
    static const inline QColor m_lose_dialog_tint = "#ab282f";

    QColor m_highlight_color = "#e8d81c";
    QColor m_StackTint       = "#71391c";
    QColor m_SliceTint       = "#7e1313";

    size_t m_slice_amount = 5;
    size_t m_stack_amount = 3;

    static const size_t m_slice_max = 10;
    static const size_t m_stack_max = 10;

    float m_AudioFXVolumeLevel    = 1.0f;
    float m_AudioMusicVolumeLevel = 1.0f;

    long long int m_timer_interval_ms = 60000 * 5;
    long long int m_timer_min         = 60000 * 3;

    QAudioOutput* m_backgroundMusicInstance = nullptr;

public:
    void setTimerInterval(long long int ms) { m_timer_interval_ms = ms; }
    void setAudioMusicVolumeLevel(float l) { m_AudioMusicVolumeLevel = l; }
    void setAudioFXVolumeLevel(float l) { m_AudioFXVolumeLevel = l; }
    void setSliceAmount(size_t n) { m_slice_amount = n; }
    void setStackAmount(size_t n) { m_stack_amount = n; }
    void setBackgroundMusicInstance(QAudioOutput* i)
    {
        m_backgroundMusicInstance = i;
    }

    const QString& getDialogBaseSprite() { return m_DialogBaseSpritePath; }
    const QString& getProgramLogo() { return m_ProgramLogo; }
    const QString& getStackLabelFont() { return m_stack_label_font; }
    const QString& getSliceSpritePath() { return m_SliceSpritePath; }
    const QString& getStackBaseSpritePath() { return m_StackBaseSpritePath; }
    const QString& getStackPoleSpritePath() { return m_StackPoleSpritePath; }
    const QString& getArrowSpritePath() { return m_ArrowSpritePath; }
    const QString& getPlacementFXAudioPath() { return m_PlacementFXAudioPath; }
    const QString& getBackgroundMusicAudioPath() { return m_BgMusicAudioPath; }
    const QString& getWinScreen() { return m_WinScreenSpritePath; };
    const QString& getLoseScreen() { return m_LoseScreenSpritePath; };
    const QColor&  getStackLabelFontColor() { return m_stack_label_font_color; }
    const QColor&  getHighlightColor() { return m_highlight_color; }
    const QColor&  getStackTint() { return m_StackTint; }
    const QColor&  getSliceTint() { return m_SliceTint; }
    const QColor&  getWinDialogTint() { return m_win_dialog_tint; }
    const QColor&  getLoseDialogTint() { return m_lose_dialog_tint; }

    long long int getTimerMin() { return m_timer_min; }
    long long int getTimerInterval() { return m_timer_interval_ms; }

    void   setStackTint(const QColor& c) { m_StackTint = c; }
    void   setSliceTint(const QColor& c) { m_SliceTint = c; }
    float  getAudioFXVolumeLevel() { return m_AudioFXVolumeLevel; }
    float  getAudioMusicVolumeLevel() { return m_AudioMusicVolumeLevel; }
    size_t getSliceMax() { return m_slice_max; }
    size_t getStackMax() { return m_stack_max; }
    size_t getSliceAmount() { return m_slice_amount; }
    size_t getStackAmount() { return m_stack_amount; }

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

    QAudioOutput* const getBackgroundMusicInstance()
    {
        return m_backgroundMusicInstance;
    }

    // void setStackLabelFont(const QString& f) { m_stack_label_font = f; }
    // void setStackLabelFontColor(const QColor& c) { m_stack_label_font_color =
    // c; } void setHighlightColor(const QColor& c) { m_highlight_color = c; }
};

#endif    // CONFIG_H
