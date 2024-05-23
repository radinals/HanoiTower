#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "hanoistack.h"

#include <QPushButton>
#include <QWidget>

class GameView : public QWidget
{
        Q_OBJECT

      private:
        HanoiStack *m_stack_A = nullptr;
        HanoiStack *m_stack_B = nullptr;
        HanoiStack *m_stack_C = nullptr;

	const size_t m_stack_count = 3;
	const size_t m_max_slice_count = 5;
	const float m_slice_base_margin = 20;
	const size_t m_stack_area_top_margin = 10;
	const size_t m_stack_area_bottom_margin = 100;

	QPushButton *m_exit_btn = nullptr;
	QPushButton *m_reset_btn = nullptr;

	QSize m_stack_base_size;
	QSize m_slice_base_size;
	QVector2D m_view_center;

	const float m_stack_base_space = 10;

	bool init = true;
	HanoiSlice *m_slice_sel = nullptr;
	HanoiStack *m_slice_sel_source = nullptr;

      public:
        explicit GameView(QWidget *parent = nullptr);

      private:
        HanoiStack *calculateStackByPos(QPointF);
        void calculateElementSizes();
        void mousePressEvent(QMouseEvent *) override;
        void mouseReleaseEvent(QMouseEvent *) override;
        void mouseMoveEvent(QMouseEvent *) override;
        void paintEvent(QPaintEvent *) override;
        void drawStack(HanoiStack *stack, QPainter *painter);
        void initStack(int x_offset, HanoiStack *stack);
        void resizeEvent(QResizeEvent *event) override;

	// void exitButtonPressed();
	// void resetButtonPressed();
	// void startGameview();

      signals:
};

#endif // GAMEVIEW_H
