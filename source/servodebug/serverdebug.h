#ifndef SERVERDEBUG_H
#define SERVERDEBUG_H

#include "source/servodebug/SplineSample/myspline.h"
#include "source/globaldefine/mainStructure.h"
#include "source/globaldefine/upDefine.h"

#include <QDockWidget>
#include <vector>
#include <QCloseEvent>
#include <QVariantMap>

using namespace std;

class QGraphicsView;
class QPushButton;
class GraphScene;
class MySpline;
class QWidget;

namespace Ui {
class ServerDebug;
}

class ServerDebug : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ServerDebug(QVariantMap param, QWidget *parent = 0);
    ~ServerDebug();
    void setParam(QVariantMap param);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

public slots:
    void refreshSlot(ACInfoMap info);
    void setServoTypeSlot(uchar type);
    void servoGoto(int time);
    void saveFileSlot(QString abcsvName,QString abbinName);
    void saveKeyPoint(QString abfileName);
    void saveMiniAction(QString abfileName);

private slots:
    void refreshInterpInfor();
    void controlTools();
    void requestButtonSlot();
    void enableAllButtonSlot();
    void enableSomeButtonSlot();

signals:
    void requestInformation();
    void enableAllInformation();
    void disenableAllInformation();
    void enableSomeInfomation();
    void servoGotoSignal(QVector<ServoInfo> info);
    void zhSceneCreated(GraphScene* p);
    void closed();
    
private:
    void initScene();
    void initGraphScene();
    void groupSignals();
    void controlButtonState();
    void groupWarning(QString str);

    MySpline spline;
    ACInfoMap zhACInfoMap;
    uchar servoType;
    QVariantMap paramMap;
    bool toolsState;

    quint8 timeSpace;
    int intSpace;

    //scene tools--ui
    QGraphicsView* graphicsView;
    GraphScene* zhScene;
    QPushButton* requestButton;
    QPushButton* enableAllButton;
    QPushButton* enableSomeButton;
    QWidget *mainWidget;

    Ui::ServerDebug *ui;
};

#endif // SERVERDEBUG_H
