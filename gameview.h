#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "hanoistack.h"

#include <QPushButton>
#include <QWidget>
#include <map>

class GameView : public QWidget
{
        Q_OBJECT

      private:
        std::map<char, HanoiStack *> m_stacks;

	int m_stack_count;
	int m_max_slice_count;

	bool init = true;

	QSize m_stack_base_size;
	QSize m_slice_base_size;
	QVector2D m_view_center;

	std::pair<HanoiSlice *, HanoiStack *> m_selected_slice;

      public:
        explicit GameView(QWidget *parent = nullptr);
        void resetGame();

      private:
        HanoiStack *calculateStackByPos(QPointF);
        void calculateElementSizes();
        void drawStack(HanoiStack *stack, QPainter *painter);
        void placeStack(int x_offset, HanoiStack *stack);

	void mousePressEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;
	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *event) override;

      signals:
};

#endif // GAMEVIEW_H
