//-- Description -------------------------------------------------------------/
// methods that handles the auto-solving process                              /
//----------------------------------------------------------------------------/

#include "gameview.h"

#include "../Config/config.h"

#include <chrono>
#include <thread>

void
GameView::hanoiIterativeSolver()
{
    const size_t n = Config::get().Settings().slice_amount;

    size_t possible_moves = (1 << n) - 1;

    size_t source = 0,
           aux    = ((m_hanoi.goal_stack->label() == 1)
                         ? m_hanoi.goal_stack->label() + 1
                         : 1),
           dest   = m_hanoi.goal_stack->label();

    if (n % 2 == 0) {
        size_t tmp;
        tmp  = dest;
        dest = aux;
        aux  = tmp;
    }

    for (int i = 1; i <= possible_moves && !m_solver_task.stop_solving; i++) {
        {
            while (m_solver_task.pause_solving) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (m_solver_task.stop_solving) { return; }
            }
        }

        if (i % 3 == 0) {
            moveTopSlice(getStack(aux), getStack(dest));
        } else if (i % 3 == 1) {
            moveTopSlice(getStack(source), getStack(dest));
        } else {
            moveTopSlice(getStack(source), getStack(aux));
        }

        ++m_move_count;

        QMetaObject::invokeMethod(this, "repaint", Qt::QueuedConnection);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool
GameView::has_solver_task()
{
    return m_solver_task.work_thread != nullptr;
}

bool
GameView::has_paused_solver_task()
{
    return has_solver_task() && m_solver_task.pause_solving;
}

void
GameView::stop_solver_task()
{
    if (!has_solver_task()) {
        throw std::runtime_error(
            "GameView::start_solver_task(): failed to stop solving "
            "task, no task is currently running");
    }

    m_solver_task.stop_solving = true;

    // wait for the thread to exit
    if (m_solver_task.work_thread->joinable()) {
        m_solver_task.work_thread->join();
    }

    // de-allocate the thread
    delete m_solver_task.work_thread;

    // reset the states
    m_solver_task.pause_solving = false;
    m_solver_task.stop_solving  = false;

    m_solver_task.work_thread = nullptr;
}

void
GameView::start_solver_task()
{
    if (has_solver_task()) {
        throw std::runtime_error(
            "GameView::start_solver_task(): failed to init a new solving "
            "task, a task is already present");
    }

    // start the thread;
    m_solver_task.work_thread
        = new std::thread(&GameView::hanoiIterativeSolver, this);
}

void
GameView::unpause_solver_task()
{
    if (!has_paused_solver_task()) {
        throw std::runtime_error("GameView::unpause_solver_task(): failed to "
                                 "un-pause task");
    }
    m_solver_task.pause_solving = false;
}

void
GameView::pause_solver_task()
{
    if (has_paused_solver_task()) {
        throw std::runtime_error("GameView::pause_solver_task(): failed to "
                                 "pause task");
    }
    m_solver_task.pause_solving = true;
}
