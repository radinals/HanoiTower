#include "gameview.h"

#include "config.h"
#include "hanoistack.h"
#include "linkedlist.h"
#include "utils.h"

#include <QFont>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <cmath>
#include <qaccessible_base.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qrandom.h>
#include <random>

GameView::GameView(QWidget* parent) : QWidget{parent}
{
	// init selected slice var
	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	m_stacks = std::vector(Config::get().getStackMax(), HanoiStack());

	// init timer.
	// timer will call checkWinState every tick (should be every 1ms).
	connect(&m_timer, &QTimer::timeout, this, &GameView::checkWinState);

	// load the placement sound effect
	m_placement_fx = new QSoundEffect(this);
	m_placement_fx->setSource("qrc" +
	                          Config::get().getPlacementFXAudioPath());
	m_placement_fx->setVolume(Config::get().getAudioFXVolumeLevel());

	m_pole_sprite = QPixmap(Config::get().getStackPoleSpritePath());
	m_stack_base_sprite = QPixmap(Config::get().getStackBaseSpritePath());
	m_arrow_sprite = QPixmap(Config::get().getArrowSpritePath());
	m_win_dialog = QPixmap(Config::get().getWinScreen());
	m_lose_dialog = QPixmap(Config::get().getLoseScreen());

	// tint the sprites
	colorizeSprite(&m_pole_sprite, Config::get().getStackTint());
	colorizeSprite(&m_stack_base_sprite, Config::get().getStackTint());
	colorizeSprite(&m_arrow_sprite, Config::get().getHighlightColor());
}

void
GameView::pause()
{
	switch (m_game_state) {
	case GameState::GamePaused:
		m_timer.start(1);
		updateInfo();
		m_game_state = GameState::GameRunning;
		break;
	case GameState::GameRunning:
		m_time_output->setText("PAUSED");
		m_timer.stop();
		m_game_state = GameState::GamePaused;
		break;
	default:
		return;
	}
}

// (re)initialize an empty state
void
GameView::reset()
{
	clear();
	updateInfo();
	m_game_state = GameState::GameRunning;
	repaint();
}

void
GameView::clear()
{
	calculatesSizes();

	m_goal_stack_index = getRandomGoalStackIndex();

	m_info_box->setText("Move All Slice to Stack " +
	                    numToChar(m_goal_stack_index));
	m_info_box->setAlignment(Qt::AlignCenter);

	m_timer_elapsed = m_move_count = 0;
	m_game_state = GameState::GameNotRunning;
	m_timer.stop();
	m_slice_list.clear();

	// clang-format off

	// clear all stack
	for (size_t label = 0; label < Config::get().getStackAmount(); label++) {
		m_stacks.at(label).clearStack();
	}

	// populate the first stack
	HanoiStack::generate_stack(&m_stacks.at(0), Config::get().getSliceAmount());

	HanoiSlice* slice = m_stacks.at(0).getHead();
	while (slice != nullptr) {
		colorizeSprite(&slice->getPixmap(), Config::get().getSliceTint());
		m_slice_list.append(slice); // save the slices for ease of access
		slice = slice->next;
	}

	// scale the slices
	scaleSlices();

	// clang-format on
}

// generate the base sizes to be used to render the sprites and etc.
void
GameView::calculatesSizes()
{

	m_stack_area_size.setWidth(float(width()) /
	                           Config::get().getStackAmount());

	m_stack_area_size.setHeight(height() * 0.9f);

	m_slice_base_size.setHeight(m_stack_area_size.height() /
	                            Config::get().getSliceMax());

	m_slice_base_size.setWidth(m_stack_area_size.width() * 0.9f);

	m_stack_base_size.setWidth(m_slice_base_size.width() * 1.1f);
	m_stack_base_size.setHeight(m_slice_base_size.height());

	m_gameover_dialog_size.setWidth(width() * 0.2f);
	m_gameover_dialog_size.setHeight(height() * 0.2f);
}

// - draw the stack's slices
// - configure each slice placement in the screen, starting from the bottom
//   of the stack
// - set Slices size to be 90% of the base slice size and the size keeps
//   getting smaller for each slice
void
GameView::drawStack(float offset, HanoiStack* stack, QPainter* painter)
{
	// clang-format off

	if (stack == nullptr || stack->isEmpty()) { return; }

	float y_axis = (height() - m_slice_base_size.height());

        HanoiSlice* slice = stack->getTail();
        while(slice != nullptr)
        {
            y_axis -= std::floor(slice->getHeight());

	    slice->setX(offset - (slice->getWidth() * 0.5f));
	    slice->setY(y_axis);

	    painter->drawPixmap(QPointF(slice->getX(),slice->getY()), slice->getScaledPixmap());

            slice = slice->prev;
        }

	// clang-format on
}

// render the stack base
void
GameView::drawStackBase(size_t label, float offset, QPainter* painter)
{
	if (painter == nullptr || !painter->isActive()) {
		return;
	}

	// clang-format off

	//--Draw Stack Base+Pole----------------------------------------------

	const float pole_height = m_stack_base_size.height() * Config::get().getSliceMax(),
	            pole_base_y_offset = height() - pole_height;

	// scale the sprites
	const QPixmap pole_sprite = m_pole_sprite.scaled(
		m_stack_base_size.width() * 0.1f,
		pole_height
	);

	const QPixmap stack_base_sprite = m_stack_base_sprite.scaled(
		m_stack_base_size.width(),
		m_stack_base_size.height()
	);

	const QPixmap arrow_sprite = m_arrow_sprite.scaled(
		offset - m_stack_area_size.width() * 0.5f,
		m_stack_base_size.width() * 0.1f
	);

        // draw the pole
        painter->drawPixmap(
            offset - (pole_sprite.width() * 0.5f),
            pole_base_y_offset,
            pole_sprite
        );

	// draw the base
	painter->drawPixmap(
	    offset - (m_stack_base_size.width() * 0.5f),
	    height() - m_stack_base_size.height(),
	    stack_base_sprite
	);

	//--Draw Stack Label--------------------------------------------------


	const QFont font(
		Config::get().getStackLabelFont(), // fontname
		m_stack_base_size.width() * 0.1f // size
	);

	const QString stack_label = numToChar(label);

	QColor font_color = Config::get().getStackLabelFontColor();

	// highlight + draw the indicator if current stack is the goal stack
	if (label == m_goal_stack_index) {

		const float box_size = (m_stack_base_size.width() * 0.1f) + 4;

		font_color = Config::get().getHighlightColor();

		if (!m_timer.isActive()) {
			painter->drawPixmap(
				m_stack_area_size.width() * 0.5f,
				pole_base_y_offset - (arrow_sprite.height()), // y
				arrow_sprite
			);
		} else {
			painter->fillRect(
			    offset - (box_size * 0.5f),             // x
			    pole_base_y_offset - (box_size * 0.5f), // y
			    box_size,                               // w
			    box_size * 0.2f,                        // h
			    Config::get().getHighlightColor()       // color
			);
		}
	}

	// set font & color for drawing the label
	painter->setPen(font_color); painter->setFont(font);

	// draw the stack label
	painter->drawText(
	    offset - (painter->font().pointSizeF() * 0.5f),
	    pole_base_y_offset - painter->font().pointSizeF(),
	    stack_label
	);

	// clang-format on
}

void
GameView::scaleSlices()
{
	// clang-format off

	float w = m_slice_base_size.width(),
	      h = m_stack_base_size.height();

	auto node = m_slice_list.m_head;
	while (node != nullptr) {
		node->data->setHeight(h *= m_slice_scale_factor);
		node->data->setWidth(w *= m_slice_scale_factor);
		node = node->next;
	}

	// clang-format on
}

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
HanoiStack*
GameView::calculateStackByPos(QPointF point)
{
	// clang-format off

	float x_area = m_stack_area_size.width(),
	      y_area = m_stack_area_size.height();

	for (size_t label = 0; label < Config::get().getStackAmount(); label++) {

		float x = x_area / point.x(), y = y_area / point.y();

		// x and y should be 1 if the point is in the stack area
		if ((x >= 1) && (y >= 1) ) { return &m_stacks.at(label); }

		// shift to the right, by stack area width
		x_area += m_stack_area_size.width();
	}

	// clang-format on

	return nullptr;
}

// add tint to a pixmap, by using masks
void
GameView::colorizeSprite(QPixmap* sprite, const QColor& color)
{
	if (sprite == nullptr) {
		return;
	}

	QPixmap mask(*sprite);

	QPainter painter;

	// create the mask
	painter.begin(&mask);
	if (painter.isActive()) {
		painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
		painter.fillRect(mask.rect(), color);
		painter.end();
	}

	// apply the mask
	painter.begin(sprite);
	if (painter.isActive()) {
		painter.setCompositionMode(QPainter::CompositionMode_Overlay);
		painter.drawImage(0, 0, mask.toImage());
		painter.end();
	}
}

void
GameView::checkWinState()
{
	m_timer_elapsed++;
	updateInfo();

	if (goalStackIsComplete()) {
		m_game_state = GameState::GameOverWon;
		m_timer.stop();
		repaint();
	} else if (m_timer_elapsed >= Config::get().getTimerInterval()) {
		m_game_state = GameState::GameOverLost;
		m_timer.stop();
		repaint();
	}
}

// update the Game's sidebar information
// the current time is calculated from subtracting the start time with a
// time elapsed counter.
void
GameView::updateInfo()
{
	if (m_time_output != nullptr) {

		auto hh_mm_ss = Utils::extractTimeFromMs(
		    Config::get().getTimerInterval() - m_timer_elapsed);

		QString h, m, s;
		h = QString::number(std::get<0>(hh_mm_ss));
		m = QString::number(std::get<1>(hh_mm_ss));
		s = QString::number(std::get<2>(hh_mm_ss));

		// clang-format off
		if (std::get<0>(hh_mm_ss) < 10) { h = '0' + h; }
		if (std::get<1>(hh_mm_ss) < 10) { m = '0' + m; }
		if (std::get<2>(hh_mm_ss) < 10) { s = '0' + s; }
		// clang-format on

		m_time_output->setText(h + ":" + m + ":" + s);
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
GameView::paintEvent(QPaintEvent* event)
{

	if (m_game_state == GameState::GameNotRunning)
		return;

	QPainter p(this);

	updateInfo();

	// clang-format off

	float offset = m_stack_area_size.width() * 0.5f;
	for (size_t label = 0; label < Config::get().getStackAmount(); label++) {

		HanoiStack* stack = &m_stacks.at(label);

		// draw the stack base
		drawStackBase(label, offset, &p);

		// render the stack
		drawStack(offset, stack, &p);

		// shift to the right for the next stack
		offset += m_stack_area_size.width();
	}

	// render the selected stack
	if (m_selected_slice.first != nullptr && m_selected_slice.second != nullptr)
	{
		p.drawPixmap(
			m_selected_slice.first->getX(),
			m_selected_slice.first->getY(),
			m_selected_slice.first->getScaledPixmap()
		);
	}


	switch (m_game_state) {
	case GameState::GameOverLost: {
			QPixmap lose_dialog = m_lose_dialog.scaled(
				m_gameover_dialog_size.toSize(),
				Qt::KeepAspectRatioByExpanding
			);

			p.drawPixmap(
				(width() * 0.5f) -
					m_gameover_dialog_size.width() * 0.5f,
				(height() * 0.5f) -
					m_gameover_dialog_size.height() * 0.5f,
				lose_dialog
			);

		}break;

	case GameState::GameOverWon: {
			QPixmap win_dialog = m_win_dialog.scaled(
				m_gameover_dialog_size.toSize(),
				Qt::KeepAspectRatioByExpanding
			);

			p.drawPixmap(
				(width() * 0.5f) -
					m_gameover_dialog_size.width() * 0.5f,
				(height() * 0.5f) -
					m_gameover_dialog_size.height() * 0.5f,
				win_dialog
			);

		}break;

	default:
		break;
	}

	// clang-format on
}

void
GameView::resizeEvent(QResizeEvent* event)
{
	calculatesSizes();
	if (m_game_state != GameState::GameNotRunning) {
		scaleSlices();
	}
}

// on mouse press, pop the slice of the stack below the mouse click,
// and store it.
void
GameView::mousePressEvent(QMouseEvent* event)
{
	if (m_selected_slice.first != nullptr ||
	    m_selected_slice.second != nullptr ||
	    m_game_state != GameState::GameRunning) {
		return;
	}

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

	moveSelectedSlice(event->pos().x(), event->pos().y());
}

// on mouse move, if a slice is stored from a clicked event, update
// the slice coordinate to be the cursor coordinates and redraw screen.
void
GameView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_selected_slice.first == nullptr ||
	    m_selected_slice.second == nullptr ||
	    m_game_state != GameState::GameRunning) {
		return;
	}

	moveSelectedSlice(event->pos().x(), event->pos().y());
}

// on mouse release, if holding any slice, place the slice in
// the stack where the mouse was released, or just put it back.
void
GameView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_selected_slice.first == nullptr ||
	    m_selected_slice.second == nullptr ||
	    m_game_state != GameState::GameRunning) {
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

		if (m_game_state == GameState::GameRunning && !m_timer.isActive()) {
		    m_timer.start(1);
		}

		destination_stack->push(m_selected_slice.first);

		m_move_count++;
	}

	// clang-format on

	m_selected_slice.first = nullptr;
	m_selected_slice.second = nullptr;

	update();

	if (!m_placement_fx->isPlaying()) {
		m_placement_fx->play();
	}
}
