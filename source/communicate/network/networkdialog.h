#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QVariantMap>

class QTcpSocket;
class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QString;
class QByteArray;
class ReceiveData;

namespace Ui {
class NetWorkDialog;
}

class NetWorkDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NetWorkDialog(QVariantMap param, QWidget *parent = 0);
    ~NetWorkDialog();

    void sendMessage(QByteArray msg);
    bool isConnected();
    bool isConnect();
    int getLocalHost();
    int getDownHost();
    
private:
    void initSetting();
    void saveSetting();
    void initScene();
    void groupSignals();

    void closeEvent(QCloseEvent *event);

private slots:
    void connectButtonSlot();
    void disconnectButtonSlot();
    void hasconnect();
    void hasdisconnect();

    //enable tools
    void controlTools();

    void readMessage();
    void handlereadMsg();

    void getInsFromReceiveData(); //get complete Ins data

signals:
    void isconnectChanged();
    void hasReadMsg();
    void closed();

    void saveParamSignal(QVariantMap paramMap);

    void getConnected();
    void getDisConnected();
    void hasSendIns(QByteArray ins);
    void hasReplyIns(QByteArray ins);

    //out data
public:
    QByteArray completeInsData;

private:
    Ui::NetWorkDialog *ui;
    QTcpSocket* zhSocket;

    //scene tools--ui
    QLabel* localHostLabel;
    QLineEdit* localHostEdit;
    QLabel* downHostLabel;
    QLineEdit* downHostEdit;
    QLabel* ipLabel;
    QLineEdit* ipLineEdit;
    QLabel* portLabel;
    QLineEdit* portLineEdit;
    QPushButton* connectButton;
    QPushButton* disconnectButton;

    //socket information
    QString IpString;
    QString portString;
    int localHost;
    int downHost;
    QVariantMap paramMap;

    //control tools state
    bool isconnect;

    //read message
    QByteArray readMsg;

    //connect out
    ReceiveData* zhReceiveData;
};

#endif // NETWORKDIALOG_H
