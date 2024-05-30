#include "gameview.h"

#include "config.h"
#include "soundplayer.h"

#include <QFont>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <qmessagebox.h>
#include <qrandom.h>
#include <random>

GameView::GameView(QWidget* parent) : QWidget{parent}
{
	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	for (size_t i = 0; i < Config::get().getStackMax(); i++) {
		m_stacks[i] = new HanoiStack;
	}

	m_timer = new QTimer(this);

	connect(m_timer, &QTimer::timeout, this, &GameView::checkWinState);

	m_placement_fx.setSource(Config::get().getPlacementFXAudioPath());

	m_gameover_dialog = new QMessageBox(this);

	m_gameover_dialog_no_btn =
	    m_gameover_dialog->addButton(QMessageBox::Yes);

	m_gameover_dialog_yes_btn =
	    m_gameover_dialog->addButton(QMessageBox::No);
}

GameView::~GameView()
{
	for (auto& stacks : m_stacks) {
		stacks.second->clearStack();
		delete stacks.second;
	}

	delete m_timer;
	delete m_gameover_dialog;
}

// generate the base sizes to be used to render the sprites and etc.
void
GameView::calculatesSizes()
{
	m_stack_area_size.setWidth(geometry().width() /
	                           Config::get().getStackAmount());
	m_stack_area_size.setHeight(geometry().height() * 0.9f);

	m_slice_base_size.setHeight(
	    (m_stack_area_size.height() / Config::get().getSliceMax()));
	m_slice_base_size.setWidth(m_stack_area_size.width() * 0.9f);

	m_stack_base_size.setWidth(m_slice_base_size.width() * 1.1f);
	m_stack_base_size.setHeight(m_slice_base_size.height());

	m_view_center = QVector2D(width() * 0.5f, height() * 0.5f);
}

void
GameView::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	updateInfo();

	float offset = m_stack_area_size.width() * 0.5f;

	size_t count = 0;
	for (auto stack : m_stacks) {

		if (count >= Config::get().getStackAmount()) {
			break;
		}

		if (!m_init) {
			scaleStack(stack.second);
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
	m_init = true;

	// clang-format off
	// render the selected stack
	if (m_selected_slice.first != nullptr && m_selected_slice.second != nullptr)
	{
		p.drawPixmap(
			m_selected_slice.first->getCoordinate().x(),
		        m_selected_slice.first->getCoordinate().y(),
		        m_selected_slice.first->getScaledPixmap()
		);
	}
	// clang-format on
}

// configure each slice placement in the screen, starting from the bottom
// of the stack
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

// draw the stack's slices
void
GameView::drawStack(HanoiStack* stack, QPainter* painter)
{
	// clang-format off
	if (stack == nullptr || stack->isEmpty()) { return; }

        HanoiSlice* slice = stack->getTail();
        while(slice != nullptr)
	{
            painter->drawPixmap(
                slice->getCoordinate().x(),
                slice->getCoordinate().y(),
                slice->getScaledPixmap()
	    );

            slice = slice->prev;
        }
	// clang-format on
}

// render the stack base
void
GameView::drawStackBase(size_t label, float offset, QPainter* painter)
{

	QPixmap pole_sprite(Config::get().getStackPoleSpritePath());
	QPixmap stack_base_sprite(Config::get().getStackBaseSpritePath());

	pole_sprite = pole_sprite.scaled(m_stack_base_size.width() * 0.1f,
	                                 m_stack_base_size.height() *
	                                     Config::get().getSliceMax());

	stack_base_sprite = stack_base_sprite.scaled(
	    m_stack_base_size.width(), m_stack_base_size.height());

	// clang-format off

	colorizeSprite(&pole_sprite, Config::get().getStackTint());
	colorizeSprite(&stack_base_sprite, Config::get().getStackTint());

        // draw the pole
        painter->drawPixmap(
            offset - (pole_sprite.width() * 0.5f),
            height() - (m_stack_base_size.height() * (Config::get().getSliceMax())),
            pole_sprite
        );

	// draw the base
	painter->drawPixmap(
	    offset - (m_stack_base_size.width() * 0.5f),
	    height() - m_stack_base_size.height(),
	    stack_base_sprite
	);

	QString stack_label = numToChar(label);
	QFont font(Config::get().getStackLabelFont(), m_stack_base_size.width() * 0.1f);

	QColor font_color;

	if (label == m_goal_stack_index) {

		unsigned int box_size = (m_stack_base_size.width() * 0.1f) + 4;

		font_color = Config::get().getStackLabelFontColorHighlight();

		painter->fillRect(
		    offset - (box_size * 0.5f),
		    height() - (m_stack_base_size.height() * Config::get().getSliceMax() + (box_size * 0.5f) ),
		    box_size,
		    box_size * 0.2f, Config::get().getStackLabelFontColorHighlight()
		);

	} else {
		font_color = Config::get().getStackLabelFontColor();
	}

	painter->setPen(font_color);
	painter->setFont(font);

	// draw the stack label
	painter->drawText(
	    offset - (painter->font().pointSizeF() * 0.5f),
	    height() - (m_stack_base_size.height() * Config::get().getSliceMax()
		+ painter->font().pointSizeF()),
	    stack_label
	);

	// clang-format on
}

//   set Slices size to be 90% of the base slice size and
//   the size keeps getting smaller for each slice
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

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
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

// on mouse press, pop the slice of the stack below the mouse click,
// and store it.
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

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
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

// on mouse release, if holding any slice, place the slice in
// the stack where the mouse was released, or just put it back.
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

	if (destination_stack == nullptr || destination_stack == m_selected_slice.second ||
	    (!destination_stack->isEmpty() && (m_selected_slice.first->getValue()
		< destination_stack->peek()->getValue())))
	{
		m_selected_slice.second->push(m_selected_slice.first);
	} else {

		if (!m_timer_started) {
		    m_timer->start(1);
		    m_timer_started = true;
		}

		destination_stack->push(m_selected_slice.first);

		m_move_count++;
	}

	// clang-format on

	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	m_placement_fx.getSound()->play();

	update();
}

// add tint to a pixmap, by using masks
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

// (re)initialize an empty state
void
GameView::reset()
{
	clear();
	updateInfo();
	repaint();
	showGameGoalDialog();
}

void
GameView::clear()
{
	calculatesSizes();

	m_goal_stack_index = getRandomGoalStackIndex();

	m_placement_fx.getSound()->setVolume(
	    Config::get().getAudioFXVolumeLevel());

	m_timer_elapsed = m_move_count = 0;
	m_init = m_timer_started = false;
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
}

void
GameView::triggerLoseDialog()
{
	m_gameover_dialog->setText("GAMEOVER.. YOU LOST");
	m_gameover_dialog->setInformativeText(
	    "You've Failed to Complete the Puzzle!\nDo You Want to Play Again?...");
	m_gameover_dialog->exec();
}

void
GameView::triggerWinDialog()
{
	m_gameover_dialog->setText("GAMEOVER.. YOU WIN");
	m_gameover_dialog->setInformativeText(
	    "You've Sucessfully to Complete the Puzzle!\nDo You Want to Play Again?...");
	m_gameover_dialog->exec();
}

void
GameView::checkWinState()
{

	m_timer_elapsed++;
	updateInfo();

	if (goalStackIsComplete()) {
		triggerWinDialog();
	} else if (m_timer_elapsed >= Config::get().getTimerInterval()) {
		triggerLoseDialog();
	}
}

// update the Game's sidebar information
// the current time is calculated from subtracting the start time with a
// time elapsed counter.
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

		// clang-format off
		if (hours < 10) { h = '0' + h; }
		if (minutes < 10) { m = '0' + m; }
		if (seconds < 10) { s = '0' + s; }
		// clang-format on

		m_time_output->setText(h + ":" + m + ":" + s);
	}

	if (m_score_output != nullptr) {
		m_score_output->setText(QString::number(m_score));
	}

	if (m_move_count_output != nullptr) {
		m_move_count_output->setText(QString::number(m_move_count));
	}
}

size_t
GameView::getRandomGoalStackIndex()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> distr(
	    1, Config::get().getStackAmount() - 1);
	return distr(gen);
}

void
GameView::showGameGoalDialog()
{
	QMessageBox dialog(this);
	dialog.setText("GOALS");
	dialog.setInformativeText(
	    "Move All Stack From Peg 'A' to '" + numToChar(m_goal_stack_index) +
	    "'. No Slice may be placed on top of a slice that is smaller to it!.");
	dialog.setStandardButtons(QMessageBox::Ok);
	dialog.exec();
}

bool
GameView::goalStackIsComplete()
{
	return (m_stacks.at(m_goal_stack_index)->getSliceCount() ==
	        Config::get().getSliceAmount());
}
