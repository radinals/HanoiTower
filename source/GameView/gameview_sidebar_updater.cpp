//-- Description -------------------------------------------------------------/
// methods that handles the updating of data on the sidebar                   /
//----------------------------------------------------------------------------/

#include "../Utils/utils.h"

#include "../Config/config.h"

#include "gameview.h"
#include <QPushButton>

void
GameView::setSidebarWidget(QPushButton *time,
                           QLabel      *moves,
                           QLabel      *info_box_label,
                           QTextEdit   *info_box)
{
    SidebarWidgets::timer_out      = time;
    SidebarWidgets::move_count_out = moves;
    SidebarWidgets::info_msg_out   = info_box;
    SidebarWidgets::info_msg_label = info_box_label;
}

// update the Game's sidebar information
// the current time is calculated from subtracting the start time with a
// time elapsed counter.
void
GameView::updateInfo()
{
    if (SidebarWidgets::timer_out != nullptr) {
        if (m_game_state == GameState::GAME_PAUSED) {
            SidebarWidgets::timer_out->setText(" PAUSED ");
        } else if (has_solver_task()) {
            SidebarWidgets::timer_out->setText("--:--:--");
        } else {
            auto hh_mm_ss = Utils::extractTimeFromMs(
                Config::Settings().time_length_ms - TimeInfo::elapsed);

            QString h, m, s;
            h = QString::number(std::get<0>(hh_mm_ss));
            m = QString::number(std::get<1>(hh_mm_ss));
            s = QString::number(std::get<2>(hh_mm_ss));

            if (std::get<0>(hh_mm_ss) < 10) { h = '0' + h; }
            if (std::get<1>(hh_mm_ss) < 10) { m = '0' + m; }
            if (std::get<2>(hh_mm_ss) < 10) { s = '0' + s; }

            SidebarWidgets::timer_out->setText(h + ":" + m + ":" + s);
        }
    }

    if (SidebarWidgets::move_count_out != nullptr) {
        SidebarWidgets::move_count_out->setText(QString::number(m_move_count));
    }

    if (SidebarWidgets::info_msg_label != nullptr) {
        SidebarWidgets::info_msg_label->setText("OBJECTIVES");
        SidebarWidgets::info_msg_label->setAlignment(Qt::AlignCenter);
    }

    if (SidebarWidgets::info_msg_out != nullptr) {
        SidebarWidgets::info_msg_out->setText(
            "Move All Slice to Stack "
            + Utils::numToChar(HanoiStacks::goal_stack->getLabel()));
        SidebarWidgets::info_msg_out->setAlignment(Qt::AlignCenter);
    }
}
