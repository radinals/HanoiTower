#include "hanoistack.h"

#include "config.h"

#include <QPainter>
#include <stdexcept>

void
HanoiStack::clearStack()
{
	while (!isEmpty()) {
		delete pop();
	}
}

QPixmap*
HanoiStack::generateSliceSprite(SliceColor slice_color)
{
	QPixmap* pixmap = new QPixmap(Config::SliceSpriteBasePath);
	QPixmap mask(*pixmap);

	QPainter painter;

	painter.begin(&mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

	QColor qcolor;
	switch (slice_color) {
	case SliceColor::RED:
		qcolor = Qt::red;
		break;
	case SliceColor::GREEN:
		qcolor = Qt::green;
		break;
	case SliceColor::BLUE:
		qcolor = Qt::blue;
		break;
	case SliceColor::YELLOW:
		qcolor = Qt::yellow;
		break;
	case SliceColor::PURPLE:
		qcolor = Qt::magenta;
		break;
	default:
		qcolor = Qt::black;
		break;
	};

	painter.fillRect(mask.rect(), qcolor);
	painter.end();

	painter.begin(pixmap);
	painter.setCompositionMode(QPainter::CompositionMode_Overlay);
	painter.drawImage(0, 0, mask.toImage());

	painter.end();

	return pixmap;
}

unsigned int
HanoiStack::getSliceColorValue(SliceColor color)
{
	switch (color) {
	case SliceColor::RED:
		return 5;
	case SliceColor::GREEN:
		return 4;
	case SliceColor::BLUE:
		return 3;
	case SliceColor::YELLOW:
		return 2;
	case SliceColor::PURPLE:
		return 1;
	default:
		return 0;
	}
}

HanoiStack::SliceColor
HanoiStack::getSliceValueColor(unsigned int value)
{
	switch (value) {
	case 5:
		return SliceColor::RED;
	case 4:
		return SliceColor::GREEN;
	case 3:
		return SliceColor::BLUE;
	case 2:
		return SliceColor::YELLOW;
	case 1:
		return SliceColor::PURPLE;
	}
	return SliceColor::NONE;
}

void
HanoiStack::push(HanoiSlice* slice)
{
	if (isFull()) {
		throw std::length_error("Stack Is Full");
	}

	if (isEmpty()) {
		m_head = slice;
	} else {
		HanoiSlice* old_head = m_head;
		slice->next = old_head;
		m_head = slice;
	}
	m_stack_slice_count++;
}

void
HanoiStack::push(HanoiStack* stack, SliceColor color)
{
	QPixmap* sprite_pixmap = generateSliceSprite(color);
	HanoiSlice* nw_slice =
	    new HanoiSlice(sprite_pixmap, getSliceColorValue(color));
	stack->push(nw_slice);
}

const HanoiSlice* const
HanoiStack::peek() const
{
	return m_head;
}

HanoiSlice*
HanoiStack::pop()
{
	if (isEmpty()) {
		throw std::out_of_range("Stack is Empty");
	}

	HanoiSlice* popped = m_head;
	m_head = m_head->next;
	--m_stack_slice_count;
	popped->next = nullptr;

	return popped;
}

bool
HanoiStack::isValidMove(HanoiSlice* src_top, HanoiSlice* dest_top)
{
	return dest_top->getValue() < src_top->getValue();
}

void
HanoiStack::setStackFull(HanoiStack* stack)
{
	push(stack, SliceColor::PURPLE);
	push(stack, SliceColor::YELLOW);
	push(stack, SliceColor::BLUE);
	push(stack, SliceColor::GREEN);
	push(stack, SliceColor::RED);
}

std::list<QPixmap>
HanoiStack::getSprites(QSize base_size)
{
	std::list<QPixmap> sprites;
	HanoiSlice* slice = m_head;

	unsigned int max = UINT_MAX;
	while (slice != nullptr) {
		QSize scale = base_size;
		scale.setWidth(scale.width() - (50 * slice->getValue()));
		scale.setHeight(scale.height() - (5 * slice->getValue()));
		sprites.push_front(slice->getPixmap()->scaled(scale));
		slice = slice->next;
	}

	return sprites;
}

void
HanoiStack::scaleSlices(QSize base_sprite_size)
{
	HanoiSlice* slice = m_head;
	while (slice != nullptr) {
		QSize scale = base_sprite_size;
		scale.setWidth(scale.width() - (50 * slice->getValue()));
		scale.setHeight(scale.height() - (5 * slice->getValue()));
		slice->setSize(scale);
		slice = slice->next;
	}
}

std::list<HanoiSlice*>
HanoiStack::getSlices()
{
	std::list<HanoiSlice*> slices;
	HanoiSlice* slice = m_head;

	while (slice != nullptr) {
		slices.push_front(slice);
		slice = slice->next;
	}

	return slices;
}
