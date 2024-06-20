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
    const size_t slice_amount = Config::get().Settings().slice_amount;
    const size_t possible_moves
        = (1 << slice_amount) - 1;    // (2^slice_amount) -1

    // starting stack
    size_t source = 0;

    // the goal of the slices
    size_t dest = m_hanoi.goal_stack->getLabel();

    // this should be either the slice after the first one,
    // or the slice after the goal stack
    size_t aux = (dest == 1) ? dest + 1 : 1;

    assert(dest != aux);
    assert(source != dest);
    assert(source != aux);

    // swap dest with aux if
    // slice_amount is an even number
    if (slice_amount % 2 == 0) {
        size_t tmp;
        tmp  = dest;
        dest = aux;
        aux  = tmp;
    }

    for (int i = 1; i <= possible_moves && !m_solver_task.stop_solving; i++) {
        // pauses the loop in place
        while (m_solver_task.pause_solving) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (m_solver_task.stop_solving) { return; }
        }

        // main algorithm
        if (i % 3 == 0) {
            makeLegalMove(getStack(aux), getStack(dest));
        } else if (i % 3 == 1) {
            makeLegalMove(getStack(source), getStack(dest));
        } else {
            makeLegalMove(getStack(source), getStack(aux));
        }

        ++m_move_count;

        // redraw screeen
        QMetaObject::invokeMethod(this, "repaint", Qt::QueuedConnection);

        // wait for some time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    emit(s_solver_exited());
}

// NOTE: the solver is ran on a different thread so it's does not interrupt with
// the widget process.

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

// starts or stops the solver task
void
GameView::stop_solver_task()
{
    assert(has_solver_task());

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
    assert(!has_solver_task());

    // start the thread;
    m_solver_task.work_thread
        = new std::thread(&GameView::hanoiIterativeSolver, this);
}

// change the state to be pause/un-pause

void
GameView::unpause_solver_task()
{
    assert(has_paused_solver_task());
    m_solver_task.pause_solving = false;
}

void
GameView::pause_solver_task()
{
    assert(!has_paused_solver_task());
    m_solver_task.pause_solving = true;
}
