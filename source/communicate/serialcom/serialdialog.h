#ifndef SERIALDIALOG_H
#define SERIALDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QVariantMap>

#include "source/communicate/serialcom/qextserialbase.h"
#include "source/communicate/serialcom/qextserialport.h"
#include "source/communicate/serialcom/win_qextserialport.h"

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QString;
class QByteArray;

class ReceiveData;

namespace Ui {
class SerialDialog;
}

class SerialDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SerialDialog(QVariantMap param, QWidget *parent = 0);
    ~SerialDialog();

    void sendMessage(QByteArray msg);
    bool isConnected();
    int getLocalHost();
    int getDownHost();

private:
    void initScene();
    void initSetting();
    void saveSetting();
    void groupSignals();

    void setzhPortSettings();

    void closeEvent(QCloseEvent *event);

signals:
    void zhComIsOpen();
    void zhComIsClose();

    void saveParamSignal(QVariantMap paramMap);

    void comIsOpenChanged();
    void hasReadMsg();

    void hasSendIns(QByteArray ins);
    void hasReplyIns(QByteArray ins);
    void closed();

private slots:
    void openCom();
    void closeCom();

    void hasOpen();
    void hasClose();

    //enable tools
    void controlTools();

    void readMessage();
    void handleMessage();

    void getInsFromReceiveData(); //get complete ins data

    //out Data
public:
    QByteArray completeInsData;
    
private:
    Ui::SerialDialog *ui;

    //scene tools--ui
    QLabel* localHostLabel;
    QLineEdit* localHostEdit;
    QLabel* downHostLabel;
    QLineEdit* downHostEdit;
    QLabel* comLabel;
    QComboBox* comComboBox;
    QLabel* paudLabel;
    QComboBox* paudComboBox;
    QLabel* dataLabel;
    QComboBox* dataComboBox;
    QLabel* stopLabel;
    QComboBox* stopComboBox;
    QLabel* checkLabel;
    QComboBox* checkComboBox;

    QPushButton* openSerialButton;
    QPushButton* closeSerialButton;

    //serialCom settings
    Win_QextSerialPort* zhCom;
    PortSettings zhPortSettings;
    QString portName;
    int localHost;
    int downHost;
    QVariantMap paramMap;

    //control tools state
    bool isComOpen;

    //read message
    QByteArray readMsg;

    //connect to out
    ReceiveData* zhReceiveData;
};

#endif // SERIALDIALOG_H
