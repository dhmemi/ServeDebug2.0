#ifndef COMMANDANALYSIS_H
#define COMMANDANALYSIS_H

#include <QObject>
#include <vector>
#include <deque>
#include <QStringList>
#include <QVariantMap>

#include "source/globaldefine/mainStructure.h"
#include "source/globaldefine/upDefine.h"

using namespace std;

class MainWindow;
class SerialDialog;
class NetWorkDialog;
class QString;
class QStringList;
class QTimer;

class CommandAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit CommandAnalysis(QVariantMap param, QObject *parent = 0);
    ~CommandAnalysis();

    void getMainWindowPointer(MainWindow* p);
    void getNetWorkPointer(NetWorkDialog* p);
    void getSerialPointer(SerialDialog* p);
    void setParam(QVariantMap param);
    void groupWarning(QString str);

signals:
    void servoPosChangedSignal(ServoInfoMap info);
    void servoInfoChangedSignal(uchar type, ServoInfoMap info);
    void ActionListChangedSignal(QStringList data);
    void SequenceListChangedSignal(QStringList data);
    void getSequenceSignal(QString sequName, QStringList contans);

    void servoTypeChangedSignal(uchar type);
    void replyCmdChangedSignal(QString cmd);
    void sendCmdChangedSignal(QString cmd);
    void replyInsStateSignal(QByteArray replydata);
    void sendInsStateSignal(QByteArray senddata);
    void stateChangedSignals(QString str);
    void sendControlSignal();

public slots:
    void rcvOK(QByteArray data);
    void rcvLengthError(QByteArray data);
    void rcvCheckError(QByteArray data);
    void getDownServoConditionSolt();
    void getServoPositionSolt();
    void enableDigitalServoSolt();
    void disableDigitalServoSolt();
    void enableSomeServoSolt(QByteArray enableInfo);
    void uploadActionNameSlot();
    void uploadSequenceNameSlot();
    void uploadSequenceFileSlot(QString fileName);
    void downLoadMiniActionSlot(QString fileName);
    void execMiniActionSlot(QString fileName);
    void saveSequenceSlot(QString sequenceName, QStringList actionNames);
    void execSequenceSlot(QString fileName);
    void deleteActionSlot(QString fileName);
    void deleteSequenceSlot(QString fileName);
    void moveServoSolt(QVector<ServoInfo> servoPos);
    void renameActionSlot(QString oldName, QString newName);
    void renameSequenceSlot(QString oldName, QString newName);

    void outTimeSlot();
    void sendControlSlot();

    void setParseData(QByteArray data);

private:
    void initCmdDefine();
    quint8 protocalCheckData(QByteArray data);
    bool checkParityFromDown(QByteArray data);
    bool checkHostFromDown(QByteArray data);
    QByteArray makeCommonDate(uchar command);

    void sendInsData(QByteArray data);

    void res_OK(QByteArray data);
    void res_LengthError(QByteArray data);
    void res_CheckError(QByteArray data);
    void replyServoConnection(QByteArray data);
    void replyServoPosition(QByteArray data);
    void replyActionNames(QByteArray data);
    void replySequenceName(QByteArray data);
    void replySequence(QByteArray data);
    void replyGoodMsg(QByteArray data);
    void replyBadMsg(QByteArray data);

    MainWindow* zhMainWindow;
    SerialDialog* zhSerialDialog;
    NetWorkDialog* zhNetWorkDialog;

    deque<Ins> insDeque;
    QTimer* zhTimeControler;
    int sendTimes;
    quint8 localHost;
    quint8 downHost;

    int actionCount;
    QStringList actionList;
    int sequNameCount;
    QStringList sequNameList;
    int sequenceCount;
    QString sequenceName;
    QStringList sequenceContants;
    int servoCount;
    ServoInfoMap servoInfoMap;

    QMap<int, QString> cmdMap;

    int localCmdCount;
    int servoNumber;
    uchar servoType;
    QVariantMap paramMap;
};

#endif // COMMANDANALYSIS_H
