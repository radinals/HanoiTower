#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QString>

class Config
{

      private:
        QString m_SliceSpritePath = ":/sprites/base_slice_sprite.png";
        QString m_StackBaseSpritePath = ":/sprites/stack_base_sprite.png";
        QString m_StackPoleSpritePath = ":/sprites/stack_pole_sprite.png";

	QColor m_StackTint = "#483333";
	QColor m_SliceTint = "#a62e2e";
	// QColor m_BackgroundColor;

	size_t m_slice_amount = 5;
	size_t m_stack_amount = 3;

	unsigned long long int m_timer_interval_ms = 60000 * 60;

      public:
        static Config& get()
        {
                static Config instance;
                return instance;
        }

	// clang-format off
	unsigned long long int getTimerInterval() {return m_timer_interval_ms; }
	QString getSliceSpritePath() { return m_SliceSpritePath; }
	QString getStackBaseSpritePath() { return m_StackBaseSpritePath; }
	QString getStackPoleSpritePath() { return m_StackPoleSpritePath; }
	QColor getStackTint() { return m_StackTint; }
	QColor getSliceTint() { return m_SliceTint; }
	// QColor getBackgroundColor() { return m_BackgroundColor; }
	void setTimerInterval(unsigned long long int ms) {m_timer_interval_ms = ms; }
	void setSliceSpritePath(QString p) { m_SliceSpritePath = p; }
	void setStackBaseSpritePath(QString p) { m_StackBaseSpritePath = p; }
	void setStackPoleSpritePath(QString p) { m_StackPoleSpritePath = p; }
	void setStackTint(QColor c) { m_StackTint = c; }
	void setSliceTint(QColor c) { m_SliceTint = c; }

        size_t getSliceAmount() {return m_slice_amount;}
        size_t getStackAmount() {return m_stack_amount;}
        void setSliceAmount(size_t n) { m_slice_amount = n;}
        void setStackAmount(size_t n) { m_stack_amount = n;}
        // void setBackgroundColor(QColor c) { m_BackgroundColor = c; }
        // clang-format on
};

#endif // CONFIG_H
