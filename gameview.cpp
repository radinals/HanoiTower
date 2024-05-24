#include "gameview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

// TODO: Better size/placement calculations, for better dynamic ui

GameView::GameView(QWidget* parent) : QWidget{parent}
{
	setMinimumSize(QSize(1366, 768));
	m_stacks['A'] = new HanoiStack;
	m_stacks['B'] = new HanoiStack;
	m_stacks['C'] = new HanoiStack;
	HanoiStack::setStackFull(m_stacks['A']);

	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;
	m_stack_count = m_stacks.size();
	m_max_slice_count = HanoiStack::m_max_slice_amount;

	setAutoFillBackground(true);
}

void
GameView::calculateElementSizes()
{
	m_stack_base_size = QSize(geometry().width() / m_stack_count,
				  (geometry().height() / 2));

	m_slice_base_size =
	    QSize((m_stack_base_size.width()),
		  m_stack_base_size.height() / m_max_slice_count);

	m_view_center =
	    QVector2D(geometry().width() / 2, geometry().height() / 2);
}

void
GameView::paintEvent(QPaintEvent* event)
{
	if (init) {
		init = false;
	}

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

	int x_offset = m_slice_base_size.width() / 2;
	for (auto stack : m_stacks) {
		placeStack(x_offset, stack.second);
		drawStack(stack.second, &p);
		x_offset += m_stack_base_size.width();
	}

	if (m_selected_slice.first != nullptr &&
	    m_selected_slice.second != nullptr) {
		p.drawPixmap(m_selected_slice.first->getCoordinate().x(),
			     m_selected_slice.first->getCoordinate().y(),
			     m_selected_slice.first->getScaledPixmap());
	}
}

void
GameView::placeStack(int x_offset, HanoiStack* stack)
{
	// clang-format off
	if (stack == nullptr || stack->isEmpty()) { return; }

        float y_axis =
            ((m_view_center.y() / 2) + m_stack_base_size.height())
                - (m_view_center.y() / 4);

        unsigned int r_x = x_offset;

	for (HanoiSlice* slices : stack->getSlices()) {
		QSize size = slices->getSize();
		y_axis -= (size.height() / 2) + 10;
		int x = r_x - (size.width() / 2);
		slices->setCoordinate(QVector2D(x, y_axis));
	}
	// clang-format on
}

void
GameView::drawStack(HanoiStack* stack, QPainter* painter)
{
	// clang-format off
	if (stack == nullptr || stack->isEmpty()) { return; }

        for (HanoiSlice* slice : stack->getSlices()) {
            painter->drawPixmap(
                slice->getCoordinate().x(),
                slice->getCoordinate().y(),
                slice->getScaledPixmap());
        }
        // clang-format on
}

void
GameView::resizeEvent(QResizeEvent* event)
{
	m_stack_base_size = QSize(geometry().width() / m_stack_count,
				  (geometry().height() / 2));

	m_slice_base_size =
	    QSize((m_stack_base_size.width()),
		  m_stack_base_size.height() / m_max_slice_count);

	m_view_center =
	    QVector2D(geometry().width() / 2, geometry().height() / 2);

	for (auto stack : m_stacks) {
		stack.second->scaleSlices(m_stack_base_size,
					  m_stack_base_size.width() /
					      m_stack_base_size.height());
	}
	repaint();
}

HanoiStack*
GameView::calculateStackByPos(QPointF point)
{
	float x_area = m_stack_base_size.width();
	float y_area = (m_view_center.y() / 2) + m_stack_base_size.height();

	// clang-format off

	for (auto stack : m_stacks) {
		float x = x_area / point.x(), y = y_area / point.y();

		// x and y should be 1 if the point is in the stack area

		if ((x >= 1) && (y >= 1) &&
		    (point.y() > (m_view_center.y() / 2)))
		{
			return m_stacks[stack.first];
		}

		// shift to the right, by stack area width
		x_area += x_area;
	}

	// clang-format on

	return nullptr;
}

void
GameView::mousePressEvent(QMouseEvent* event)
{
	if (m_selected_slice.first != nullptr ||
	    m_selected_slice.second != nullptr) {
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

	m_selected_slice.first = clicked_stack->pop();
	m_selected_slice.second = clicked_stack;
}

void
GameView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_selected_slice.first == nullptr ||
	    m_selected_slice.second == nullptr) {
		return;
	}

	// clang-format off

	QVector2D drag_coordinate(
	    event->pos().x() - (m_selected_slice.first->getSize().width() / 2),
	    event->pos().y() - (m_selected_slice.first->getSize().height() / 2)
	);

	// clang-format on

	m_selected_slice.first->setCoordinate(drag_coordinate);

	repaint();
}

void
GameView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_selected_slice.first == nullptr ||
	    m_selected_slice.second == nullptr) {
		return;
	}

	HanoiStack* destination_stack =
	    calculateStackByPos(event->position().toPoint());

	// clang-format off

        // checks:
        //  - if the destination stack is full -> cancel
        //  - if destination stack is the same with the source stack -> cancel
        //  - if the selected slice is bigger
        //     than the destination stack top value -> cancel

	if (destination_stack == nullptr ||
	    destination_stack->isFull() ||
	    destination_stack == m_selected_slice.second ||
	    (!destination_stack->isEmpty() &&
		(m_selected_slice.first->getValue() <
		    destination_stack->peek()->getValue())))
	{
		m_selected_slice.second->push(m_selected_slice.first);
	} else {
		destination_stack->push(m_selected_slice.first);
	}

	// clang-format on

	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	repaint();
}

void
GameView::resetGame()
{
	for (auto stack : m_stacks) {
		stack.second->clearStack();
	}

	init = true;

	repaint();
}
