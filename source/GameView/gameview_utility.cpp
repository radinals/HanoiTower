#include "gameview.h"

#include "../Config/config.h"
#include <string>

// compare the QPointF x and y values to a stack's area, if
// if said point is in a stack's area, return the pointer
// to the stack
HanoiStack*
GameView::calculateStackByPos(const QPointF& point)
{
    const float stack_area_height = m_geometry.stack_area.height();
    const float stack_area_width  = m_geometry.stack_area.width();

    float area_width = stack_area_width;

    for (size_t i = 0; i < Config::get().Settings().stack_amount; i++) {
        const float x = (point.x() != 0) ? (point.x() / area_width) : 0;
        const float y = (point.y() != 0) ? (point.y() / stack_area_height) : 0;

        if ((x >= 0 && x <= 1) && (y >= 0 && y <= 1)) {
            return &m_hanoi.stacks[i];
        }

        area_width += stack_area_width;
    }
    return nullptr;
}

// add tint to a pixmap, by using masks
void
GameView::colorizeSprite(QPixmap* sprite, const QColor& color)
{
    if (sprite == nullptr || sprite->isNull()) {
        std::runtime_error("colorizeSprite(): invalid pixmap was passed");
    }

    QPixmap  mask(*sprite);
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

// convert the numeric labels of stacks to alphabets
QString
GameView::numToChar(size_t n)
{
    std::string str;
    str += char('A' + n);
    return QString::fromStdString(str);
};
