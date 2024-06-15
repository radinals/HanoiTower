#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "../GameView/gameview.h"
#include "ui_gamewindow.h"
#include <QWidget>
#include <qevent.h>

namespace Ui {
    class GameWindow;
}

class GameWindow : public QWidget {
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow() override;
    bool &isSettingsBtnPressed() { return m_settings_btn_pressed; };

private:
    static inline GameView *m_game_view = nullptr;

    bool m_settings_btn_pressed = false;

private slots:
    void on_AutoSolveBtn_clicked();

    void on_ResetBtn_clicked();

    void on_PauseBtn_clicked();

    void on_BackToMenuBtn_clicked();

    void on_QuitGameBtn_clicked();

    void hideEvent(QHideEvent *event) override
    {
        emit(hidden());
        QWidget::hideEvent(event);
    }

    void showEvent(QShowEvent *event) override
    {
        QWidget::showEvent(event);
        m_game_view->reset();
    }

    void on_OpenSettingsBtn_clicked();

private:
    Ui::GameWindow *ui;

signals:
    void open_settings();
    void back_to_menu();
    void quit_game();
    void hidden();
};

#endif    // GAMEWINDOW_H
