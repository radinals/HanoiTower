#ifndef HANOISTACK_H
#define HANOISTACK_H

#include <QPixmap>
#include <QString>
#include <QVector2D>
#include <list>

class HanoiSlice
{
      private:
        QPixmap* m_pixmap = nullptr;
        QSize m_size;
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
        inline void setSize(QSize size) { m_size = size; }
        inline QSize getSize() { return m_size; }
        inline unsigned int getValue() const { return m_value; }
        // clang-format on

        HanoiSlice* next = nullptr;
};

class HanoiStack
{
      public:
        enum class SliceColor {
                NONE,
                RED,
                GREEN,
                BLUE,
                YELLOW,
                PURPLE,
        };

      private:
        const static size_t m_max_slice_amount = 5;

	unsigned int m_stack_colors = 0;
	size_t m_stack_slice_count = 0;

	HanoiSlice* m_head = nullptr;

	static void push(HanoiStack* stack, SliceColor color);

	static QPixmap* generateSliceSprite(SliceColor);

	static SliceColor getSliceValueColor(unsigned int);

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
	std::list<QPixmap> getSprites(QSize base_size);

	static bool isValidMove(HanoiSlice* src_top, HanoiSlice* dest_top);
	static void setStackFull(HanoiStack* stack);
	static unsigned int getSliceColorValue(SliceColor color);
	std::list<HanoiSlice*> getSlices();
	void scaleSlices(QSize base_sprite_size);
};

#endif // HANOISTACK_H
