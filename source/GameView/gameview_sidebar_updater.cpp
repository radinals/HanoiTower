//-- Description -------------------------------------------------------------/
// methods that handles the updating of data on the sidebar                   /
//----------------------------------------------------------------------------/

#include "../Utils/utils.h"

#include "../Config/config.h"

#include "gameview.h"

// update the Game's sidebar information
// the current time is calculated from subtracting the start time with a
// time elapsed counter.
void
GameView::updateInfo()
{
    if (m_sidebar_widgets.timer_out != nullptr) {
        if (has_solver_task()) {
            m_sidebar_widgets.timer_out->setText(".....");
        } else {
            auto hh_mm_ss = Utils::extractTimeFromMs(
                Config::get().Settings().time_length_ms - m_time.elapsed);

            QString h, m, s;
            h = QString::number(std::get<0>(hh_mm_ss));
            m = QString::number(std::get<1>(hh_mm_ss));
            s = QString::number(std::get<2>(hh_mm_ss));

            if (std::get<0>(hh_mm_ss) < 10) { h = '0' + h; }
            if (std::get<1>(hh_mm_ss) < 10) { m = '0' + m; }
            if (std::get<2>(hh_mm_ss) < 10) { s = '0' + s; }

            m_sidebar_widgets.timer_out->setText(h + ":" + m + ":" + s);
        }

        if (m_sidebar_widgets.move_count_out != nullptr) {
            m_sidebar_widgets.move_count_out->setText(
                QString::number(m_move_count));
        }
    }
}
