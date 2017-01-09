#ifndef ACTIONMANGER_H
#define ACTIONMANGER_H

#include <QObject>
#include <QVariantMap>

#include "source/globaldefine/mainStructure.h"
#include "source/servodebug/SplineSample/myspline.h"

class MySpline;
class GraphScene;
class QString;

class ActionManger : public QObject
{
    Q_OBJECT
public:
    explicit ActionManger(QVariantMap param, QTableWidget *parent = 0);
    ~ActionManger();
    void setParam(QVariantMap param);
    void setzhScenePointer(GraphScene* p);

public slots:
    void refreshKeyInfor(ACInfoMap keyInfo);
    //form odwn/CommandAnalysis
    void setServoInfo(uchar type, ServoInfoMap info);
    void setServoPos(ServoInfoMap info);
    //from ArmManage
    void saveFileSlot(QString abcsvName,QString abbinName);
    void saveKeyPoint(QString abfileName);
    void saveMiniAction(QString abfileName);
    //void downloadCsvSlot(QString fileName);
    void servoGoto(int time);

signals:

    void createSecneSignal();
    //to commandAnalysis
    void servoGotoSignal(QVector<ServoInfo> servopos);
    //to KeyFrameWidget

private:
    void refreshSlot();
    void refreshInterpInfor();
    void groupSignals();
    void groupWarning(QString str);

    MySpline spline;
    ACInfoMap zhACInfoMap;
    uchar servoType;

    QVariantMap paramMap;
    quint8 timeSpace;
    int intSpace;
};

#endif // ACTIONMANGER_H
