#include "graphscene.h"
#include "graphitem.h"
#include "bubbleitem.h"
#include <QKeyEvent>
#include <QGraphicsView>
#include <QDebug>

GraphScene::GraphScene(QObject *parent) :
    QGraphicsScene(parent)
{
    m_itemHeight = 240;
}

GraphScene::~GraphScene()
{

}

template <typename T, typename T1, typename T2>
T min(const T1 &a, const T2 &b) {
    return (a>b)?b:a;
}

template <typename T, typename T1, typename T2>
T max(const T1 &a, const T2 &b) {
    return (a>b)?a:b;
}

void GraphScene::setKeyFrameData(int servo, const PointList *points, QString name)
{
    PointList tempList = (NULL == points)? PointList() : *points;
    if(name.size() == 0){
        name = "Servo";
    }
    GraphItem *pItem;
    if (!itemMap.contains(servo))
    {
        pItem = new GraphItem(servo, this);
        pItem->setTitle(QString(QObject::tr("%1th-")).arg(servo)+name);
        pItem->selectKeyFrame(-1);
        itemMap.insert(servo, pItem);
        this->addItem(pItem);
        rearrange();

        pItem->setKeyFrames(tempList);
        pItem->setDefaultExtent();
    }else{
        pItem = itemMap.value(servo);
        pItem->setKeyFrames(tempList);
    }
}

void GraphScene::setInterpFrameData(int servo, const PointList *points)
{
    PointList tempList = (NULL == points)? PointList() : *points;
    GraphItem *pItem;
    if (itemMap.contains(servo) == false)
    {
        pItem = new GraphItem(servo, this);
        pItem->setMesh(10, 8);
        pItem->setTitle(QString(QObject::tr("%1th-Servo")).arg(servo));
        pItem->selectKeyFrame(-1);
        itemMap.insert(servo, pItem);
        this->addItem(pItem);
        rearrange();

        pItem->setInterpFrames(tempList);
        pItem->setDefaultExtent();
    }else{
        pItem = itemMap.value(servo);
        pItem->setInterpFrames(tempList);
    }
}

bool GraphScene::isCtrlDown()
{
    return m_isCtrlDown;
}

void GraphScene::cb_keyFrameSelected(int index)                        // choose index
{
    QList<GraphItem*> items = itemMap.values();
    for (int i=0; i<items.size(); i++) items.at(i)->selectKeyFrame(index);
    emit keyFrameSelected(index);
}

void GraphScene::cb_keyFrameMove(int time, int newTime)               // move index
{
    emit keyFrameMove(time, newTime);
}

void GraphScene::cb_keyFrameModify(int servo, int time, int newData)  // change index data
{
    emit keyFrameModify(servo, time, newData);
}

void GraphScene::cb_viewExtentChange(int timeBegin, int timeEnd)
{
    QList<GraphItem*> items = itemMap.values();
    for (int i=0; i<items.size(); i++) items.at(i)->setViewExtent(timeBegin, timeEnd);
}

void GraphScene::cb_setShowCursor(bool show)
{
    QList<GraphItem*> items = itemMap.values();
    for (int i=0; i<items.size(); i++)
        items.at(i)->shouldShowBubble = show;
}

void GraphScene::setItemHeight(int height)
{
    m_itemHeight = height;
    rearrange();
}

void GraphScene::rearrange()
{
    QList<int> servoList = itemMap.keys();

    int itemWidth = this->views().at(0)->width();
    for (int i=0; i<servoList.size(); i++)
    {
        GraphItem *pItem = itemMap.value(servoList[i]);
        pItem->setSize(m_itemHeight, itemWidth);
        pItem->setPos(0, (m_itemHeight+120)*i+20);
    }
    this->setSceneRect(-40, -40, itemWidth+100, (m_itemHeight+140)*servoList.size());
    this->update();
}

void GraphScene::act_insertKeyFrame()
{
    emit insertKeyFrame(menu_time);
}

void GraphScene::act_keyFrameDelete()
{
    QMetaObject::invokeMethod(this, "act_keyFrameDelete2", Qt::QueuedConnection);
}

void GraphScene::act_keyFrameDelete2()
{
    emit keyFrameDelete(menu_time);
}

void GraphScene::act_keyFrameGoto()
{
    emit keyFrameGoto(menu_time);
}

void GraphScene::act_keyFrameLoad()
{
    emit keyFrameLoad(menu_time);
}

void GraphScene::setYBounds(int servo, int bound1, int bound2)
{
    GraphItem *pItem = itemMap.value(servo, NULL);
    if (pItem != NULL)
    {
        pItem->setYBounds(bound1, bound2);
    }
}

void GraphScene::getYBounds(int servo, int &boundL, int &boundH)
{
    GraphItem *pItem = itemMap.value(servo, NULL);
    if (pItem != NULL)
    {
        pItem->getYBounds(boundL, boundH);
    }
}
