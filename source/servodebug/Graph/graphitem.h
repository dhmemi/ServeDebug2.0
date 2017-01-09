#ifndef GRAPHITEM_H
#define GRAPHITEM_H

#include <QPointF>
#include <QString>
#include <QGraphicsItem>
#include <QList>
#include <QFont>
#include <QFontMetrics>
#include "source/globaldefine/mainStructure.h"
#include "source/globaldefine/upDefine.h"
#include <math.h>

class KeyPointItem;
class GraphScene;
class BubbleItem;
class BoundCursorItem;

class Label {
public:
    QPointF pos;
    QString str;
    Label(QPointF _pos, QString _str): pos(_pos), str(_str) {}
};

class GraphItem : public QGraphicsItem
{
private:
    int m_servoID;                  // servo Id

    QFont m_font;
    QFontMetrics m_fm;

    bool shouldShowBubble;
    BubbleItem *m_bubble;           // Tooltip
    QPainterPath axis;              // pbaspect
    QPainterPath mesh;              // coordinate grid
    QList<Label> meshLabels;        // Coordinates Of The Text
    QPainterPath interpCurve;       // SPL (spline)
    QPainterPath cursor;            // The two wires of the mouse cursor
    QPainterPath selectLine;        // Mark the selected key frames
    QPainterPath yBound;             // Y axis of the feasible region
    double cursorWidth;

    QList<KeyPointItem*> m_keyPointList;    // The Item list of key frames

    QString m_title;

    int m_width;
    int m_height;
    int timeBegin;
    int timeEnd;
    enum { m_maxPos = 1023} ;

    int m_yBounds[2];
    int m_yLow, m_yHigh;
    void setYBound(int ID, int bound);
    BoundCursorItem *boundItem[2];

    int m_xDiv;
    int m_yDiv;

    int m_frameSelected;

    void buildAxis();
    void buildMesh();
    void buildCursor(QPointF pos);
    void buildSelectLine();
    void buildInterpCurve();
    void buildKeyPoints();
    void resetKeyPointsPos();

    double timeFactor;
    double posFactor;
    void updateTimeFactor();
    void updatePosFactor();
    double  conv_Point2Display_x(int x);
    double  conv_Point2Display_y(int y);
    QPointF conv_Point2Display(int x, int y);
    QPointF conv_Point2Display(const Point &point);

    int   conv_Display2Point_x(double x);
    int   conv_Display2Point_y(double y);
    Point   conv_Display2Point(QPointF point);

    bool mouseOn;
    bool dragged;

    PointList keyList;              // The key frame data
    PointList interpList;           // The data of interpolation points

    int lookupKeyFrameTime(int index);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    // KeyPointItem callback
    void cb_keyFrame_selected(int index);                  // The first index key frames is selected
    void cb_keyFrame_move(int index, int dt);              // The first moment of the index key frames are moved
    void cb_keyFrame_modify(int index, int dy);            // The first index key frames of the location data was changed

public:
    GraphScene *pScene;                 // GraphScene pointer

    GraphItem(int servoID, GraphScene *scene);
    ~GraphItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    void setSize(int height, int width);                    // set Item size
    void setViewExtent(int _timeBegin, int _timeEnd);       // Set the Item shows the time range
    void setMesh(int xDivides, int yDivides);               // Set the fine fraction of grid
    void setTitle(const QString &title);                    // Set the title

    void setYBounds(int bound1, int bound2);                // Set y feasible range
    void getYBounds(int &boundL, int &boundH);              // Read the feasible interval of y axis

    void setDefaultExtent();

    void setKeyFrames(const PointList &list);               // Provide key frame data
    void setInterpFrames(const PointList &list);            // To provide the data of interpolation points

    void selectKeyFrame(int index);                         // Select one of the key frames
    void selectKeyFrameByTime(int time);                    // According to the selected one of the key frames

    friend class KeyPointItem;
    friend class GraphScene;
    friend class BoundCursorItem;
};

#endif // GRAPHITEM_H
