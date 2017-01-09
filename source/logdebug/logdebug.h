#ifndef LOGDEBUG_H
#define LOGDEBUG_H

#include "../globaldefine/mainStructure.h"
#include <QDockWidget>
#include <QCloseEvent>
#include <QVariantMap>
#include <vector>
#include <QFile>

using namespace std;

class QFile;
class QDateTime;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QString;
class QByteArray;
class QWidget;

namespace Ui {
class LogDebug;
}

class LogDebug : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit LogDebug(QVariantMap param, QWidget *parent = 0);
    ~LogDebug();
    void setParam(QVariantMap param);

signals:
    void closed();

public slots:
    void setconnectState(QString state);
    void setservoType(uchar type);
    void setservoTable(uchar type, ServoInfoMap info);

    void setSendInsSlot(QByteArray ins);
    void setSendInsSlot(QString ins);
    void setSendInsSlot(vector<unsigned char> ins);
    void setSendMsg(QString msg);

    void setReplyInsSlot(QString ins);
    void setReplyInsSlot(QByteArray ins);
    void setReplyInsSlot(vector<unsigned char> ins);
    void setReplyMsg(QString msg);

    void setSendCmdSlot(QString cmd);
    void setReplyCmdSlot(QString cmd);
    void setReplyInsStateSlot(QString str);
    void clearButtonSlot();

private:
    void closeEvent(QCloseEvent *event);

    void initScene();

private:
    Ui::LogDebug *ui;
    QFile file;
    QDateTime* pTime;

    //scene tools--ui
    QLabel* connectLabel;
    QLineEdit* connectEdit;
    QLabel* servoTypeLabel;
    QLineEdit* servoTypeEdit;
    QLabel* servoTableLabel;
    QLineEdit* servoTableEdit;
    QLabel* lastSendCmdLabel;
    QLineEdit* lastSendCmdEdit;
    QLabel* lastReplyCmdLabel;
    QLineEdit* lastReplyCmdEdit;
    QLabel* lastSendInsLabel;
    QLineEdit* lastSendInsEdit;
    QLabel* lastReplyInsLabel;
    QLineEdit* lastReplyInsEdit;
    QPushButton* clearButton;
    QTextEdit* recordTextEdit;

    QWidget *mainWidget;

    QVariantMap paramMap;
};

#endif // LOGDEBUG_H
