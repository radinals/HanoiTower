#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "hanoistack.h"
#include "linkedlist.h"
#include "soundplayer.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QWidget>
#include <QtMultimedia>
#include <map>
#include <qaccessible_base.h>
#include <qaudiooutput.h>
#include <qmainwindow.h>
#include <qmediaplayer.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <string>

class GameView : public QWidget
{
	Q_OBJECT

      private:
	bool m_timer_started = false;
	bool m_game_paused = false;
	bool m_game_started = false;

	const float m_slice_scale_factor = 0.9f;

	unsigned long long int m_timer_elapsed = 0;
	size_t m_goal_stack_index = 0;
	unsigned int m_move_count = 0;

	QSizeF m_stack_area_size, m_stack_base_size, m_slice_base_size;

	std::unordered_map<size_t, HanoiStack *> m_stacks;
	LinkedList<HanoiSlice *> m_slice_list;

	std::pair<HanoiSlice *, HanoiStack *> m_selected_slice;

	QLabel *m_time_output = nullptr, *m_move_count_output = nullptr;
	QTimer *m_timer = nullptr;
	QMessageBox *m_gameover_dialog = nullptr;

	SoundPlayer m_placement_fx;

      public:
	explicit GameView(QWidget *parent = nullptr);

	// button events are set in MainWindow
	QPushButton *m_gameover_dialog_yes_btn = nullptr,
	            *m_gameover_dialog_no_btn = nullptr;

	~GameView() override;

	void reset(); // reset the game states, and re-draw
	void clear(); // reset the game states
	void pause(); // halt the timer, inhibit input

	bool isPaused() { return m_game_paused; }

	// clang-format off

	void setGameInfoOutputs(QLabel *time, QLabel *moves) {
		m_time_output = time; m_move_count_output = moves;
	}

	// clang-format on

      private slots:
	void checkWinState();

      private:
	// clang-format off

        inline static QString numToChar(size_t n) {
                std::string str = ""; str += char('A' + n);
		return QString::fromStdString(str);
        };

	inline void moveSelectedSlice(int x, int y) {
		m_selected_slice.first->setX(
		    x - (m_selected_slice.first->getWidth() * 0.5));
		m_selected_slice.first->setY(
		    y - (m_selected_slice.first->getHeight() * 0.5));
		update();
	}

	inline bool goalStackIsComplete() { ;
		return ( m_stacks.at(m_goal_stack_index)->getSliceCount() ==
				Config::get().getSliceAmount() );
	}

	// clang-format on

	static size_t getRandomGoalStackIndex();
	void showGameGoalDialog();

	void updateInfo();
	void triggerLoseDialog();
	void triggerWinDialog();

	static void colorizeSprite(QPixmap *, const QColor &);
	HanoiStack *calculateStackByPos(QPointF);
	void calculatesSizes();
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

#endif // GAMEVIEW_H
