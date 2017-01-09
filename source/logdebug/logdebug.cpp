#include "logdebug.h"
#include "ui_logdebug.h"

#include <QFile>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QString>
#include <QByteArray>
#include <QMessageBox>
#include <QWidget>
#include <QDebug>
#pragma execution_character_set("utf-8")

LogDebug::LogDebug(QVariantMap param, QWidget *parent) :
    QDockWidget(parent),paramMap(param),ui(new Ui::LogDebug)
{
    ui->setupUi(this);
    pTime = NULL;

    initScene();

    pTime = new QDateTime;
    file.setFileName("debug.log");
    if(file.open(QIODevice::ReadWrite)){
        recordTextEdit->setText(QString(file.readAll()));
    }else{
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("logdebugfile fail to open!"));
    }
    setWindowTitle(QObject::tr("调试日志"));
}

LogDebug::~LogDebug()
{
    delete ui;
}

void LogDebug::initScene()
{
    connectLabel = new QLabel("连接类型:",this);
    connectEdit = new QLineEdit(this);
    connectEdit->setEnabled(false);

    servoTypeLabel = new QLabel("舵机类型:",this);
    servoTypeEdit = new QLineEdit(this);
    servoTypeEdit->setEnabled(false);

    servoTableLabel = new QLabel("舵机列表:",this);
    servoTableEdit = new QLineEdit(this);
    servoTableEdit->setEnabled(false);

    lastSendInsLabel = new QLabel("最新发送:",this);
    lastSendInsEdit = new QLineEdit(this);
    lastSendInsEdit->setEnabled(false);

    lastReplyInsLabel = new QLabel("最新接收:",this);
    lastReplyInsEdit = new QLineEdit(this);
    lastReplyInsEdit->setEnabled(false);

    lastSendCmdLabel = new QLabel("最新发令:", this);
    lastSendCmdEdit = new QLineEdit(this);
    lastSendCmdEdit->setEnabled(false);

    lastReplyCmdLabel = new QLabel("最新收令:", this);
    lastReplyCmdEdit = new QLineEdit(this);
    lastReplyCmdEdit->setEnabled(false);

    clearButton = new QPushButton("清空记录", this);
    recordTextEdit = new QTextEdit(this);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(connectLabel, 0, 0);
    gridLayout->addWidget(connectEdit, 0, 1);
    gridLayout->addWidget(servoTypeLabel, 0, 2);
    gridLayout->addWidget(servoTypeEdit, 0, 3);
    gridLayout->addWidget(servoTableLabel, 1, 0);
    gridLayout->addWidget(servoTableEdit, 1, 1, 1 , 3);
    gridLayout->addWidget(lastSendInsLabel, 2, 0);
    gridLayout->addWidget(lastSendInsEdit, 2, 1, 1, 3);
    gridLayout->addWidget(lastReplyInsLabel, 3, 0);
    gridLayout->addWidget(lastReplyInsEdit, 3, 1, 1, 3);
    gridLayout->addWidget(lastSendCmdLabel, 4, 0);
    gridLayout->addWidget(lastSendCmdEdit, 4, 1);
    gridLayout->addWidget(lastReplyCmdLabel, 4, 2);
    gridLayout->addWidget(lastReplyCmdEdit, 4, 3);
    gridLayout->addWidget(clearButton, 5, 0, 1, 4);
    gridLayout->addWidget(recordTextEdit, 6, 0, 1, 4);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(gridLayout);
    setWidget(mainWidget);

    connect(clearButton,&QPushButton::clicked,this,&LogDebug::clearButtonSlot);
}

void LogDebug::setParam(QVariantMap param)
{
    paramMap = param;
}

void LogDebug::setconnectState(QString str)
{
    connectEdit->setText(str);
}

void LogDebug::setservoType(uchar type)
{
    if(type == 0x01){
        servoTypeEdit->setText("Digital_UBT12HB");
    }else if(type == 0x02){
        servoTypeEdit->setText("Simulate_LD1602");
    }else{
        //TODO
    }
}

void LogDebug::setservoTable(uchar type, ServoInfoMap info)
{
    setservoType(type);

    QString str;
    for(int i : info.keys()){
        str += QString::number(i, 10);
        str += ",";
    }
    servoTableEdit->setText(str);
}

void LogDebug::setSendInsSlot(QByteArray ins)
{
    QString msg;
    quint8 it = 0;
    if(ins.size() <= 0)
        return ;
    for(int i=0; i<(int)ins.size(); i++)
    {
        it = (quint8)ins.at(i);
        msg += QString::number(it, paramMap["logFormat"].toInt());
        msg += tr(" ");
    }
    if(msg.isEmpty())
        return ;
    lastSendInsEdit->setText(msg);
    setSendMsg(msg);
}

void LogDebug::setSendInsSlot(QString ins)
{
    lastSendInsEdit->setText(ins);
}

void LogDebug::setSendInsSlot(vector<unsigned char> ins)
{
    QString msg;
    quint8 it = 0;
    if(ins.size() <= 0)
    {
        return ;
    }
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        msg += QString::number(it, paramMap["logFormat"].toInt());
        msg += tr(" ");
    }
    if(msg.isEmpty())
    {
        return ;
    }
    lastSendInsEdit->setText(msg);
}

void LogDebug::setSendMsg(QString msg)
{
    if(recordTextEdit->toPlainText().size() > 5000){
        file.flush();
        recordTextEdit->clear();
    }
    QString test = pTime->currentDateTime().toString(Qt::TextDate);
    test.push_front("["), test.push_back("][Send]:\n"+msg+"\n");
    recordTextEdit->insertPlainText(test);
    file.write(test.toLatin1().data(),test.length());
}

void LogDebug::setReplyInsSlot(QString ins)
{
    lastReplyInsEdit->setText(ins);
}

void LogDebug::setReplyInsSlot(QByteArray ins)
{
    QString data;
    quint8 it = 0;
    if(ins.size() <= 0)
    {
        return ;
    }
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        data += QString::number(it, paramMap["logFormat"].toInt());
        data += tr(" ");
    }
    if(data.isEmpty())
    {
        return ;
    }
    lastReplyInsEdit->setText(data);
    setReplyMsg(data);
}

void LogDebug::setReplyInsSlot(vector<unsigned char> ins)
{
    QString data;
    quint8 it = 0;
    if(ins.size() <= 0)
    {
        return ;
    }
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        data += QString::number(it, paramMap["logFormat"].toInt());
        data += tr(" ");
    }
    if(data.isEmpty())
    {
        return ;
    }
    lastReplyInsEdit->setText(data);
}

void LogDebug::setReplyMsg(QString msg)
{
    if(recordTextEdit->toPlainText().size() > 5000){
        file.flush();
        recordTextEdit->clear();
    }
    QString test = pTime->currentDateTime().toString(Qt::TextDate);
    test.push_front("[");
    test += "][Reply]:\n"+msg+"\n";
    recordTextEdit->insertPlainText(test);
    file.write(test.toLatin1().data(),test.length());
}

void LogDebug::setSendCmdSlot(QString cmd)
{
    lastSendCmdEdit->setText(cmd);
}

void LogDebug::setReplyCmdSlot(QString cmd)
{
    lastReplyCmdEdit->setText(cmd);
}

void LogDebug::clearButtonSlot()
{
    file.resize(0);
    recordTextEdit->setText("");
}

void LogDebug::setReplyInsStateSlot(QString str)
{
    //
}

void LogDebug::closeEvent(QCloseEvent *event)
{
    event->ignore();
    emit closed();
}
