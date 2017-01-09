#include "bubbleitem.h"
#include <QFontMetrics>
#include <QPainter>

BubbleItem::BubbleItem()
{
    this->setZValue(100);
}

BubbleItem::BubbleItem(const QFont &font, const QString &str)
{
    setFont(font);
    setStr(str);
    this->setZValue(100);
}

void BubbleItem::setFont(const QFont &font)
{
    m_font = font;
    calcStrRect();
}

void BubbleItem::setStr(const QString &str)
{
    m_str = str;
    calcStrRect();
}

void BubbleItem::calcStrRect()
{
    QFontMetrics fm(m_font);
    m_strRect = fm.boundingRect(
                QRect(-5,-105,100,100),
                Qt::AlignLeft | Qt::AlignBottom,
                m_str);
}

QRectF BubbleItem::boundingRect() const
{
    return QRectF(-50, -50, 50, 50);
    return m_strRect.adjusted(0,0,0,5);
}

void BubbleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    QPointF points[6] = {
        QPointF(0, 0),
        QPointF(-5, -5),
        QPointF(-5, m_strRect.top()),
        QPointF(m_strRect.right(), m_strRect.top()),
        QPointF(m_strRect.right(), -5),
        QPointF(5, -5)
    };
    painter->setBrush(Qt::yellow);
    painter->drawPolygon(points, 6);
    painter->setFont(m_font);
    painter->drawText(m_strRect, m_str);
}
