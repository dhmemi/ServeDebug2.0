#include "keypointitem.h"
#include "graphitem.h"
#include "graphscene.h"
#include "bubbleitem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <cmath>
#include <QMenu>
#include <QDebug>

KeyPointItem::KeyPointItem(GraphItem *parent, int _frameIndex)
{

    this->frameIndex = _frameIndex;
    this->pParentItem = parent;
    setAcceptHoverEvents(true);
    mouseOn = false;
    selected = false;

    m_bubble = new BubbleItem();
    m_bubble->setParentItem(this);
    m_bubble->setPos(0, 0);
    m_bubble->setVisible(false);
}

KeyPointItem::~KeyPointItem()
{

}

void KeyPointItem::updateBubble()
{
    m_bubble->setStr(QString(QObject::tr(" Time=%1   \n  Position=%2  "))
                     .arg(pParentItem->conv_Display2Point_x(pos().x())+dragPoint.x())
                     .arg(pParentItem->conv_Display2Point_y(pos().y())+dragPoint.y()));
    m_bubble->setPos(dragPoint);
}

QVariant KeyPointItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    updateBubble();
    return QGraphicsItem::itemChange(change, value);
}

void KeyPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    event;
    this->mouseOn = true;
    update();
}

void KeyPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    event;
    this->mouseOn = false;
    update();
}

void KeyPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->setSelected(true);
    pParentItem->cb_keyFrame_selected(this->frameIndex);
    dragged = false;

    if (event->modifiers() & Qt::ControlModifier)
        dragMode = MOVE;
    else
        dragMode = MODIFY;
}

void KeyPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    dragged = true;
    if (dragMode == MOVE)
    {
        dragPoint = QPoint(event->pos().x(), 0);
    }
    if (dragMode == MODIFY)
    {
        int posY = pParentItem->conv_Display2Point_y(event->pos().y()+pos().y());
        if (posY > pParentItem->m_yHigh) posY = pParentItem->m_yHigh;
        if (posY < pParentItem->m_yLow) posY = pParentItem->m_yLow;

        dragPoint = QPoint(0, pParentItem->conv_Point2Display_y(posY)-pos().y());
    }

    updateBubble();

    pParentItem->update();
    update();
}

void KeyPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!dragged)
        m_bubble->setVisible(!m_bubble->isVisible());
    else
    {
        if (dragMode == MOVE)
        {
            pParentItem->cb_keyFrame_move(frameIndex, dragPoint.x());
        }
        else if (dragMode == MODIFY)
        {
            pParentItem->cb_keyFrame_modify(frameIndex, dragPoint.y());
        }
    }
    dragMode = NONE;
    dragPoint = QPoint(0, 0);
    dragged = false;
    update();
    pParentItem->pScene->update();

    updateBubble();

}

void KeyPointItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    GraphScene *pScene = pParentItem->pScene;
    pScene->menu_time = pParentItem->lookupKeyFrameTime(frameIndex);
    pScene->menu_KeyFrameIndex = this->frameIndex;
    pScene->menu_Position = pParentItem->conv_Display2Point_y(this->pos().y());
    pScene->menu_ServoID = pParentItem->m_servoID;

    QMenu menu;
    menu.addAction(QString(QObject::tr("Servo goto")), pScene, SLOT(act_keyFrameGoto()));
    menu.addAction(QString(QObject::tr("Refrash")), pScene, SLOT(act_keyFrameLoad()));
    menu.addSeparator();
    menu.addAction(QString(QObject::tr("Delete")), pScene, SLOT(act_keyFrameDelete()));
    QAction *act = menu.exec(event->screenPos());
}

QRectF KeyPointItem::boundingRect() const
{
    QPointF p1;
    QSizeF size;
    p1 = QPointF(-R-3 + dragPoint.x()-2, -R-3 + dragPoint.y()-2);
    size = QSizeF(2*(R+3) + abs(dragPoint.x()) + 4, 2*(R+3)+ abs(dragPoint.y()) + 4);
    return QRectF(p1, size);
}

void KeyPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    painter->translate(dragPoint);
    if (mouseOn | selected)
    {
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->drawRect(-(R+3), -(R+3), 2*(R+3), 2*(R+3));
    }

    if (this->selected)
    {
        painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
        painter->drawEllipse(QPoint(0, 0), R, R);
    }
    else
    {
        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
        painter->drawEllipse(QPoint(0, 0), R, R);
    }
}

void KeyPointItem::setFrameIndex(int index)
{
    this->frameIndex = index;
}

void KeyPointItem::setSelected(bool sel)
{
    this->selected = sel;
    if (!sel) m_bubble->setVisible(false);
    update();
}
