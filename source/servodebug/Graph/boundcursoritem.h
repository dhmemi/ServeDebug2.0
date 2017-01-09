#ifndef BOUNDCURSORITEM_H
#define BOUNDCURSORITEM_H

#include <QGraphicsItem>
#include "graphitem.h"

class BoundCursorItem : public QGraphicsItem
{
private:
    int m_ID;
    GraphItem *m_parent;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

public:
    explicit BoundCursorItem(GraphItem *parent, int ID);
    virtual ~BoundCursorItem();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);
};

#endif // BOUNDCURSORITEM_H
