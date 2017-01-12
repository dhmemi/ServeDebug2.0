#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "source/communicate/network/networkdialog.h"
#include "source/communicate/serialcom/serialdialog.h"
#include "source/insanalysis/commandanalysis.h"
#include "source/logdebug/logdebug.h"
#include "source/servodebug/serverdebug.h"
#include "source/keyframewidget/keyframewidget.h"
#include "source/downserve/armmanage.h"
#include "source/servodebug/Graph/graphscene.h"
#include "source/settings/settingdialog.h"
#include "source/globaldefine/upDefine.h"
#include "source/about/about.h"

#include <QToolBar>
#include <QToolButton>
#include <QMessageBox>
#include <QJsonDocument>
#include <QLabel>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    zhNetWork = NULL;
    zhSerial = NULL;
    zhCommandAnalysis = NULL;
    zhLogDebug = NULL;
    zhServerDebug = NULL;
    zhKeyFrameWidget = NULL;
    zhArmManage = NULL;
    zhSetting = NULL;
    zhabout = NULL;

    QWidget* p = takeCentralWidget();
    if(p){
       delete p;
    }
    setDockNestingEnabled(true);

    readSettingFile();
    initToolBar();
    initButtonsState();
    initLogDebug();

    stateLabel = new QLabel(this);
    this->statusBar()->addWidget(stateLabel);
    stateLabel->setText(QObject::tr("Ready!"));
    setWindowTitle(QObject::tr("ServoDebug 2.0"));
    this->setWindowIcon(QIcon(":/MainOperator-Image/robot.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete zhCommandAnalysis;
}

void MainWindow::writeSettingFile()
{
    QFile settingFile(QDir::currentPath()+"/setting.json");
    if(settingFile.open(QFile::WriteOnly)){
        QJsonDocument setJson = QJsonDocument::fromVariant(paramMap);
        settingFile.write(setJson.toJson().data(), setJson.toJson().size());
        settingFile.close();
    }
}

void MainWindow::initToolBar()
{
    mainToolBar = new QToolBar("工具栏",this);
    mainToolBar->setIconSize(QSize(18, 18));

    serialButton = new QToolButton(this);
    serialButton->setIcon(QIcon(":/MainOperator-Image/serialCom.ico"));
    serialButton->setText("串口");
    serialButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    networkButton = new QToolButton(this);
    networkButton->setIcon(QIcon(":/MainOperator-Image/network.ico"));
    networkButton->setText("网络");
    networkButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    servoDebugButton = new QToolButton(this);
    servoDebugButton->setIcon(QIcon(":/MainOperator-Image/servo.ico"));
    servoDebugButton->setText("舵机");
    servoDebugButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    logrecordButton = new QToolButton(this);
    logrecordButton->setIcon(QIcon(":/MainOperator-Image/log.ico"));
    logrecordButton->setText("日志");
    logrecordButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    downserveButton = new QToolButton(this);
    downserveButton->setIcon(QIcon(":/MainOperator-Image/downArm.ico"));
    downserveButton->setText("文件");
    downserveButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    settingButton = new QToolButton(this);
    settingButton->setIcon(QIcon(":/MainOperator-Image/setting.png"));
    settingButton->setText("设置");
    settingButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    aboutButton = new QToolButton(this);
    aboutButton->setIcon(QIcon(":/MainOperator-Image/exit.ico"));
    aboutButton->setText("关于");
    aboutButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mainToolBar->addWidget(serialButton);
    mainToolBar->addWidget(networkButton);
    mainToolBar->addWidget(servoDebugButton);
    mainToolBar->addWidget(downserveButton);
    mainToolBar->addWidget(logrecordButton);
    mainToolBar->addWidget(settingButton);
    mainToolBar->addWidget(aboutButton);

    addToolBar(mainToolBar);
    groupSignals();
}

void MainWindow::groupSignals()
{
    connect(serialButton,SIGNAL(clicked()),this,SLOT(serialButtonSlot()),Qt::UniqueConnection);
    connect(networkButton,SIGNAL(clicked()),this,SLOT(networkButtonSlot()),Qt::UniqueConnection);
    connect(servoDebugButton,SIGNAL(clicked()),this,SLOT(servoDebugButtonSlot()),Qt::UniqueConnection);
    connect(downserveButton,SIGNAL(clicked()),this,SLOT(downserveButtonSlot()),Qt::UniqueConnection);
    connect(logrecordButton,SIGNAL(clicked()),this,SLOT(logrecordButtonSlot()),Qt::UniqueConnection);
    connect(settingButton,SIGNAL(clicked()),this,SLOT(settingButtonSlot()),Qt::UniqueConnection);
    connect(aboutButton,SIGNAL(clicked()),this,SLOT(aboutButtonSlot()),Qt::UniqueConnection);
}

void MainWindow::serialButtonSlot()
{
    if(NULL == zhSerial){
        initSerialDialog();
        networkButton->setDisabled(true);
        serialButton->setDisabled(true);
    }else if(zhSerial->isHidden()){
        zhSerial->show();
        serialButton->setDisabled(true);
    }else{
        if(zhSerial->isConnected()){
            zhSerial->hide();
            serialButton->setEnabled(true);
        }else{
            zhSerial->close();
            delete zhSerial;
            zhSerial = NULL;
            networkButton->setEnabled(true);
            serialButton->setEnabled(true);
        }
    }
}

void MainWindow::networkButtonSlot()
{
    if(NULL == zhNetWork){
        initNetWorkDialog();
        networkButton->setDisabled(true);
        serialButton->setDisabled(true);
    }else if(zhNetWork->isHidden()){
        zhNetWork->show();
        networkButton->setDisabled(true);
    }else{

        if(zhNetWork->isConnect()){
            zhNetWork->hide();
            networkButton->setEnabled(true);
        }else{
            zhNetWork->close();
            delete zhNetWork;
            zhNetWork = NULL;
            networkButton->setEnabled(true);
            serialButton->setEnabled(true);
        }
    }
}

void MainWindow::servoDebugButtonSlot()
{
    if(NULL == zhServerDebug){
        initServerDebug();
        servoDebugButton->setDisabled(true);
        downserveButton->setEnabled(true);
    }else if(zhServerDebug->isHidden()){
        zhServerDebug->show();
        servoDebugButton->setDisabled(true);
    }else{
        zhServerDebug->hide();
        servoDebugButton->setEnabled(true);
    }
}

void MainWindow::downserveButtonSlot()
{
    if(NULL == zhArmManage){
        initArmManage();
        downserveButton->setDisabled(true);
    }else if(zhArmManage->isHidden()){
        zhArmManage->show();
        downserveButton->setDisabled(true);
    }else{
        zhArmManage->hide();
        downserveButton->setEnabled(true);
    }
}

void MainWindow::logrecordButtonSlot()
{
    if(NULL == zhLogDebug){
        initLogDebug();
        logrecordButton->setDisabled(false);
    }else if(zhLogDebug->isHidden()){
        zhLogDebug->show();
        logrecordButton->setDisabled(true);
    }else{
        zhLogDebug->hide();
        logrecordButton->setEnabled(true);
    }
}

void MainWindow::settingButtonSlot()
{
    if(NULL == zhSetting){
        initSetting();
    }
}

void MainWindow::aboutButtonSlot()
{
    initAbout();
}

void MainWindow::setStateSlot(QString str)
{
    stateLabel->setText(str);
    statusBar()->show();
}

void MainWindow::initNetWorkDialog()
{
    if(NULL==zhNetWork)
    {
        zhNetWork = new NetWorkDialog(paramMap["NetSetting"].toMap(),this);
        zhNetWork->show();
        zhNetWork->setAttribute(Qt::WA_DeleteOnClose);
        connect(zhNetWork,SIGNAL(getConnected()),this,SLOT(createCommandAnalysis()));
        connect(zhNetWork,SIGNAL(getDisConnected()),this,SLOT(destroyCommandAnalysisSlot()));
        connect(zhNetWork,SIGNAL(hasSendIns(QByteArray)),zhLogDebug,SLOT(setSendInsSlot(QByteArray)));
        connect(zhNetWork,SIGNAL(hasReplyIns(QByteArray)),zhLogDebug,SLOT(setReplyInsSlot(QByteArray)));
        connect(zhNetWork,SIGNAL(closed()),this,SLOT(networkButtonSlot()));
        connect(zhNetWork, &NetWorkDialog::saveParamSignal, this, &MainWindow::saveNetSettingSlot);
        emit startConnect(QObject::tr("NetWork"));
    }
    zhNetWork->show();
}

void MainWindow::initSerialDialog()
{
    if(NULL==zhSerial)
    {
        zhSerial = new SerialDialog(paramMap["ComSetting"].toMap(),this);
        zhSerial->show();
        zhSerial->setAttribute(Qt::WA_DeleteOnClose);
        connect(zhSerial,&SerialDialog::zhComIsOpen,this,&MainWindow::createCommandAnalysis);
        connect(zhSerial,&SerialDialog::zhComIsClose,this,&MainWindow::destroyCommandAnalysisSlot);
        connect(zhSerial,SIGNAL(hasSendIns(QByteArray)),zhLogDebug,SLOT(setSendInsSlot(QByteArray)));
        connect(zhSerial,SIGNAL(hasReplyIns(QByteArray)),zhLogDebug,SLOT(setReplyInsSlot(QByteArray)));
        connect(zhSerial,SIGNAL(closed()),this,SLOT(serialButtonSlot()),Qt::UniqueConnection);
        connect(zhSerial, &SerialDialog::saveParamSignal, this, &MainWindow::saveComSettingSlot);
        emit startConnect(QObject::tr("SerialCom"));
    }
    zhSerial->show();
}

void MainWindow::createCommandAnalysis()
{
    initCommandAnalysis();
    qDebug()<<"servobutton set enabled";
    servoDebugButton->setEnabled(true);
}

void MainWindow::destroyCommandAnalysisSlot()
{
    zhCommandAnalysis->destroyed();
    delete zhCommandAnalysis;
    zhCommandAnalysis = NULL;
}

void MainWindow::initCommandAnalysis()
{
    if(NULL==zhCommandAnalysis)
    {
        zhCommandAnalysis = new CommandAnalysis(paramMap, this);
        zhCommandAnalysis->getMainWindowPointer(this);
        if(zhNetWork!=NULL)
        {
            zhCommandAnalysis->getNetWorkPointer(zhNetWork);
            connect(zhNetWork,SIGNAL(hasReplyIns(QByteArray)),zhCommandAnalysis,SLOT(setParseData(QByteArray)));
        }
        else if(zhSerial!=NULL)
        {
            zhCommandAnalysis->getSerialPointer(zhSerial);
            connect(zhSerial,SIGNAL(hasReplyIns(QByteArray)),zhCommandAnalysis,SLOT(setParseData(QByteArray)));
        }
        else
        {
            QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("Communicate is NULL!"));
            return;
        }
        connect(zhCommandAnalysis,&CommandAnalysis::replyCmdChangedSignal,zhLogDebug,&LogDebug::setReplyCmdSlot);
        connect(zhCommandAnalysis,&CommandAnalysis::sendCmdChangedSignal,zhLogDebug,&LogDebug::setSendCmdSlot);
        connect(zhCommandAnalysis,&CommandAnalysis::servoInfoChangedSignal,zhLogDebug,&LogDebug::setservoTable);
        connect(zhCommandAnalysis,&CommandAnalysis::stateChangedSignals,this,&MainWindow::setStateSlot);

    }
}

void MainWindow::initLogDebug()
{
    if(NULL==zhLogDebug)
    {
        zhLogDebug = new LogDebug(paramMap,this);
        addDockWidget(Qt::LeftDockWidgetArea, zhLogDebug);
        zhLogDebug->hide();
        connect(this,SIGNAL(startConnect(QString)),zhLogDebug,SLOT(setconnectState(QString)));
        connect(zhLogDebug,SIGNAL(closed()),this,SLOT(logrecordButtonSlot()),Qt::UniqueConnection);
        zhLogDebug->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void MainWindow::initServerDebug()
{
    initKeyFrameWidget();
    if(NULL==zhServerDebug)
    {
        zhServerDebug = new ServerDebug(paramMap, this);
        addDockWidget(Qt::LeftDockWidgetArea, zhServerDebug);
        zhServerDebug->setAttribute(Qt::WA_DeleteOnClose);
        zhServerDebug->show();

        connect(zhServerDebug,&ServerDebug::zhSceneCreated,this,&MainWindow::groupzhSceneSignals);
        connect(zhServerDebug,&ServerDebug::closed,this,&MainWindow::servoDebugButtonSlot);
        connect(zhServerDebug,&ServerDebug::enableSomeInfomation,zhKeyFrameWidget,&KeyFrameWidget::enableSomeServoDialog);
        connect(zhKeyFrameWidget,&KeyFrameWidget::dataChangedSignal,zhServerDebug,&ServerDebug::refreshSlot);
        if(zhCommandAnalysis !=NULL){
            connect(zhKeyFrameWidget,&KeyFrameWidget::gotoFrameSignal,zhCommandAnalysis,&CommandAnalysis::moveServoSolt);
            connect(zhServerDebug,&ServerDebug::requestInformation,zhCommandAnalysis,&CommandAnalysis::getDownServoConditionSolt);
            connect(zhServerDebug,&ServerDebug::enableAllInformation,zhCommandAnalysis,&CommandAnalysis::enableDigitalServoSolt);
            connect(zhServerDebug,&ServerDebug::disenableAllInformation,zhCommandAnalysis,&CommandAnalysis::disableDigitalServoSolt);
            connect(zhServerDebug,&ServerDebug::servoGotoSignal,zhCommandAnalysis,&CommandAnalysis::moveServoSolt);
            connect(zhCommandAnalysis,&CommandAnalysis::servoTypeChangedSignal,zhServerDebug,&ServerDebug::setServoTypeSlot);
        }
    }
    //initscene when request servo information
    //zhServerDebug->initGraphScene();
}

void MainWindow::initKeyFrameWidget()
{
    if(NULL==zhKeyFrameWidget)
    {
        zhKeyFrameWidget = new KeyFrameWidget(paramMap, this);
        addDockWidget(Qt::RightDockWidgetArea,zhKeyFrameWidget);
        zhKeyFrameWidget->show();
        zhKeyFrameWidget->setAttribute(Qt::WA_DeleteOnClose);
    }
    if(zhCommandAnalysis == NULL){
        return;
    }
    connect(zhKeyFrameWidget,&KeyFrameWidget::getDownPosSignal,zhCommandAnalysis,&CommandAnalysis::getServoPositionSolt);
    connect(zhKeyFrameWidget,&KeyFrameWidget::servoEnableChangedSignal,zhCommandAnalysis,&CommandAnalysis::enableSomeServoSolt);
    connect(zhCommandAnalysis,&CommandAnalysis::servoInfoChangedSignal,zhKeyFrameWidget,&KeyFrameWidget::setSliderConnectionSlot);
    connect(zhCommandAnalysis,&CommandAnalysis::servoPosChangedSignal,zhKeyFrameWidget,&KeyFrameWidget::setSliderPosSlot);
}

void MainWindow::initArmManage()
{
    if(NULL==zhArmManage)
    {
        zhArmManage = new ArmManage(paramMap, this);
        addDockWidget(Qt::RightDockWidgetArea,zhArmManage);
        zhArmManage->show();
        zhArmManage->setAttribute(Qt::WA_DeleteOnClose);
    }
    if(zhServerDebug == NULL && paramMap["debugModel"].toInt() == MODEL_FILE){
        initServerDebug();
    }
    connect(zhArmManage,&ArmManage::upSaveActionSignal,zhServerDebug,&ServerDebug::saveFileSlot);
    connect(zhArmManage,&ArmManage::upOpenActionSiganl,zhKeyFrameWidget,&KeyFrameWidget::openFileSlot);
    connect(zhArmManage,&ArmManage::closed,this,&MainWindow::downserveButtonSlot);
    if(zhCommandAnalysis != NULL){
        connect(zhArmManage,&ArmManage::downloadActionSignal,zhCommandAnalysis,&CommandAnalysis::downLoadMiniActionSlot);
        connect(zhArmManage,&ArmManage::downloadSequenceSignal,zhCommandAnalysis,&CommandAnalysis::saveSequenceSlot);
        connect(zhArmManage,&ArmManage::downRefreshActionSignal,zhCommandAnalysis,&CommandAnalysis::uploadActionNameSlot);
        connect(zhArmManage,&ArmManage::downRefreshSequenceSignal,zhCommandAnalysis,&CommandAnalysis::uploadSequenceNameSlot);
        connect(zhArmManage,&ArmManage::downUploadSequenceSiganl,zhCommandAnalysis,&CommandAnalysis::uploadSequenceFileSlot);
        connect(zhArmManage,&ArmManage::downRenameActionSignal,zhCommandAnalysis,&CommandAnalysis::renameActionSlot);
        connect(zhArmManage,&ArmManage::downExecuteActionSignal,zhCommandAnalysis,&CommandAnalysis::execMiniActionSlot);
        connect(zhArmManage,&ArmManage::downDeleteActionSignal,zhCommandAnalysis,&CommandAnalysis::deleteActionSlot);
        connect(zhArmManage,&ArmManage::downExecuteSequenceSignal,zhCommandAnalysis,&CommandAnalysis::execSequenceSlot);
        connect(zhArmManage,&ArmManage::downRenameSequenceSignal,zhCommandAnalysis,&CommandAnalysis::renameSequenceSlot);
        connect(zhArmManage,&ArmManage::downDeleteSequenceSignal,zhCommandAnalysis,&CommandAnalysis::deleteSequenceSlot);
        connect(zhCommandAnalysis,&CommandAnalysis::SequenceListChangedSignal,zhArmManage,&ArmManage::downReplySequenceSlot);
        connect(zhCommandAnalysis,&CommandAnalysis::getSequenceSignal,zhArmManage,&ArmManage::downUploadSequenceSlot);
        connect(zhCommandAnalysis,&CommandAnalysis::ActionListChangedSignal,zhArmManage,&ArmManage::downReplyActionSlot);
    }
}

void MainWindow::readSettingFile()
{
    QFile settingFile(QDir::currentPath()+"/setting.json");
    if(settingFile.open(QFile::ReadOnly)){
        QByteArray setData = settingFile.readAll();
        QJsonParseError dataError;
        QJsonDocument setJson = QJsonDocument::fromJson(setData, &dataError);
        if(dataError.error == QJsonParseError::NoError && setJson.isObject()){
            paramMap = setJson.toVariant().toMap();
        }
        settingFile.close();
    }

    if(!paramMap.contains("csvPath")){
        paramMap.insert("csvPath", DEFAULT_CSV_PATH);
    }
    if(!paramMap.contains("binPath")){
        paramMap.insert("binPath", DEFAULT_BIN_PATH);
    }
    if(!paramMap.contains("odrPath")){
        paramMap.insert("odrPath", DEFAULT_ODR_PATH);
    }
    if(!paramMap.contains("netWait")){
        paramMap.insert("netWait", DEFAULT_ARQ_WAIT);
    }
    if(!paramMap.contains("debugModel")){
        paramMap.insert("debugModel", DEFAULT_DBG_MODEL);
    }
    if(!paramMap.contains("msgLenth")){
        paramMap.insert("msgLenth", DEFAULT_MSG_LEN);
    }
    if(!paramMap.contains("interpolateTime")){
        paramMap.insert("interpolateTime", DEFAULT_INTERPPLATE_TIME);
    }
    if(!paramMap.contains("isMsgLimited")){
        paramMap.insert("isMsgLimited",DEFAULT_IS_MSG_LIMITED);
    }
    if(!paramMap.contains("isInterpolateTimeSet")){
        paramMap.insert("isInterpolateTimeSet", DEFAULT_IS_ITP_LIMITED);
    }
    if(!paramMap.contains("tabDrc")){
        paramMap.insert("tabDrc", DEFAULT_TAB_DRC);
    }
    if(!paramMap.contains("logFormat")){
        paramMap.insert("logFormat", DEFAULT_LOG_FORMAT);
    }
    if(!paramMap.contains("NetSetting")){
        paramMap.insert("NetSetting", QVariantMap());
    }
    if(!paramMap.contains("ComSetting")){
        paramMap.insert("ComSetting", QVariantMap());
    }
}

void MainWindow::initSetting()
{
    if(zhSetting == NULL){
        zhSetting = new SettingDialog(paramMap, this);
    }
    connect(zhSetting,&SettingDialog::settingChanged,this,&MainWindow::getSetting);
    zhSetting->exec();
    delete zhSetting;
    zhSetting = NULL;
}

void MainWindow::initAbout()
{
    if(zhabout == NULL){
        zhabout = new about(this);
    }
    zhabout->exec();
    delete zhabout;
    zhabout=NULL;
}



void MainWindow::saveNetSettingSlot(QVariantMap param)
{
    paramMap.insert("NetSetting",param);
}

void MainWindow::saveComSettingSlot(QVariantMap param)
{
    paramMap.insert("ComSetting",param);
}

void MainWindow::getSetting()
{
    paramMap = zhSetting->getSetting();

    if(zhLogDebug != NULL){
        zhLogDebug->setParam(paramMap);
    }
    if(zhCommandAnalysis != NULL){
        zhCommandAnalysis->setParam(paramMap);
    }
    if(zhServerDebug != NULL){
        zhServerDebug->setParam(paramMap);
    }
    if(zhArmManage != NULL){
        zhArmManage->setParam(paramMap);
    }
    if(zhKeyFrameWidget != NULL){
        zhKeyFrameWidget->setParam(paramMap);
    }

    writeSettingFile();
}

void MainWindow::groupzhSceneSignals(GraphScene* p)
{
    if(NULL==zhKeyFrameWidget)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("KeyFrameWidget is NULL!"));
        return;
    }
    connect(p,&GraphScene::insertKeyFrame,zhKeyFrameWidget,&KeyFrameWidget::ghpInsertSlot);
    connect(p,&GraphScene::keyFrameDelete,zhKeyFrameWidget,&KeyFrameWidget::ghpRemoveSlot);
    connect(p,&GraphScene::keyFrameModify,zhKeyFrameWidget,&KeyFrameWidget::ghpModifySlot);
    connect(p,&GraphScene::keyFrameMove,zhKeyFrameWidget,&KeyFrameWidget::ghpMoveSlot);
    connect(p,&GraphScene::keyFrameSelected,zhKeyFrameWidget,&KeyFrameWidget::ghpSelected);
    connect(p,&GraphScene::keyFrameGoto,zhServerDebug,&ServerDebug::servoGoto);
    connect(p,&GraphScene::keyFrameLoad,zhKeyFrameWidget,&KeyFrameWidget::ghpRefreshSlot);
}

void MainWindow::initButtonsState()
{
    if(paramMap["debugModel"].toInt()==MODEL_FILE){
        serialButton->setEnabled(false);
        networkButton->setEnabled(false);
        servoDebugButton->setEnabled(false);
    }else{
        servoDebugButton->setEnabled(false);
        downserveButton->setEnabled(false);
    }
}

void MainWindow::setServoTypeState(unsigned char type)
{
    stateLabel->setText(QObject::tr("ServoType:%1").arg((int)type));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettingFile();
    event->accept();
}
