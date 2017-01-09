#include "networkdialog.h"
#include "ui_networkdialog.h"

#include "source/communicate/receivedata/receivedata.h"

#include <QTcpSocket>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QByteArray>

NetWorkDialog::NetWorkDialog(QVariantMap param, QWidget *parent) :
    QDialog(parent), paramMap(param), ui(new Ui::NetWorkDialog)
{
    ui->setupUi(this);
    zhSocket = NULL;
    isconnect = false;
    zhReceiveData = NULL;

    initSetting();
    initScene();
    controlTools();
    bool ok = false;
    connectButton->setDefault(&ok);
    setWindowTitle("网络");
}

NetWorkDialog::~NetWorkDialog()
{
    delete ui;
    delete zhReceiveData;
    zhReceiveData = NULL;
}

void NetWorkDialog::initSetting()
{
    if(!paramMap.contains("LocalHost")){
        paramMap.insert("LocalHost", 5);
    }
    if(!paramMap.contains("DownHost")){
        paramMap.insert("DownHost", 5);
    }
    if(!paramMap.contains("IpAdress")){
        paramMap.insert("IpAdress", "127.0.0.1");
    }
    if(!paramMap.contains("Port")){
        paramMap.insert("Port", "8080");
    }
}

void NetWorkDialog::initScene()
{
    setWindowTitle(QObject::tr("Socket"));

    localHostLabel = new QLabel("LocalHost",this);
    localHostEdit = new QLineEdit(this);
    localHostEdit->setValidator(new QIntValidator(0, 0xff, this));
    localHostEdit->setText(QString::number(paramMap["LocalHost"].toInt()));

    downHostLabel = new QLabel("DownHost",this);
    downHostEdit = new QLineEdit(this);
    downHostEdit->setValidator(new QIntValidator(0, 0xff, this));
    downHostEdit->setText(QString::number(paramMap["DownHost"].toInt()));

    ipLabel = new QLabel("IpAdress",this);
    ipLineEdit = new QLineEdit(this);
    ipLineEdit->setText(paramMap["IpAdress"].toString());


    portLabel = new QLabel("Port",this);
    portLineEdit = new QLineEdit(this);
    portLineEdit->setText(paramMap["Port"].toString());

    connectButton = new QPushButton(this);
    disconnectButton = new QPushButton(this);
    connectButton->setText(QObject::tr("Connect"));
    disconnectButton->setText(QObject::tr("DisConnect"));

    QGridLayout* vLayout = new QGridLayout;
    vLayout->addWidget(localHostLabel, 0, 0);
    vLayout->addWidget(localHostEdit, 0, 1);
    vLayout->addWidget(downHostLabel, 1, 0);
    vLayout->addWidget(downHostEdit, 1, 1);
    vLayout->addWidget(ipLabel, 2, 0);
    vLayout->addWidget(ipLineEdit, 2, 1);
    vLayout->addWidget(portLabel, 3, 0);
    vLayout->addWidget(portLineEdit, 3, 1);
    vLayout->addWidget(connectButton, 4, 0);
    vLayout->addWidget(disconnectButton, 4, 1);

    setLayout(vLayout);
    groupSignals();
}

void NetWorkDialog::groupSignals()
{
    connect(connectButton,SIGNAL(clicked()),this,SLOT(connectButtonSlot()));
    connect(disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectButtonSlot()));
    connect(this,SIGNAL(hasReadMsg()),this,SLOT(handlereadMsg()));
}

int NetWorkDialog::getLocalHost()
{
    return localHostEdit->text().toInt();
}

int NetWorkDialog::getDownHost()
{
    return downHostEdit->text().toInt();
}

void NetWorkDialog::connectButtonSlot()
{
    IpString = ipLineEdit->text();
    portString = portLineEdit->text();

    localHost = localHostEdit->text().toInt();
    downHost = downHostEdit->text().toInt();

    bool ok = false;
    quint16 port = portString.toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("The Port is not correct!"));
        return;
    }
    if(!IpString.isEmpty()&&!portString.isEmpty())
    {
        if(NULL==zhSocket)
        {
            zhSocket = new QTcpSocket;
            connect(zhSocket,SIGNAL(disconnected()),this,SLOT(hasdisconnect()));
            connect(zhSocket,SIGNAL(connected()),this,SLOT(hasconnect()));
            connect(this,SIGNAL(isconnectChanged()),this,SLOT(controlTools()));
            connect(zhSocket,SIGNAL(readyRead()),this,SLOT(readMessage()));
        }
        zhSocket->connectToHost(IpString,port);
        if(!zhSocket->isOpen())
        {
            QMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("cannot connect!"));
        }
    }
}

void NetWorkDialog::disconnectButtonSlot()
{
    zhSocket->disconnectFromHost();
}

void NetWorkDialog::controlTools()
{
    connectButton->setDisabled(isconnect);
    disconnectButton->setDisabled(!isconnect);
    localHostEdit->setDisabled(isconnect);
    downHostEdit->setDisabled(isconnect);
    ipLineEdit->setDisabled(isconnect);
    portLineEdit->setDisabled(isconnect);
}

void NetWorkDialog::hasconnect()
{
    isconnect = true;
    emit getConnected();
    emit isconnectChanged();
}

void NetWorkDialog::hasdisconnect()
{
    isconnect = false;
    emit getDisConnected();
    emit isconnectChanged();
}

void NetWorkDialog::sendMessage(QByteArray msg)
{
    zhSocket->write(msg);
    zhSocket->flush();
    emit hasSendIns(msg);
}

void NetWorkDialog::readMessage()
{
    readMsg.clear();
    readMsg = zhSocket->readAll();

    if(!readMsg.isEmpty())
    {
        emit hasReadMsg();
    }
}

bool NetWorkDialog::isConnected()
{
    if(zhSocket!=NULL)
    {
        return zhSocket->isOpen();
    }
    return false;
}

bool NetWorkDialog::isConnect()
{
    return isconnect;
}

void NetWorkDialog::handlereadMsg()
{
    if(NULL==zhReceiveData)
    {
        zhReceiveData = new ReceiveData;
        zhReceiveData->setLocalHost(localHost);
        zhReceiveData->setDownHost(downHost);
        connect(zhReceiveData,SIGNAL(readFinished()),this,SLOT(getInsFromReceiveData()));
    }
    zhReceiveData->setParseData(readMsg);
}

void NetWorkDialog::getInsFromReceiveData()
{
    if(NULL==zhReceiveData)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("ReceiveData is NULL!"));
        return;
    }
    completeInsData.clear();
    completeInsData = zhReceiveData->getcomplateInsData();
    emit hasReplyIns(completeInsData);
}

void NetWorkDialog::saveSetting()
{
    paramMap.insert("LocalHost", localHostEdit->text().toInt());
    paramMap.insert("DownHost", downHostEdit->text().toInt());
    paramMap.insert("IpAdress",ipLineEdit->text());
    paramMap.insert("Port",portLineEdit->text());
    emit saveParamSignal(paramMap);
}

void NetWorkDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
    emit closed();
}
