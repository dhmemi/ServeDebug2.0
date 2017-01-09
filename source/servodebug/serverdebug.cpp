#include "serverdebug.h"
#include "ui_serverdebug.h"

#include "source/servodebug/Graph/graphscene.h"

#include <QGraphicsView>
#include <QPushButton>
#include <QGridLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QWidget>
#include <QVariantMap>
#include <QTextStream>
#include <QDebug>

ServerDebug::ServerDebug(QVariantMap param, QWidget *parent) :
    QDockWidget(parent), paramMap(param), ui(new Ui::ServerDebug)
{
    ui->setupUi(this);
    toolsState = false;
    zhScene = NULL;
    intSpace =  1;
    servoType = Digital_UBT12HB;
    if(paramMap["isInterpolateTimeSet"].toBool()){
        timeSpace = paramMap["interpolateTime"].toInt();
    }else{
        timeSpace = DEFAULT_INTERPPLATE_TIME;
    }
    initScene();
    controlTools();
    setWindowTitle(QObject::tr("数据预览"));
}

ServerDebug::~ServerDebug()
{
    delete ui;
}

void ServerDebug::setParam(QVariantMap param)
{
    paramMap = param;
    if(paramMap["isInterpolateTimeSet"].toBool()){
        timeSpace = paramMap["interpolateTime"].toInt();
    }
}

void ServerDebug::refreshSlot(ACInfoMap info)
{
    if(zhACInfoMap.size() != info.size()){
         initGraphScene();
    }
    zhACInfoMap.clear();
    zhACInfoMap = info;
    refreshInterpInfor();

    for(int i : zhACInfoMap.keys())
    {
        //zhScene->setItemName(i, zhACInfoMap[i].name);
        zhScene->setKeyFrameData(i,&(zhACInfoMap[i].keyPoint),zhACInfoMap[i].name);   //keyFrame
        if(zhACInfoMap[i].keyPoint.size()>=3)
        {
            zhScene->setInterpFrameData(zhACInfoMap[i].id,&(zhACInfoMap[i].InterPoint));//interp data
        }
        else
        {
            PointList tempPoint;
            tempPoint.clear();
            zhScene->setInterpFrameData(zhACInfoMap[i].id,&tempPoint);
        }
    }
}

void ServerDebug::refreshInterpInfor()
{
    for(int i : zhACInfoMap.keys()){
        spline.Clear();
        int size = zhACInfoMap[i].keyPoint.size();
        for(int j=0; j<size; j++){
            spline.AddSamplePoint(zhACInfoMap[i].keyPoint[j].x, zhACInfoMap[i].keyPoint[j].y);
            spline.Calculation();
        }
        if(size>0){
            size = (int)(zhACInfoMap[i].keyPoint[size-1].x/timeSpace);
        }
        double y;
        for(int j=0; j<size; j++)
        {
            spline.GetCal_S(j*timeSpace,y);
            Point pointTemp;
            pointTemp.x = j*timeSpace;
            pointTemp.y = y;
            zhACInfoMap[i].InterPoint.push_back(pointTemp);
        }
    }
}

void ServerDebug::saveFileSlot(QString abcsvName, QString abbinName)
{
    saveKeyPoint(abcsvName);
    saveMiniAction(abbinName);
}

void ServerDebug::saveKeyPoint(QString abfileName)
{
    QFile saveFile(abfileName);
    if(!saveFile.open(QIODevice::WriteOnly)){
        groupWarning(QObject::tr("file fail to open!"));
        return ;
    }
    QTextStream csvStream(&saveFile);
    csvStream.setIntegerBase(10);
    int key_0 = zhACInfoMap.keys().at(0);
    int frameNum = zhACInfoMap[key_0].keyPoint.size();
    qDebug()<<"frame num:"<<frameNum;
    if(frameNum == 0){
        groupWarning(QObject::tr("frame num is zero!"));
        return ;
    }

    csvStream << "time";
    for(int i : zhACInfoMap.keys()){
        QString temp;
        temp = temp.setNum(i,10);
        temp.append("-"+zhACInfoMap[i].name);
        csvStream << "," << temp;
    }
    csvStream << "\r\n";
    for(int i=0; i<frameNum; i++) {
        csvStream << zhACInfoMap[key_0].keyPoint[i].x;
        for(int j : zhACInfoMap.keys()) {
            csvStream << "," << zhACInfoMap[j].keyPoint[i].y;
        }
        csvStream<< "\r\n";
    }
    qDebug()<<"write over";
    saveFile.flush();
    saveFile.close();
}

void ServerDebug::saveMiniAction(QString abfileName)
{
    QFile saveFile(abfileName);
    if(!saveFile.open(QIODevice::WriteOnly)){
        groupWarning(QObject::tr("file fail to open!"));
        return;
    }

    int servoNum = zhACInfoMap.size();
    int key_0 = zhACInfoMap.keys().at(0);
    int frameNum = zhACInfoMap[key_0].InterPoint.size();

    if(!frameNum) {
        groupWarning(QObject::tr("frame number is zero !"));
        saveFile.close();
        return ;
    }else if(!servoNum) {
        groupWarning(QObject::tr("servo number is zero !"));
        saveFile.close();
        return ;
    }

    QByteArray bWriteData;
    bWriteData.clear();

    for(int i=0; i<frameNum; i++){
        for(int j : zhACInfoMap.keys()){
            bWriteData.append((quint8)zhACInfoMap[j].id);
            bWriteData.append(quint16(zhACInfoMap[j].InterPoint[i].y)/0x100);
            bWriteData.append(quint16(zhACInfoMap[j].InterPoint[i].y)%0x100);
        }
    }
    saveFile.write(bWriteData, bWriteData.size());
    saveFile.close();
}

void ServerDebug::resizeEvent(QResizeEvent *event)
{
    event->accept();
    if(zhScene !=NULL){
        zhScene->rearrange();
    }
}

void ServerDebug::servoGoto(int time)
{
    bool tag = false;
    int kry_0 = zhACInfoMap.keys().at(0);
    for(int i=0; i< zhACInfoMap[kry_0].keyPoint.size(); i++){
        if(zhACInfoMap[kry_0].keyPoint[i].x == time){
            QVector<ServoInfo> posinfo;
            for(int j : zhACInfoMap.keys()){
                ServoInfo tempServo;
                tempServo.servoId = zhACInfoMap[j].id;
                tempServo.currentPos = (qint16)zhACInfoMap[j].keyPoint[i].y;
                posinfo.push_back(tempServo);
            }
            emit servoGotoSignal(posinfo);
            tag = true;
            break;
        }
    }
    if(!tag){
        groupWarning(QObject::tr("cannot find data, please refresh data!"));
    }
}

void ServerDebug::setServoTypeSlot(uchar type)
{
    servoType = type;
    controlButtonState();
}

void ServerDebug::initGraphScene()
{
    if(zhScene)
    {
        delete zhScene;
        zhScene = NULL;
        graphicsView->clearFocus();
    }
    zhScene = new GraphScene(this);
    graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    graphicsView->setScene(zhScene);
    emit zhSceneCreated(zhScene);
}

void ServerDebug::requestButtonSlot()
{
    toolsState = false;
    initGraphScene();
    emit requestInformation();
}

void ServerDebug::enableAllButtonSlot()
{
    if(enableAllButton->text() == "EnableAllServo"){
        emit enableAllInformation();
        enableAllButton->setText("DisableAllServo");
    }else if(enableAllButton->text() == "DisableAllServo"){
        emit disenableAllInformation();
        enableAllButton->setText("EnableAllServo");
    }
}

void ServerDebug::enableSomeButtonSlot()
{
    emit enableSomeInfomation();
}


void ServerDebug::controlTools()
{
    if(paramMap["debugModel"].toInt() == MODEL_FILE){
        requestButton->setEnabled(false);
        enableAllButton->setEnabled(false);
        //enableSomeButton->setEnabled(false);
    }else{
        requestButton->setEnabled(!toolsState);
        enableAllButton->setEnabled(toolsState);
        enableSomeButton->setEnabled(toolsState);
    }
}

void ServerDebug::controlButtonState()
{
    if(servoType==Digital_UBT12HB)
    {
        toolsState = true;
        controlTools();
        requestButton->setEnabled(toolsState);
    }
}

void ServerDebug::initScene()
{
    graphicsView = new QGraphicsView(this);
    requestButton = new QPushButton("pingAllServo", this);
    enableAllButton = new QPushButton("EnableAllServo", this);
    enableSomeButton = new QPushButton("EnableSomeServo", this);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(graphicsView,0,0,4,3);
    gridLayout->addWidget(requestButton,4,0);
    gridLayout->addWidget(enableAllButton,4,1);
    gridLayout->addWidget(enableSomeButton,4,2);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(gridLayout);
    setWidget(mainWidget);
    groupSignals();
}

void ServerDebug::groupWarning(QString str)
{
    QMessageBox::warning(this,tr("Wrong!"),str);
    return;
}

void ServerDebug::groupSignals()
{
    connect(requestButton,SIGNAL(clicked()),this,SLOT(requestButtonSlot()),Qt::UniqueConnection);
    connect(enableAllButton,SIGNAL(clicked()),this,SLOT(enableAllButtonSlot()),Qt::UniqueConnection);
    connect(enableSomeButton,SIGNAL(clicked()),this,SLOT(enableSomeButtonSlot()),Qt::UniqueConnection);
}

void ServerDebug::closeEvent(QCloseEvent *event)
{
    event->ignore();
    emit closed();
}
