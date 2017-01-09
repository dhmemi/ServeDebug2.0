#ifndef KEYPOINTITEM_H
#define KEYPOINTITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPoint>
#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

class GraphItem;
class BubbleItem;

class KeyPointItem : public QGraphicsItem
{
private:
    enum { R = 4 };

    GraphItem *pParentItem;
    int frameIndex;
    bool selected;
    bool mouseOn;

    bool dragged;
    enum {NONE, MOVE, MODIFY} dragMode;
    QPoint dragPoint;

    BubbleItem *m_bubble;
    void updateBubble();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    explicit KeyPointItem(GraphItem *parent, int _frameIndex);
    virtual ~KeyPointItem();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    void setFrameIndex(int index);
    void setSelected(bool sel);
};

#endif // KEYPOINTITEM_H
