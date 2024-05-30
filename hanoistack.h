#ifndef HANOISTACK_H
#define HANOISTACK_H

#include "config.h"

#include <QPixmap>
#include <QString>
#include <QVector2D>

class HanoiSlice
{
      private:
	QPixmap* m_pixmap = nullptr;
	QSize m_size;
	QVector2D m_coordinate = QVector2D(0, 0);
	unsigned int m_value = 0;

      public:
	// clang-format off
        HanoiSlice(HanoiSlice&&) = delete;

        HanoiSlice(){ m_pixmap = new QPixmap(Config::get().getSliceSpritePath()); };
        HanoiSlice(unsigned int value) : m_value(value){m_pixmap = new QPixmap(Config::get().getSliceSpritePath());};
        ~HanoiSlice(){ delete m_pixmap; };

        inline void setCoordinate(const QVector2D coord) { m_coordinate = coord; }
        inline QVector2D getCoordinate() const { return m_coordinate; };
        inline QPixmap* getPixmap() const { return m_pixmap; }
        inline QPixmap getScaledPixmap() const { return m_pixmap->scaled(m_size); }
        inline void setValue(unsigned int value) { m_value = value; }
        inline void setSize(QSize size) { m_size = size; }
        inline QSize getSize() const { return m_size; }
        inline unsigned int getValue() const { return m_value; }

        inline int width() {return m_size.width(); }
        inline int height() {return m_size.height(); }
	// clang-format on

	HanoiSlice* next = nullptr;
	HanoiSlice* prev = nullptr;
};

class HanoiStack
{
      private:
	unsigned int m_stack_colors = 0;
	size_t m_stack_slice_count = 0;

	HanoiSlice *m_head = nullptr, *m_tail = nullptr;

      public:
	HanoiStack(){};
	HanoiStack(HanoiStack&&) = delete;
	~HanoiStack() { clearStack(); };

	// clang-format off
	inline bool isEmpty() const { return m_stack_slice_count <= 0; }
	// clang-format on

	void clearStack();

	void push(HanoiSlice* slice);

	const HanoiSlice* const peek() const;
	HanoiSlice* pop();

	inline HanoiSlice* const getTail() { return m_tail; }
	inline HanoiSlice* const getHead() { return m_head; }
	inline size_t getSliceCount() const { return m_stack_slice_count; };

	static void generate_stack(HanoiStack* stack, size_t slice_amount);
};

#endif // HANOISTACK_H
