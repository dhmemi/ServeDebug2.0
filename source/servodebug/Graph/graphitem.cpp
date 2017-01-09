#include "graphitem.h"
#include "keypointitem.h"
#include "graphscene.h"
#include "bubbleitem.h"
#include "boundcursoritem.h"
#include "source/globaldefine/mainStructure.h"
#include "source/globaldefine/upDefine.h"
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>
#include <cmath>
#include <QMenu>
#include <math.h>
#include <cstdlib>
#include <QDebug>

GraphItem::GraphItem(int servoID, GraphScene *scene) :
    m_fm(m_font), m_servoID(servoID), pScene(scene),
    m_xDiv(10), m_yDiv(8)
{
    m_font = QFont("", 8);
    m_fm = QFontMetrics(m_font);

    setViewExtent(0, 1);

    mouseOn = false;
    setAcceptHoverEvents(true);

    shouldShowBubble = false;

    m_bubble = new BubbleItem();
    m_bubble->setParentItem(this);
    m_bubble->setVisible(false);

    boundItem[0] = new BoundCursorItem(this, 0);
    boundItem[1] = new BoundCursorItem(this, 1);
    boundItem[0]->setParentItem(this);
    boundItem[1]->setParentItem(this);

    setYBound(0, 0);
    setYBound(1, 1023);
}

GraphItem::~GraphItem()
{

}

void GraphItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    mouseOn = true;
    update();
}

void GraphItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    mouseOn = false;
    update();
}

void GraphItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    cursorWidth = 0.5;
    buildCursor(event->pos());
    update();
}

void GraphItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void GraphItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    cursorWidth = 1;
    buildCursor(event->pos());
    update();

    dragged = true;
    //if (event->button() == Qt::RightButton)
    {
        double inc = conv_Display2Point_x(event->lastPos().x())
                - conv_Display2Point_x(event->pos().x());
        pScene->cb_viewExtentChange(timeBegin+inc, timeEnd+inc);
    }
}

void GraphItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !dragged)
    {
        if (m_frameSelected == -1)
        {
            shouldShowBubble = !shouldShowBubble;
            pScene->cb_setShowCursor(shouldShowBubble);
        }
        else
        {
            cb_keyFrame_selected(-1);
        }
    }
    dragged = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

#include <QDebug>
void GraphItem::wheelEvent(QGraphicsSceneWheelEvent * event)
{
    const double const_rate = 0.1;

    if (QRectF(0, 0, m_width, m_height).contains(event->pos()))
    {
        double rate = (event->delta()>0)?const_rate:(-const_rate);
        int curTime = conv_Display2Point_x(event->pos().x());
        int newTimeBegin = timeBegin + (curTime-timeBegin)*rate;
        int newTimeEnd = timeEnd - (timeEnd-curTime)*rate;
        pScene->cb_viewExtentChange(newTimeBegin, newTimeEnd);
    }
}

void GraphItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    pScene->menu_time = conv_Display2Point_x(event->pos().x());

    QMenu menu;
    menu.addAction(QString(QObject::tr("Insert KeyFrame Here")), pScene, SLOT(act_insertKeyFrame()));
    QAction *act = menu.exec(event->screenPos());
}

void GraphItem::cb_keyFrame_selected(int index)
{
    pScene->cb_keyFrameSelected(lookupKeyFrameTime(index));
}

void GraphItem::cb_keyFrame_move(int index, int dt)
{
    int newTime = conv_Display2Point_x(m_keyPointList.at(index)->pos().x()+dt);

    QMetaObject::invokeMethod(pScene, "cb_keyFrameMove",
                              Qt::QueuedConnection,
                              Q_ARG(int, lookupKeyFrameTime(index)),
                              Q_ARG(int, newTime));
}

void GraphItem::cb_keyFrame_modify(int index, int dy)
{
    int newPos = conv_Display2Point_y(m_keyPointList.at(index)->pos().y()+dy);

    QMetaObject::invokeMethod(pScene, "cb_keyFrameModify",
                              Qt::QueuedConnection,
                              Q_ARG(int, m_servoID),
                              Q_ARG(int, lookupKeyFrameTime(index)),
                              Q_ARG(int, newPos));
}

QRectF GraphItem::boundingRect() const
{
    return QRectF(-40, -40, m_width+80, m_height+60);
}

void GraphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    painter->setPen(QPen(Qt::black, 0.5));
    painter->setBrush(QBrush(qRgb(180,255,180)));
    painter->drawPath(yBound);
    painter->setBrush(Qt::NoBrush);

    painter->setPen(QPen(Qt::black, 1));
    painter->drawPath(axis);

    painter->setPen(QPen(Qt::blue, 2));
    painter->drawPath(interpCurve);

    painter->setFont(QFont("", 12));
    painter->drawText(m_width/2 - m_fm.width(m_title)/2, 0-m_fm.height(), m_title);

    painter->setPen(QPen(Qt::black, cursorWidth));
    painter->drawPath(cursor);

    painter->setPen(QPen(Qt::black, 0.5, Qt::DotLine));
    painter->drawPath(mesh);

    painter->setPen(QPen(Qt::red, 1, Qt::SolidLine));
    painter->drawPath(selectLine);

    painter->setPen(QPen());
    painter->setFont(m_font);
    for (int i=0; i<meshLabels.size(); i++)
    {
        const Label &lbl = meshLabels.at(i);
        painter->drawText(lbl.pos, lbl.str);
    }

    if (mouseOn)
    {
        painter->setPen(QPen(Qt::red, 1.5, Qt::DashLine));
        painter->drawRect(this->boundingRect());
    }
}

void GraphItem::setSize(int height, int width)
{
    this->m_height = height;
    this->m_width = width;
    updateTimeFactor();
    updatePosFactor();

    buildAxis();
    buildMesh();
    buildInterpCurve();
    resetKeyPointsPos();
    buildSelectLine();
    setYBounds(m_yLow, m_yHigh);

    update();
}

void GraphItem::setViewExtent(int _timeBegin, int _timeEnd)
{
    if (_timeBegin+1000 >= _timeEnd)
    {
        int mid = (_timeBegin+_timeEnd)/2;
        _timeBegin = mid-500;
        _timeEnd = mid+500;
    }
    this->timeBegin = _timeBegin;
    this->timeEnd = _timeEnd;
    updateTimeFactor();
    buildMesh();
    buildInterpCurve();
    resetKeyPointsPos();
    buildSelectLine();

    update();
}

/************** two factor ***************/
void GraphItem::updateTimeFactor()
{
    timeFactor = (double)m_width / (timeEnd-timeBegin);
}

void GraphItem::updatePosFactor()
{
    posFactor = (double)m_height / 1023.0;
}

/***************** axis change ******************/
double  GraphItem::conv_Point2Display_x(int x)
{
    return timeFactor*(x-timeBegin);
}

double  GraphItem::conv_Point2Display_y(int y)
{
    return posFactor*(1023-y);
}

QPointF GraphItem::conv_Point2Display(int x, int y)
{
    return QPointF(conv_Point2Display_x(x), conv_Point2Display_y(y));
}
QPointF GraphItem::conv_Point2Display(const Point &point)
{
    return conv_Point2Display(point.x, point.y);
}

/**************** relate to axis change *****************/
int   GraphItem::conv_Display2Point_x(double x)
{
    return x/timeFactor + timeBegin;
}

int   GraphItem::conv_Display2Point_y(double y)
{
    return 1023 - y/posFactor;
}


Point   GraphItem::conv_Display2Point(QPointF point)
{
    Point pt;
    pt.x = conv_Display2Point_x(point.x());
    pt.y = conv_Display2Point_y(point.y());
    return pt;
}
/*******************************************/

void GraphItem::setMesh(int xDivides, int yDivides)
{
    m_xDiv = xDivides;
    m_yDiv = yDivides;
}

void GraphItem::setTitle(const QString &title)
{
    m_title = title;
}

void GraphItem::setKeyFrames(const PointList &list)
{
    keyList = list;
    buildKeyPoints();
    resetKeyPointsPos();
}

void GraphItem::setInterpFrames(const PointList &list)
{
    interpList = list;
    buildInterpCurve();
}

void GraphItem::selectKeyFrame(int index)
{
    for (int i=0; i<m_keyPointList.size(); i++)
    {
        if (i == index)
            m_keyPointList.at(i)->setSelected(true);
        else
            m_keyPointList.at(i)->setSelected(false);
    }
    m_frameSelected = index;
    buildSelectLine();
}

void GraphItem::selectKeyFrameByTime(int time)
{
    int i;
    for (i=0; i<keyList.size(); i++)
    {
        if (keyList.at(i).x == time) break;
    }
    if (i==keyList.size()) i=-1;
    selectKeyFrame(i);
}

void GraphItem::buildAxis()
{
    axis = QPainterPath();
    axis.addRect(0, 0, m_width, m_height);
    axis.moveTo(0, m_height/2);
    axis.lineTo(m_width, m_height/2);
}

void GraphItem::buildMesh()
{
    mesh = QPainterPath();
    meshLabels.clear();

    for (int i=0; i<=m_xDiv; i++)
    {
        double x = (double)m_width * i / m_xDiv;
        mesh.moveTo(x, 0);
        mesh.lineTo(x, m_height);

        QString lblStr = QString::number(conv_Display2Point_x(x),'f',1);
        meshLabels.append(Label(QPointF(x-m_fm.width(lblStr)/2, m_height+m_fm.height()), lblStr));
    }
    for (int i=0; i<=m_yDiv; i++)
    {
        double y = m_height - (double)m_height * i / m_yDiv;
        mesh.moveTo(0, y);
        mesh.lineTo(m_width, y);

        QString lblStr = QString::number((int)((double)m_maxPos*i/m_yDiv),'f',0);
        meshLabels.append(Label(QPointF(0-m_fm.width(lblStr)*1.2, y+m_fm.height()/3), lblStr));
    }
}

void GraphItem::buildCursor(QPointF pos)
{
    if (QRectF(0, 0, m_width, m_height).contains(pos))
    {
        cursor = QPainterPath();
        cursor.moveTo(0, pos.y());
        cursor.lineTo(m_width, pos.y());
        cursor.moveTo(pos.x(), 0);
        cursor.lineTo(pos.x(), m_height);
        m_bubble->setVisible(shouldShowBubble);
        m_bubble->setPos(pos);
        m_bubble->setStr(QString(QObject::tr(" Time =%1   \n Position =%2 "))
                         .arg(conv_Display2Point_x(pos.x()))
                         .arg(conv_Display2Point_y(pos.y())));
    }
    else
    {
        m_bubble->setVisible(false);
        cursor = QPainterPath();
    }
}

void GraphItem::buildSelectLine()
{
    selectLine = QPainterPath();

    if (m_frameSelected>=this->keyList.size() || m_frameSelected<0) return;

    double x = conv_Point2Display_x(this->keyList.at(m_frameSelected).x);
    if (x>=0 && x<m_width)
    {
        selectLine.moveTo(x, 0);
        selectLine.lineTo(x, m_height);
    }
}

void GraphItem::buildInterpCurve()
{
    interpCurve = QPainterPath();

    if (interpList.size() == 0) return;

    // [0, m_width)
    int s;
    for (s=0; s<interpList.size(); s++)
    {
        double xd = conv_Point2Display_x(interpList.at(s).x);
        if (xd>=0 && xd<m_width) break;
    }
    if (s==interpList.size()) return;   // no approciat point

    interpCurve.moveTo(conv_Point2Display(interpList.at(s).x, interpList.at(s).y));

    for (int i=s+1; i<interpList.size(); i++)
    {
        double xd = conv_Point2Display_x(interpList.at(i).x);
        if (!(xd>=0 && xd<m_width)) break;
        interpCurve.lineTo(conv_Point2Display(interpList.at(i).x, interpList.at(i).y));
    }
    update();
}

void GraphItem::buildKeyPoints()
{
    for (int i=0; i<m_keyPointList.size(); i++)
    {
        delete m_keyPointList.at(i);
    }
    m_keyPointList.clear();

    for (int i=0; i<keyList.size(); i++)
    {
        KeyPointItem *pItem = new KeyPointItem(this, i);
        pItem->setParentItem(this);
        m_keyPointList.append(pItem);
    }
}

void GraphItem::resetKeyPointsPos()
{
    for (int i=0; i<keyList.size(); i++)
    {
        KeyPointItem *pItem = m_keyPointList.at(i);
        QPointF pos = conv_Point2Display(keyList.at(i));
        pItem->setPos(pos);
        if (pos.x()<0 || pos.x()>=m_width)
            pItem->setVisible(false);
        else
            pItem->setVisible(true);
    }
}

int GraphItem::lookupKeyFrameTime(int index)
{
    if (index == -1)
        return -1;
    else
        return keyList.at(index).x;

}

template <typename T, typename T1, typename T2>
T min(const T1 &a, const T2 &b) {
    return (a>b)?b:a;
}

template <typename T, typename T1, typename T2>
T max(const T1 &a, const T2 &b) {
    return (a>b)?a:b;
}


void GraphItem::setDefaultExtent()
{
    int timeBegin = ~(1<<31);   // maxInt;
    int timeEnd = -timeBegin;   // minInt;
    for (int i=0; i<keyList.size(); i++)
    {
        timeBegin = min<int>(timeBegin, keyList.at(i).x);
        timeEnd = max<int>(timeEnd, keyList.at(i).x);
    }
    for (int i=0; i<interpList.size(); i++)
    {
        timeBegin = min<int>(timeBegin, interpList.at(i).x);
        timeEnd = max<int>(timeEnd, interpList.at(i).x);
    }
    int margin = (timeEnd-timeBegin)*0.05;

    setViewExtent(timeBegin-margin, timeEnd+margin);
}

void GraphItem::setYBound(int ID, int bound)
{
    if (bound < 0) bound = 0;
    if (bound > 1023) bound = 1023;
    m_yBounds[ID] = bound;
    m_yLow = min<int>(m_yBounds[0], m_yBounds[1]);
    m_yHigh = max<int>(m_yBounds[0], m_yBounds[1]);
    boundItem[ID]->setPos(m_width, conv_Point2Display_y(bound));
    boundItem[ID]->update();

    yBound = QPainterPath();
    yBound.addRect(0, conv_Point2Display_y(m_yHigh),
                   m_width, conv_Point2Display_y(m_yLow)-conv_Point2Display_y(m_yHigh));
    update();
}

void GraphItem::setYBounds(int bound1, int bound2)
{
    setYBound(0, bound1);
    setYBound(1, bound2);
}

void GraphItem::getYBounds(int &boundL, int &boundH)
{
    boundL = m_yLow;
    boundH = m_yHigh;
}
