#include "gameview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

GameView::GameView(QWidget* parent) : QWidget{parent}
{
	m_stack_A = new HanoiStack;
	m_stack_B = new HanoiStack;
	m_stack_C = new HanoiStack;

	m_exit_btn = new QPushButton("Exit", this);
	m_reset_btn = new QPushButton("Reset", this);

	HanoiStack::setStackFull(m_stack_A);
	setAutoFillBackground(true);

	// initStack((m_stack_base_space * 2) + m_stack_base_size.width(),
	// m_stack_B);
	// initStack((m_stack_base_space * 2) + m_stack_base_size.width(),
	// m_stack_C);
}

void
GameView::calculateElementSizes()
{
	m_stack_base_size =
	    QSize(width() / m_stack_count, (height() / m_stack_count) + 200);

	m_slice_base_size =
	    QSize(m_stack_base_size.width() - (m_slice_base_margin * 2),
		  m_stack_base_size.height() / m_max_slice_count);

	m_view_center = QVector2D(width() / 2, height() / 2);
}

void
GameView::paintEvent(QPaintEvent* event)
{
	if (init) {
		m_stack_A->scaleSlices(m_slice_base_size);
		init = false;
	}
	initStack(0, m_stack_A);
	initStack(m_stack_base_size.width(), m_stack_B);
	initStack(m_stack_base_size.width() * 2, m_stack_C);

	QPainter p(this);

	{
		float offset = 0;
		for (size_t i = 0; i < m_stack_count; i++) {
			p.drawRect(offset, m_view_center.y() / 2,
				   m_stack_base_size.width(),
				   m_stack_base_size.height());
			offset += m_stack_base_size.width();
		}
	}

	drawStack(m_stack_A, &p);
	drawStack(m_stack_B, &p);
	drawStack(m_stack_C, &p);

	if (m_slice_sel != nullptr) {
		p.drawPixmap(
		    m_slice_sel->getCoordinate().x(),
		    m_slice_sel->getCoordinate().y(),
		    m_slice_sel->getPixmap()->scaled(m_slice_sel->getSize()));
	}
}

void
GameView::initStack(int x_offset, HanoiStack* stack)
{
	if (stack->isEmpty())
		return;
	float y_axis = ((m_view_center.y() / 2) + m_stack_base_size.height()) -
		       m_stack_area_bottom_margin;

	unsigned int r_x = x_offset + (m_stack_base_size.width() / 2);

	for (HanoiSlice* slices : stack->getSlices()) {
		QSize size = slices->getSize();
		int x = r_x - (size.width() / 2);
		slices->setCoordinate(QVector2D(x, y_axis));
		y_axis -= (size.height() / 2) + 10;
	}
}

void
GameView::drawStack(HanoiStack* stack, QPainter* painter)
{
	if (stack->isEmpty()) {
		return;
	}
	for (HanoiSlice* slice : stack->getSlices()) {
		painter->drawPixmap(
		    slice->getCoordinate().x(), slice->getCoordinate().y(),
		    slice->getPixmap()->scaled(slice->getSize()));
	}
}

void
GameView::resizeEvent(QResizeEvent* event)
{
	calculateElementSizes();
	m_stack_A->scaleSlices(m_stack_base_size);
	m_stack_B->scaleSlices(m_stack_base_size);
	m_stack_C->scaleSlices(m_stack_base_size);
	repaint();
}

HanoiStack*
GameView::calculateStackByPos(QPointF point)
{
	float x_area = m_stack_base_size.width();
	float y_area = (m_view_center.y() / 2) + m_stack_base_size.height();
	for (size_t i = 0; i < m_stack_count; i++) {
		float x = x_area / point.x();
		float y = y_area / point.y();
		if (x >= 1 && y >= 1 && point.y() > m_view_center.y() / 2) {
			switch (i) {
			case 0:
				return m_stack_A;
			case 1:
				return m_stack_B;
			case 2:
				return m_stack_C;
			default:
				return nullptr;
			}
		}
		x_area += x_area;
	}
	return nullptr;
}

void
GameView::mousePressEvent(QMouseEvent* event)
{
	if (m_slice_sel != nullptr) {
		return;
	}

	QPoint point;
	HanoiStack* clicked_stack = nullptr;
	switch (event->button()) {
	case Qt::LeftButton:
		clicked_stack =
		    calculateStackByPos(event->position().toPoint());
		break;
	default:
		return;
	}

	if (clicked_stack == nullptr || clicked_stack->isEmpty()) {
		return;
	}
	m_slice_sel = clicked_stack->pop();
	m_slice_sel_source = clicked_stack;
}

void
GameView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_slice_sel == nullptr) {
		return;
	}
	m_slice_sel->setCoordinate(
	    QVector2D(event->pos().x(), event->pos().y()));
	repaint();
}

void
GameView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_slice_sel == nullptr) {
		return;
	}

	HanoiStack* stack_area =
	    calculateStackByPos(event->position().toPoint());

	if (stack_area == nullptr || stack_area->isFull() ||
	    stack_area == m_slice_sel_source ||
	    (!stack_area->isEmpty() &&
	     m_slice_sel->getValue() < stack_area->peek()->getValue())) {

		m_slice_sel_source->push(m_slice_sel);
	} else {
		stack_area->push(m_slice_sel);
	}

	m_slice_sel = nullptr;
	m_slice_sel_source = nullptr;
	repaint();
}
