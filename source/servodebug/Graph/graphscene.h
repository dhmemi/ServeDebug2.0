#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>
#include <QMap>
#include "source/globaldefine/mainStructure.h"
#include "source/globaldefine/upDefine.h"

class GraphItem;
class KeyPointItem;

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
private:
    bool m_isCtrlDown;
    QMap<int, GraphItem *> itemMap;

    int m_timeBegin;
    int m_timeEnd;

    int m_itemHeight;

    bool isCtrlDown();

    // Right-click menu pop up when the cursor in the information
    int menu_ServoID;
    int menu_KeyFrameIndex;
    int menu_time;
    int menu_Position;

public:
    explicit GraphScene(QObject *parent = 0);
    ~GraphScene();

    void setItemHeight(int height);
    void setKeyFrameData(int servo, const PointList *points, QString name = "Servo");
    void setInterpFrameData(int servo, const PointList *points);
    void setYBounds(int servo, int bound1, int bound2);     // Set y feasible range
    void getYBounds(int servo, int &boundL, int &boundH);   // Read the feasible interval of y axis

    void rearrange();
    void selectKeyFrame(int index);

signals:
    void keyFrameSelected(int time);                        // The first index key frames is selected
    void keyFrameMove(int time, int newTime);               // The first moment of the index key frames are moved
    void keyFrameModify(int servo, int time, int newData);  // The first index key frames of the location data was changed
    void keyFrameDelete(int time);                          // Delete Keyframe
    void keyFrameGoto(int time);                            // Run to the key frames
    void keyFrameLoad(int time);                            // Save the key frames of data

    void insertKeyFrame(int time);                          //Insert keyframe

private slots:
    void act_insertKeyFrame();
    void act_keyFrameDelete();
    void act_keyFrameDelete2();
    void act_keyFrameGoto();
    void act_keyFrameLoad();

    void cb_viewExtentChange(int timeBegin, int timeEnd);
    void cb_keyFrameSelected(int time);                        // The first index key frames is selected
    void cb_keyFrameMove(int time, int newTime);               // The first moment of the index key frames are moved
    void cb_keyFrameModify(int servo, int time, int newData);  // The first index key frames of the location data was changed
    void cb_setShowCursor(bool show);

    friend class GraphItem;
    friend class KeyPointItem;
};

#endif // GRAPHSCENE_H
