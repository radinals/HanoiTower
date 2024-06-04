#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "config.h"
#include "hanoistack.h"
#include "linkedlist.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <QtMultimedia>
#include <qaccessible_base.h>
#include <qaudiooutput.h>
#include <qmainwindow.h>
#include <qmediaplayer.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qsoundeffect.h>
#include <string>
#include <utility>

class GameView : public QWidget {
    Q_OBJECT

private:
    const float m_slice_scale_factor = 0.9f;

    enum class GameState {
        GameNotRunning,
        GameRunning,
        GamePaused,
        GameOverLost,
        GameOverWon,
    };

    GameState m_game_state = GameState::GameNotRunning;

    QPixmap m_pole_sprite, m_stack_base_sprite, m_arrow_sprite;

    QSizeF m_stack_area_size, m_stack_base_size, m_slice_base_size,
        m_dialog_size;

    QTimer        m_timer;
    QSoundEffect *m_placement_fx = nullptr;

    unsigned long long int m_timer_elapsed = 0;
    unsigned int           m_move_count    = 0;

    LinkedList<std::pair<size_t, HanoiStack>> m_stacks;
    LinkedList<HanoiSlice *>                  m_slice_list;
    std::pair<size_t, HanoiStack *>           m_goal_stack { 0, nullptr };

    struct SelectedData_t {
        HanoiStack *stack = nullptr;
        HanoiSlice *slice = nullptr;
        inline bool valid() { return stack != nullptr && slice != nullptr; }
    } m_selected;

    QLabel    *m_time_output = nullptr, *m_move_count_output = nullptr;
    QTextEdit *m_info_box = nullptr;

public:
    explicit GameView(QWidget *parent = nullptr);
    ~GameView() override { delete m_placement_fx; }
    // ~GameView() override {};

    void reset();    // reset the game states, and re-draw
    void clear();    // reset the game states
    void pause();    // halt the timer, inhibit input

    bool isPaused() { return m_game_state == GameState::GamePaused; }

    void setGameInfoOutputs(QLabel *time, QLabel *moves, QTextEdit *info_box)
    {
        m_time_output       = time;
        m_move_count_output = moves;
        m_info_box          = info_box;
    }

private slots:
    void checkWinState();

private:
    inline static QString numToChar(size_t n)
    {
        std::string str;
        str += char('A' + n);
        return QString::fromStdString(str);
    };

    inline void moveSelectedSlice(const QPoint &p)
    {
        m_selected.slice->setX(p.x() - (m_selected.slice->getWidth() * 0.5f));
        m_selected.slice->setY(p.y() - (m_selected.slice->getHeight() * 0.5f));
        update();
    }

    inline bool goalStackIsComplete()
    {
        return (m_goal_stack.second->getSize()
                == Config::get().getSliceAmount());
    }

    inline bool moveIsValid(const std::pair<size_t, HanoiStack *> &dest)
    {
        return (dest.second != nullptr)
               && dest.first < Config::get().getStackAmount()
               && (dest.second->isEmpty()
                   || m_selected.slice->getValue()
                          > dest.second->peek()->getValue());
    }

    void drawDialog(const QString  &text,
                    const QColor   &color,
                    QPainter *const painter);

    static size_t getRandomGoalStackIndex();

    void updateInfo();

    static void                     colorizeSprite(QPixmap *, const QColor &);
    std::pair<size_t, HanoiStack *> calculateStackByPos(const QPointF &);

    void calculateBaseSizes();
    void drawStack(float offset, HanoiStack *, QPainter *p);
    void drawStackBase(size_t label, float offset, QPainter *painter);
    void scaleSlices();

    // QWidget Event Handlers
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

signals:
};

#endif    // GAMEVIEW_H
