#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "config.h"
#include "hanoistack.h"
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
#include <qpushbutton.h>
#include <string>

class GameView : public QWidget
{
	Q_OBJECT

      private:
	QMessageBox *m_gameover_dialog = nullptr;

	SoundPlayer m_placement_fx;

	static QColor m_slice_tint;
	static QColor m_stack_tint;

	bool m_init = false;
	bool m_timer_started = false;
	unsigned long long int m_timer_elapsed = 0;
	size_t m_goal_stack_index = Config::get().getStackAmount() - 1;

	std::map<size_t, HanoiStack *> m_stacks;
	QSize m_stack_area_size, m_stack_base_size, m_slice_base_size;
	QVector2D m_view_center;

	std::pair<HanoiSlice *, HanoiStack *> m_selected_slice;

	QTimer *m_timer;
	unsigned int m_score = 0, m_move_count = 0;

	QLabel *m_time_output = nullptr, *m_score_output = nullptr,
	       *m_move_count_output = nullptr;

      public:
	explicit GameView(QWidget *parent = nullptr);

	QPushButton *m_gameover_dialog_yes_btn = nullptr;
	QPushButton *m_gameover_dialog_no_btn = nullptr;

	~GameView() override;
	void init();

	void setSliceTint(const QColor &color) { m_slice_tint = color; }
	void setStackTint(const QColor &color) { m_stack_tint = color; }

	void setGameInfoOutputs(QLabel *time, QLabel *score, QLabel *moves)
	{
		m_time_output = time;
		m_score_output = score;
		m_move_count_output = moves;
	}

      private slots:
	void checkWinState();

      private:
	// clang-format off
        inline static QString numToChar(size_t n) {
                std::string str = ""; str += char('A' + n); return QString::fromStdString(str);
        };
	// clang-format on

	static size_t getRandomGoalStackIndex();
	void showGameGoalDialog();
	bool goalStackIsComplete();

	void updateInfo();
	void triggerLoseDialog();
	void triggerWinDialog();

	void scaleStack(HanoiStack *);
	static void colorizeSprite(QPixmap *, const QColor &);
	HanoiStack *calculateStackByPos(QPointF);
	void calculatesSizes();
	static void drawStack(HanoiStack *stack, QPainter *painter);
	void drawStackBase(const QString &stack_label, float offset,
	                   QPainter *painter);
	void setStackCoordinates(float offset, HanoiStack *stack);

	void mousePressEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;
	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *) override;

      signals:
};

#endif // GAMEVIEW_H
