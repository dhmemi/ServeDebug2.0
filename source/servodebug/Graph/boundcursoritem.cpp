#include "boundcursoritem.h"
#include "graphitem.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

BoundCursorItem::BoundCursorItem(GraphItem *parent, int ID)
{
    m_parent = parent;
    m_ID = ID;
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

BoundCursorItem::~BoundCursorItem()
{
}

void BoundCursorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    int y = event->pos().y() + pos().y();
    //if (y<0) y = 0;
    //if (y>=m_parent->m_height) y = m_parent->m_height-1;

    this->setY(y);
    m_parent->setYBound(m_ID, m_parent->conv_Display2Point_y(y));
}

QRectF BoundCursorItem::boundingRect() const
{
    return QRectF(0, -7, 35, 14);
}

void BoundCursorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    int curY = m_parent->conv_Display2Point_y(this->y());
    QPointF points[5] = {
        QPointF(0, 0),
        QPointF(7, -7),
        QPointF(35, -7),
        QPointF(35, 7),
        QPointF(7, 7),
    };
    painter->setBrush(Qt::yellow);
    painter->drawPolygon(points, 5);
    painter->setFont(m_parent->m_font);
    painter->drawText(QRectF(6, -6, 29, 13), QString::number(curY));
}
