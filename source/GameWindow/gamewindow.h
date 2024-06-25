//-- Description -------------------------------------------------------------/
// the game's view widget, holds the GameView and also it's sidebar           /
//----------------------------------------------------------------------------/

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "../GameView/gameview.h"
#include "ui_gamewindow.h"
#include <QWidget>

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

private:
    Ui::GameWindow *ui;

signals:
    void s_open_settings();
    void s_back_to_main_menu();
    void s_exit_game();
};

#endif    // GAMEWINDOW_H
