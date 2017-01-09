#include "serialdialog.h"
#include "ui_serialdialog.h"

#include "source/communicate/receivedata/receivedata.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QGridLayout>

SerialDialog::SerialDialog(QVariantMap param, QWidget *parent) :
    QDialog(parent), paramMap(param), ui(new Ui::SerialDialog)
{
    ui->setupUi(this);
    zhCom = NULL;
    isComOpen = false;
    zhReceiveData = NULL;

    initSetting();
    initScene();
    controlTools();
    setWindowTitle(QObject::tr("Sericom"));
}

SerialDialog::~SerialDialog()
{
    delete ui;
    delete zhReceiveData;
    zhReceiveData = NULL;
}

void SerialDialog::initSetting()
{
    if(!paramMap.contains("LocalHost")){
        paramMap.insert("LocalHost", 5);
    }
    if(!paramMap.contains("DownHost")){
        paramMap.insert("DownHost", 5);
    }
    if(!paramMap.contains("Com")){
        paramMap.insert("Com", 3);
    }
    if(!paramMap.contains("Paud")){
        paramMap.insert("Paud", 4);
    }
    if(!paramMap.contains("Data")){
        paramMap.insert("Data", 0);
    }
    if(!paramMap.contains("Stop")){
        paramMap.insert("Stop", 0);
    }
    if(!paramMap.contains("Check")){
        paramMap.insert("Check", 0);
    }

}

void SerialDialog::initScene()
{
    comLabel = new QLabel(this);
    comLabel->setText(QObject::tr("Com"));
    comComboBox = new QComboBox(this);
    QStringList str;
    str<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"
      <<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12";
    comComboBox->addItems(str);
    comComboBox->setCurrentIndex(paramMap["Com"].toInt());

    localHostLabel = new QLabel("LocalHost",this);
    localHostEdit = new QLineEdit(this);
    localHostEdit->setValidator(new QIntValidator(0, 0xFF, this));
    localHostEdit->setText(QString::number(paramMap["LocalHost"].toInt()));

    downHostLabel = new QLabel("DownHost",this);
    downHostEdit = new QLineEdit(this);
    downHostEdit->setValidator(new QIntValidator(0, 0xFF, this));
    downHostEdit->setText(QString::number(paramMap["DownHost"].toInt()));

    paudLabel = new QLabel("Paud",this);
    paudComboBox = new QComboBox(this);
    str.clear();
    str<<"9600"<<"19200"<<"38400"<<"57600"<<"115200";
    paudComboBox->addItems(str);
    paudComboBox->setCurrentIndex(paramMap["Paud"].toInt());

    dataLabel = new QLabel("Data",this);
    dataComboBox = new QComboBox(this);
    str.clear();
    str<<"8"<<"7";
    dataComboBox->addItems(str);
    dataComboBox->setCurrentIndex(paramMap["Data"].toInt());

    stopLabel = new QLabel("Stop",this);
    stopComboBox = new QComboBox(this);
    str.clear();
    str<<"1"<<"2";
    stopComboBox->addItems(str);
    stopComboBox->setCurrentIndex(paramMap["Stop"].toInt());

    checkLabel = new QLabel("Check",this);
    checkComboBox = new QComboBox(this);
    str.clear();
    str<<"NULL"<<"EVEN"<<"ODD";
    checkComboBox->addItems(str);
    checkComboBox->setCurrentIndex(paramMap["Check"].toInt());

    openSerialButton = new QPushButton(this);
    openSerialButton->setText(QObject::tr("OpenSerialCom"));
    closeSerialButton = new QPushButton(this);
    closeSerialButton->setText(QObject::tr("CloseSerialCom"));

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(localHostLabel, 0, 0);
    gridLayout->addWidget(localHostEdit, 0, 1);
    gridLayout->addWidget(downHostLabel, 1, 0);
    gridLayout->addWidget(downHostEdit, 1, 1);
    gridLayout->addWidget(comLabel,2,0);
    gridLayout->addWidget(comComboBox,2,1);
    gridLayout->addWidget(paudLabel,3,0);
    gridLayout->addWidget(paudComboBox,3,1);
    gridLayout->addWidget(dataLabel,4,0);
    gridLayout->addWidget(dataComboBox,4,1);
    gridLayout->addWidget(stopLabel,5,0);
    gridLayout->addWidget(stopComboBox,5,1);
    gridLayout->addWidget(checkLabel,6,0);
    gridLayout->addWidget(checkComboBox,6,1);
    gridLayout->addWidget(openSerialButton,7,0);
    gridLayout->addWidget(closeSerialButton,7,1);

    setLayout(gridLayout);

    groupSignals();
}

void SerialDialog::groupSignals()
{
    connect(openSerialButton,SIGNAL(clicked()),this,SLOT(openCom()),Qt::UniqueConnection);
    connect(closeSerialButton,SIGNAL(clicked()),this,SLOT(closeCom()),Qt::UniqueConnection);
    connect(this,SIGNAL(zhComIsOpen()),this,SLOT(hasOpen()),Qt::UniqueConnection);
    connect(this,SIGNAL(zhComIsClose()),this,SLOT(hasClose()),Qt::UniqueConnection);
    connect(this,SIGNAL(comIsOpenChanged()),this,SLOT(controlTools()),Qt::UniqueConnection);
    connect(this,SIGNAL(hasReadMsg()),this,SLOT(handleMessage()),Qt::UniqueConnection);
}

bool SerialDialog::isConnected()
{
    if(zhCom!=NULL)
    {
        return zhCom->isOpen();
    }
    return false;
}

void SerialDialog::sendMessage(QByteArray msg)
{
    zhCom->write(msg);
    zhCom->flush();
    emit hasSendIns(msg);
}

void SerialDialog::openCom()
{
    saveSetting();
    setzhPortSettings();

    localHost = localHostEdit->text().toInt();
    downHost = downHostEdit->text().toInt();
    if(NULL==zhCom)
    {
        zhCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);
    }
    zhCom->open(QIODevice::ReadWrite);
    zhCom->setBaudRate(zhPortSettings.BaudRate);
    zhCom->setDataBits(zhPortSettings.DataBits);
    zhCom->setParity(zhPortSettings.Parity);
    zhCom->setStopBits(zhPortSettings.StopBits);
    zhCom->setFlowControl(zhPortSettings.FlowControl);
    zhCom->setTimeout(zhPortSettings.Timeout_Millisec);
    if(zhCom->isOpen()){
        connect(zhCom,SIGNAL(readyRead()),this,SLOT(readMessage()),Qt::UniqueConnection);
        emit zhComIsOpen();
    }
    else
    {
        delete zhCom;
        zhCom = NULL;
        QMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("cannot open!"));
    }
}

void SerialDialog::closeCom()
{
    zhCom->close();
    if(!zhCom->isOpen())
    {
        delete zhCom;
        zhCom = NULL;
        emit zhComIsClose();
    }
}

void SerialDialog::setzhPortSettings()
{
    portName = comComboBox->currentText();

    if(paudComboBox->currentText()==QObject::tr("9600")){
        zhPortSettings.BaudRate = BAUD9600;
    }else if(paudComboBox->currentText()==QObject::tr("19200")){
        zhPortSettings.BaudRate = BAUD19200;
    }else if(paudComboBox->currentText()==QObject::tr("38400")){
        zhPortSettings.BaudRate = BAUD38400;
    }else if(paudComboBox->currentText()==QObject::tr("57600")){
        zhPortSettings.BaudRate = BAUD57600;
    }else if(paudComboBox->currentText()==QObject::tr("115200")){
        zhPortSettings.BaudRate = BAUD115200;
    }else{
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    if(dataComboBox->currentText()==QObject::tr("8")){
        zhPortSettings.DataBits = DATA_8;
    }else if(dataComboBox->currentText()==QObject::tr("7")){
        zhPortSettings.DataBits = DATA_7;
    }else{
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    if(stopComboBox->currentText()==QObject::tr("1")){
        zhPortSettings.StopBits = STOP_1;
    }else if(stopComboBox->currentText()==QObject::tr("2")){
        zhPortSettings.StopBits = STOP_2;
    }else{
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    if(checkComboBox->currentText()==QObject::tr("NULL")){
        zhPortSettings.Parity = PAR_NONE;
    }else if(checkComboBox->currentText()==QObject::tr("EVEN")){
        zhPortSettings.Parity = PAR_EVEN;
    }else if(checkComboBox->currentText()==QObject::tr("ODD")){
        zhPortSettings.Parity = PAR_ODD;
    }else{
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    zhPortSettings.FlowControl = FLOW_OFF;
    zhPortSettings.Timeout_Millisec = 10;
}

void SerialDialog::hasOpen()
{
    isComOpen = true;
    emit comIsOpenChanged();
}

void SerialDialog::hasClose()
{
    isComOpen = false;
    emit comIsOpenChanged();
}

void SerialDialog::controlTools()
{
    localHostEdit->setDisabled(isComOpen);
    downHostEdit->setDisabled(isComOpen);
    comComboBox->setDisabled(isComOpen);
    paudComboBox->setDisabled(isComOpen);
    dataComboBox->setDisabled(isComOpen);
    stopComboBox->setDisabled(isComOpen);
    checkComboBox->setDisabled(isComOpen);
    openSerialButton->setDisabled(isComOpen);
    closeSerialButton->setDisabled(!isComOpen);
}

int SerialDialog::getLocalHost()
{
    return localHostEdit->text().toInt();
}

int SerialDialog::getDownHost()
{
    return downHostEdit->text().toInt();
}

void SerialDialog::readMessage()
{
    readMsg.clear();
    readMsg = zhCom->readAll();
    if(!readMsg.isEmpty())
    {
        emit hasReadMsg();
    }
}

void SerialDialog::handleMessage()
{
    if(NULL==zhReceiveData)
    {
        zhReceiveData = new ReceiveData;
        zhReceiveData->setLocalHost(uchar(localHost));
        zhReceiveData->setDownHost(uchar(downHost));
        connect(zhReceiveData,SIGNAL(readFinished()),this,SLOT(getInsFromReceiveData()),Qt::UniqueConnection);
    }
    zhReceiveData->setParseData(readMsg);
}

void SerialDialog::getInsFromReceiveData()
{
    if(NULL==zhReceiveData)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("ReceiveData is NULL!"));
        return;
    }
    completeInsData.clear();
    completeInsData = zhReceiveData->getcomplateInsData();
    emit hasReplyIns(completeInsData);
}

void SerialDialog::saveSetting()
{
    paramMap.insert("LocalHost", localHostEdit->text().toInt());
    paramMap.insert("DownHost", downHostEdit->text().toInt());
    paramMap.insert("Com",comComboBox->currentIndex());
    paramMap.insert("Paud",paudComboBox->currentIndex());
    paramMap.insert("Data", dataComboBox->currentIndex());
    paramMap.insert("Stop", stopComboBox->currentIndex());
    paramMap.insert("Check", checkComboBox->currentIndex());
    emit saveParamSignal(paramMap);
}

void SerialDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
    emit closed();
}
