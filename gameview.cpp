#include "gameview.h"

#include "config.h"

#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

// TODO: Better size/placement calculations, for better dynamic ui

GameView::GameView(QWidget* parent) : QWidget{parent}
{
	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	for (size_t i = 0; i < m_max_stack_amount; i++) {
		m_stacks[i] = new HanoiStack;
	}

	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &GameView::checkWinState);
	// m_timer->setSingleShot(true);
}

GameView::~GameView()
{
	for (auto& stacks : m_stacks) {
		stacks.second->clearStack();
		delete stacks.second;
	}
}

void
GameView::calculatesSizes()
{
	m_stack_area_size.setWidth(geometry().width() /
				   Config::get().getStackAmount());
	m_stack_area_size.setHeight(geometry().height() * 0.9f);

	m_slice_base_size.setHeight(
	    (m_stack_area_size.height() / m_max_slice_amount));
	m_slice_base_size.setWidth(m_stack_area_size.width() * 0.9f);

	m_stack_base_size.setWidth(m_stack_area_size.width() * 0.8f);
	m_stack_base_size.setHeight(m_stack_area_size.height() * 0.1f);

	m_view_center = QVector2D(width() * 0.5f, height() * 0.5f);
}

void
GameView::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	if (!m_init) {
		for (auto stack : m_stacks) {
			scaleStack(stack.second);
		}
		m_init = true;
	}

	updateInfo();

	float offset = m_stack_area_size.width() * 0.5f;

	size_t count = 0;
	for (auto stack : m_stacks) {
		if (count >= Config::get().getStackAmount()) {
			break;
		}

		// draw the stack base
		drawStackBase(numToChar(stack.first), offset, &p);

		// configure each slice on the stack coordinates (placement)
		setStackCoordinates(offset, stack.second);

		// render the stack
		drawStack(stack.second, &p);

		// shift to the right for the next stack
		offset += m_stack_area_size.width();
		count++;
	}

	// render the selected stack
	if (m_selected_slice.first != nullptr &&
	    m_selected_slice.second != nullptr) {
		p.drawPixmap(m_selected_slice.first->getCoordinate().x(),
			     m_selected_slice.first->getCoordinate().y(),
			     m_selected_slice.first->getScaledPixmap());
	}
}

void
GameView::setStackCoordinates(float offset, HanoiStack* stack)
{
	// clang-format off
	if (stack == nullptr || stack->isEmpty()) { return; }

	float y_axis = height() - m_slice_base_size.height();

        HanoiSlice* slice = stack->getTail();
        while(slice != nullptr) {

                y_axis -= slice->height();

                QVector2D slice_coord(
                    offset - (slice->width() * 0.5f),
                    y_axis);

                slice->setCoordinate(slice_coord);

                slice = slice->prev;

        }

        // clang-format on
}

void
GameView::drawStack(HanoiStack* stack, QPainter* painter)
{
	// clang-format off
	if (stack == nullptr || stack->isEmpty()) { return; }

        HanoiSlice* slice = stack->getTail();
        while(slice != nullptr) {
            assert(slice->getPixmap() != nullptr);
            painter->drawPixmap(
                slice->getCoordinate().x(),
                slice->getCoordinate().y(),
                slice->getScaledPixmap());
            slice = slice->prev;
        }
        // clang-format on
}

void
GameView::drawStackBase(const QString& stack_label, float offset,
			QPainter* painter)
{
	// draw the stack label
	// painter->drawText(offset - (m_stack_area_size.width() * 0.5f),
	// m_view_center.y() * 1.2f, stack_label);

	QPixmap pole_sprite(Config::get().getStackPoleSpritePath());
	QPixmap stack_base_sprite(Config::get().getStackBaseSpritePath());
	colorizeSprite(&pole_sprite, Config::get().getStackTint());
	colorizeSprite(&stack_base_sprite, Config::get().getStackTint());

	painter->drawPixmap(
	    offset - ((m_slice_base_size.width() * 0.1f) * 0.5f),
	    height() - (m_slice_base_size.height() * (m_max_slice_amount)),
	    pole_sprite.scaled(m_slice_base_size.width() * 0.1f,
			       m_slice_base_size.height() *
				   (m_max_slice_amount)));

	painter->drawPixmap(
	    offset - ((m_slice_base_size.width() * 1.1f) * 0.5f),
	    height() - m_slice_base_size.height(),
	    stack_base_sprite.scaled(m_slice_base_size.width() * 1.1f,
				     m_slice_base_size.height()));
}

void
GameView::scaleStack(HanoiStack* stack)
{
	HanoiSlice* slice = stack->getTail();

	QSize size = m_slice_base_size;
	while (slice != nullptr) {
		size.setHeight(size.height() * 0.9f);
		size.setWidth(size.width() * 0.9f);
		slice->setSize(size);
		slice = slice->prev;
	}
}

void
GameView::resizeEvent(QResizeEvent* event)
{
	calculatesSizes();
}

HanoiStack*
GameView::calculateStackByPos(QPointF point)
{
	float x_area = m_stack_area_size.width();
	float y_area = m_stack_area_size.height();

	// clang-format off

	size_t count = 0;
	for (auto stack : m_stacks) {
		if (count >= Config::get().getStackAmount()) {
			break;;
		}
		float x = x_area / point.x(), y = y_area / point.y();

		// x and y should be 1 if the point is in the stack area

		if ((x >= 1) && (y >= 1) )
		{
			return m_stacks[stack.first];
		}

		// shift to the right, by stack area width
		x_area += m_stack_area_size.width();
	    count++;
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
	    event->pos().x() - (m_selected_slice.first->width() * 0.5),
	    event->pos().y() - (m_selected_slice.first->height() * 0.5)
	);

	// clang-format on

	m_selected_slice.first->setCoordinate(drag_coordinate);

	update();
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
	    destination_stack == m_selected_slice.second ||
	    (!destination_stack->isEmpty() &&
		(m_selected_slice.first->getValue() <
		    destination_stack->peek()->getValue())))
	{
		m_selected_slice.second->push(m_selected_slice.first);
	} else {
		if (!m_timer_started) {
		    // m_timer->setSingleShot(true);
		    m_timer->start(1);
		    m_timer_started = true;
		}
		destination_stack->push(m_selected_slice.first);
		m_move_count++;
	}

	// clang-format on

	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	update();
}

void
GameView::colorizeSprite(QPixmap* sprite, const QColor& color)
{
	QPixmap mask(*sprite);

	QPainter painter;

	painter.begin(&mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

	painter.fillRect(mask.rect(), color);
	painter.end();

	painter.begin(sprite);
	painter.setCompositionMode(QPainter::CompositionMode_Overlay);
	painter.drawImage(0, 0, mask.toImage());

	painter.end();
}

void
GameView::init()
{
	calculatesSizes();
	m_timer_elapsed = 0;

	m_init = false;
	m_timer_started = false;
	m_timer->stop();

	for (auto stack : m_stacks) {
		stack.second->clearStack();
	}

	HanoiStack::generate_stack(m_stacks.at(0),
				   Config::get().getSliceAmount());

	HanoiSlice* slice = m_stacks.at(0)->getHead();

	while (slice != nullptr) {
		colorizeSprite(slice->getPixmap(),
			       Config::get().getSliceTint());
		slice = slice->next;
	}
	updateInfo();

	repaint();
}

void
GameView::triggerLoseDialog()
{
	QMessageBox::information(this, "YOU LOSE!", "LOSER");
	init();
}

void
GameView::triggerWinDialog()
{
	QMessageBox::information(this, "YOU WIN!", "WINNER");
	init();
}

void
GameView::checkWinState()
{

	m_timer_elapsed++;
	updateInfo();

	if (m_stacks.at(Config::get().getStackAmount() - 1)->getSliceCount() >=
	    Config::get().getSliceAmount()) {
		triggerWinDialog();
	} else if (m_timer_elapsed >= Config::get().getTimerInterval()) {
		triggerLoseDialog();
	}
}

void
GameView::updateInfo()
{
	if (m_time_output != nullptr) {

		unsigned long long int ms = 0;

		ms = Config::get().getTimerInterval() - m_timer_elapsed;

		QString h, m, s;

		unsigned long long int hours = 0, minutes = 0, seconds = 0;

		// Calculate total seconds from milliseconds
		unsigned long long int total_seconds = ms / 1000;

		// Calculate hours
		hours = total_seconds / 3600;

		// Calculate remaining minutes
		minutes = (total_seconds % 3600) / 60;

		// Calculate remaining seconds
		seconds = (ms % 60000) / 1000.0;

		h = QString::number(hours);
		m = QString::number(minutes);
		s = QString::number(seconds);

		if (hours < 10) {
			h = '0' + h;
		}

		if (minutes < 10) {
			m = '0' + m;
		}

		if (seconds < 10) {
			s = '0' + s;
		}

		m_time_output->setText(h + ":" + m + ":" + s);
	}

	if (m_score_output != nullptr) {
		m_score_output->setText(QString::number(m_score));
	}

	if (m_move_count_output != nullptr) {
		m_move_count_output->setText(QString::number(m_move_count));
	}
}
