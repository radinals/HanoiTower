#ifndef HANOISTACK_H
#define HANOISTACK_H

#include <QPixmap>
#include <QString>
#include <QVector2D>
#include <map>

class HanoiSlice
{
      private:
        QPixmap* m_pixmap = nullptr;
        QVector2D m_coordinate = QVector2D(0, 0);
        unsigned int m_value = 0;

      public:
        // clang-format off
        HanoiSlice(){};
        HanoiSlice(HanoiSlice&&) = delete;
        ~HanoiSlice(){ delete m_pixmap; };
        HanoiSlice(QPixmap* pixmap, unsigned int value) : m_pixmap(pixmap), m_value(value){};

        inline void setCoordinate(const QVector2D coord) { m_coordinate = coord; }
        inline QVector2D getCoordinate() const { return m_coordinate; };
        inline QPixmap* getPixmap() const { return m_pixmap; }
        inline void setValue(unsigned int value) { m_value = value; }
        inline unsigned int getValue() const { return m_value; }
        // clang-format on

        HanoiSlice* next = nullptr;
};

class HanoiStack
{
      private:
        const static size_t m_max_slice_amount = 5;
        enum SliceColor {
                CLR_NONE = 0,
                CLR_RED = 1 < 2,
                CLR_GREEN = 2 < 2,
                CLR_BLUE = 3 < 2,
                CLR_YELLOW = 4 < 2,
                CLR_PURPLE = 5 < 2,
        };

        QSize m_sprite_size;

	QPixmap* m_stack_sprite;
	QSize m_stack_sprite_size;
	QVector2D m_stack_sprite_coordinate;

	unsigned int m_stack_colors = 0;
	size_t m_stack_slice_count = 0;

	HanoiSlice* m_head = nullptr;

	static void push(HanoiStack* stack, unsigned int color,
			 QVector2D coordinate);

	static QPixmap* generateSliceSprite(unsigned int color);

      public:
        HanoiStack(){};
        HanoiStack(HanoiStack&&) = delete;
        ~HanoiStack() { clearStack(); };

	// clang-format off
	inline bool isFull() { return m_stack_slice_count >= m_max_slice_amount; };
	inline bool isEmpty() { return m_stack_slice_count <= 0; }
	// clang-format on

	void clearStack();
	void push(HanoiSlice* slice);
	const HanoiSlice* const peek() const;
	HanoiSlice* pop();

	static bool isValidMove(HanoiSlice* src_top, HanoiSlice* dest_top);
	static void setStackFull(HanoiStack* stack);
};

#endif // HANOISTACK_H
