#include "commandanalysis.h"

#include "mainwindow.h"
#include "source/communicate/network/networkdialog.h"
#include "source/communicate/serialcom/serialdialog.h"
#include "source/insanalysis/communicateDefine.h"
#include "source/globaldefine/upDefine.h"

#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QFileInfo>
#include <QDebug>

CommandAnalysis::CommandAnalysis(QVariantMap param, QObject *parent) :
    QObject(parent), paramMap(param)
{
    zhMainWindow = NULL;
    zhSerialDialog = NULL;
    zhNetWorkDialog = NULL;

    zhTimeControler = new QTimer(this);
    connect(zhTimeControler, &QTimer::timeout, this, &CommandAnalysis::outTimeSlot);
    connect(this, SIGNAL(sendControlSignal()), this, SLOT(sendControlSlot()));

    initCmdDefine();
    servoNumber = 36;
    sendTimes = 0;
    localCmdCount = 0;
    servoType = 0x03;

    actionCount = -1;
    sequenceCount = -1;
    sequNameCount = -1;
    servoCount = -1;
}

CommandAnalysis::~CommandAnalysis()
{

}

void CommandAnalysis::getMainWindowPointer(MainWindow* p)
{
    zhMainWindow = p;
}

void CommandAnalysis::getNetWorkPointer(NetWorkDialog* p)
{
    zhNetWorkDialog = p;
    localHost = (uchar) p->getLocalHost();
    downHost = (uchar) p->getDownHost();
}

void CommandAnalysis::getSerialPointer(SerialDialog* p)
{
    zhSerialDialog = p;
    localHost = (uchar) p->getLocalHost();
    downHost = (uchar) p->getDownHost();
}

void CommandAnalysis::setParam(QVariantMap param)
{
   paramMap = param;
}

//legnth high-first
void CommandAnalysis::setParseData(QByteArray data)
{
    if(!checkHostFromDown(data)){
        groupWarning(QObject::tr("Wrong host number!"));
        return;
    }
    if(data.at(6) < RECEIVE_OK || data.at(6) > CHECK_Error){
        if(!checkParityFromDown(data)){
            rcvCheckError(data);
            return;
        }else{
            rcvOK(data);
            emit replyCmdChangedSignal(cmdMap[(int)data.at(6)]);
            //emit replyInsStateSignal(data);
        }
    }else if(!checkParityFromDown(data)){
        groupWarning(QObject::tr("checkParity is wrong!"));
        return;
    }

    switch((int)data.at(6))
    {
    case RECEIVE_OK:
        res_OK(data);
        break;
    case LENGTH_Error:
        res_LengthError(data);
        break;
    case CHECK_Error:
        res_CheckError(data);
        break;

    case ReplyServoConnection:
        replyServoConnection(data);
        break;
    case ReplyServoPosition:
        replyServoPosition(data);
        break;
    case ReplyActionNames:
        replyActionNames(data);
        break;
    case ReplySequNames:
        replySequenceName(data);
        break;
    case ReplySequContants:
        replySequence(data);
        break;
    case GOOD_MESSAGE:
        replyGoodMsg(data);
        break;
    case BAD_MESSAGE:
        replyBadMsg(data);
        break;
    default:
        groupWarning(tr("unknown configuration from down!"));
  }
    return;
}

void CommandAnalysis::res_OK(QByteArray rcvdata)
{
    if(zhTimeControler->isActive()){
        zhTimeControler->stop();
    }
    if(insDeque.empty()){
        return;
    }
    Ins top = insDeque.front();
    if(top.priority == NEED_REPlY && top.data.at(1) == rcvdata.at(1)){
        insDeque.pop_front();
        sendTimes = 0;
    }else{
        groupWarning(QObject::tr("down replied RECEIVE_OK but some error happened!"));
    }
    emit sendControlSignal();
    return;
}

void CommandAnalysis::res_LengthError(QByteArray rcvdata)
{
    groupWarning(QObject::tr("data length error!"));
    if(zhTimeControler->isActive()){
        zhTimeControler->stop();
    }
    if(insDeque.empty()){
        return;
    }
    Ins top = insDeque.front();
    if(top.priority == NEED_REPlY && top.data.at(2) == rcvdata.at(2)){
        insDeque.pop_front();
        sendTimes = 0;
    }else{
        groupWarning(QObject::tr("down replied LENGTH_ERROR and some other error happened!"));
    }
    emit sendControlSignal();
    return;
}

void CommandAnalysis::res_CheckError(QByteArray rcvdata)
{
    if(zhTimeControler->isActive()){
        zhTimeControler->stop();
    }
    if(insDeque.empty()){
        return;
    }
    Ins top = insDeque.front();
    if(top.data.at(2) == rcvdata.at(2)){
        if(sendTimes<10){
            emit sendControlSignal();
        }else{
            groupWarning(QObject::tr("check error out times!"));
            sendTimes = 0;
            insDeque.pop_front();
            emit sendControlSignal();
        }
    }
    return;
}

void CommandAnalysis::replyServoConnection(QByteArray data)
{
    /* ff 0n 01 05 00 02 13 31 01 xx */
    if(servoCount == -1){
        servoCount = data.at(8);
        servoNumber = servoCount;
        servoInfoMap.clear();
        if(servoType != (uchar)data.at(7)){
            servoType = (uchar)data.at(7);
            emit servoTypeChangedSignal(servoType);
        }
    }else if(servoCount > 0){
        ServoInfo newServo;
        newServo.servoId = data.at(7);
        newServo.currentPos = data.at(8)*256+data.at(9);
        for(int i=10; i<(int)data.size()-1; i++)
        {
            newServo.name.append(data.at(i));
        }
        servoInfoMap.insert(newServo.servoId, newServo);
        servoCount--;
    }else{
        if(data.at(4) == char(0) && data.at(5) == char(2) &&
                data.at(7) == char(255) && data.at(8) == char(255)){
            emit servoInfoChangedSignal(servoType, servoInfoMap);
        }else{
            groupWarning(QObject::tr("Upload servo info failed!"));
        }
        servoCount = -1;
    }
}

void CommandAnalysis::replyServoPosition(QByteArray data)
{
    if((data.size()-10)%3!=0 || data.at(8) != (data.size()-10)/3)
    {
        groupWarning(QObject::tr("reply current servo position error!"));
        return;
    }
    if(servoType != data.at(7)){
        groupWarning("Wrong servo type!");
        return;
    }
    if(servoNumber!=(int)data.at(8))
    {
        groupWarning("ServoNumber is Wrong!");
        return;
    }
    for(int i = 0; i < servoNumber; i++)
    {
        if(servoInfoMap.contains(uchar(data.at(9+i*3)))){
            servoInfoMap[uchar(data.at(9+i*3))].currentPos =
                    uchar(data.at(i*3+10))*256 + uchar(data.at(i*3+11));
        }
    }
    emit servoPosChangedSignal(servoInfoMap);
}

void CommandAnalysis::replyActionNames(QByteArray data)
{
    if(actionCount == -1){
        actionCount = (quint8)data.at(7);
        actionList.clear();
    }else if(actionCount > 0){
        QString str;
        for(int i=7; i<(int)data.size()-1; i++)
        {
            str.append(data.at(i));
        }
        actionList.push_back(str);
        actionCount--;
    }else{
        if(data.at(4) == char(0) && data.at(5) == char(2) &&
                data.at(7) == char(255) && data.at(8) == char(255)){
            emit ActionListChangedSignal(actionList);
        }else{
            groupWarning(QObject::tr("Upload file name failed!"));
        }
        actionCount = -1;
    }
}

void CommandAnalysis::replySequenceName(QByteArray data){
    if(sequNameCount == -1){
        sequNameCount = int(data.at(7));
        sequNameList.clear();
    }else if(sequNameCount > 0){
        QString str;
        for(int i=7; i<(int)data.size()-1; i++)
        {
            str.append(data.at(i));
        }
        sequNameList.push_back(str);
        sequNameCount--;
    }else{
        if(data.at(4) == char(0) && data.at(5) == char(2) &&
                data.at(7) == char(255) && data.at(8) == char(255)){
            emit SequenceListChangedSignal(sequNameList);
        }else{
            groupWarning(QObject::tr("Upload file name failed!"));
        }
        sequNameCount = -1;
    }
}

void CommandAnalysis::replySequence(QByteArray data)
{
    if(sequenceCount == -1){
        sequenceCount = (int)data.at(7);
        sequenceName.clear();
        for(int i=8; i<data.size()-1; i++){
            sequenceName.append(data.at(i));
        }
        sequenceContants.clear();
    }else if(sequenceCount > 0){
        if(data.at(4) == char(0) && data.at(5) == char(2) &&
                data.at(7) == char(255) && data.at(8) == char(255)){
            sequenceCount = -1;
            groupWarning(tr("Upload sequence failed!"));
            return;
        }
        QString str;
        for(int i=7; i<data.size()-1; i++){
            str.append(data.at(i));
        }
        sequenceContants.push_back(str);
        sequenceCount--;
    }else{
        if(data.at(4) == char(0) && data.at(5) == char(2) &&
                data.at(7) == char(255) && data.at(8) == char(255)){
            emit getSequenceSignal(sequenceName, sequenceContants);
        }else{
            groupWarning(tr("Upload sequence failed!"));
        }
        sequenceCount = -1;
    }
}

void CommandAnalysis::replyGoodMsg(QByteArray data)
{
    QString msg;
    for(int i=7; i<data.size()-1; i++){
        msg.append(data.at(i));
    }
    emit stateChangedSignals("[下位机]:"+msg);
}

void CommandAnalysis::replyBadMsg(QByteArray data)
{
    //zhTimeControler->stop();
    //insDeque.clear();// 直接清栈会引起崩溃，原因未知
    //emit sendControlSignal();
    sendTimes = 20; //触发超时清栈

    QString msg;
    for(int i=7; i<data.size()-1; i++){
        msg.append(data.at(i));
    }
    QMessageBox::about(zhMainWindow, tr("Message"), msg);
    return;
}

void CommandAnalysis::outTimeSlot()
{
    zhTimeControler->stop();
    sendControlSlot();
}

void CommandAnalysis::sendControlSlot()
{
    if(zhTimeControler->isActive()){
        return;
    }
    if(sendTimes >= 1 && NO_REPLY == insDeque.front().priority){
        insDeque.pop_front();
        sendTimes = 0;
    }else if(sendTimes >= 10){
        if(sendTimes == 10){ //大于10时为badMsg触发，不弹出超时提示
            groupWarning(QObject::tr("Network out time!"));
        }
        insDeque.clear();
        sendTimes = 0;
    }
    if(!insDeque.empty()){
        Ins ins = insDeque.front();
        QByteArray sendData(ins.data);
        sendTimes ++;
        sendInsData(sendData);
        if(ins.priority == NO_REPLY){
            emit sendControlSignal();
        }else{
            zhTimeControler->start(paramMap["netWait"].toInt());
        }
    }
    return;
}

void CommandAnalysis::sendInsData(QByteArray data)
{
    if(zhSerialDialog != NULL){
        if(zhSerialDialog->isConnected()){
            zhSerialDialog->sendMessage(data);
        }else{
            groupWarning(QObject::tr("The SerialCom is not open!"));
            return ;
        }

    }else if(zhNetWorkDialog != NULL){
        if(zhNetWorkDialog->isConnected()){
            zhNetWorkDialog->sendMessage(data);
        }else{
            groupWarning(QObject::tr("The Network is not work!"));
            return;
        }
    }else{
        groupWarning(QObject::tr("Disconnected!"));
        return;
    }
    emit sendCmdChangedSignal(cmdMap[uchar(data.at(6))]);
    emit sendInsStateSignal(data);
}

void CommandAnalysis::rcvOK(QByteArray rcvdata)
{
    QByteArray data = makeCommonDate(RECEIVE_OK);
    data[1] = (uchar)rcvdata.at(1);
    data[data.size()-1] = (uchar)protocalCheckData(data);
    insDeque.push_front(Ins(data, NO_REPLY));
    sendTimes = 0;
    //zhTimeControler->stop();
    emit sendControlSignal();
}

void CommandAnalysis::rcvCheckError(QByteArray rcvdata)
{
    QByteArray data = makeCommonDate(CHECK_Error);
    data[1] = (quint8)rcvdata.at(1);
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_front(Ins(data, NO_REPLY));
    sendTimes = 0;
    emit sendControlSignal();
}

void CommandAnalysis::rcvLengthError(QByteArray rcvdata)
{
    QByteArray data = makeCommonDate(LENGTH_Error);
    data[1] = (quint8)rcvdata.at(1);
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_front(Ins(data, NO_REPLY));
    sendTimes = 0;
    emit sendControlSignal();
}

void CommandAnalysis::getDownServoConditionSolt()
{
    QByteArray data = makeCommonDate(UploadServoConnection);
    insDeque.push_back(Ins(data, NEED_REPlY));
    emit sendControlSignal();
}

void CommandAnalysis::getServoPositionSolt()
{
    QByteArray data = makeCommonDate(UploadServoPosition);
    insDeque.push_back(Ins(data, NEED_REPlY));
    emit sendControlSignal();
}

void CommandAnalysis::enableDigitalServoSolt()
{
    //QByteArray data = makeCommonDate(EnableDigitalServo);
    //insDeque.push_back(Ins(data, NEED_REPlY));
    //emit sendControlSignal();
}

void CommandAnalysis::disableDigitalServoSolt()
{
    //QByteArray data = makeCommonDate(DisableDigitalServo);
    //insDeque.push_back(Ins(data, NEED_REPlY));
   // emit sendControlSignal();
}

void CommandAnalysis::uploadActionNameSlot()
{
    QByteArray data = makeCommonDate(UploadActionNames);
    insDeque.push_back(Ins(data, NEED_REPlY));
    emit sendControlSignal();
}

void CommandAnalysis::uploadSequenceNameSlot()
{
    QByteArray data = makeCommonDate(UpLoadSequNames);
    insDeque.push_back(Ins(data, NEED_REPlY));
    emit sendControlSignal();
}

void CommandAnalysis::enableSomeServoSolt(QByteArray enableInfo)
{
    QByteArray data = makeCommonDate(EnableDigitalServo);
    data.insert(7, enableInfo);
    data[4] = enableInfo.size()/0x100;
    data[5] = enableInfo.size()%0x100;
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::downLoadMiniActionSlot(QString fileName)
{
    /*fileName is absulate path*/
    QFileInfo file(fileName);
    QByteArray headPack = makeCommonDate(DownloadAction);
    headPack[4] = (byte)((2+file.fileName().size())/0x100);
    headPack[5] = (byte)((2+file.fileName().size())%0x100);

    int BuffSize = servoNumber*3;
    if(!paramMap["isMsgLimited"].toBool()){
        BuffSize = servoNumber*3;
    }else{
        BuffSize = paramMap["msgLenth"].toInt();
    }
    int flame = (BuffSize-1+file.size())/BuffSize;
    qDebug()<<"filesize:"<<file.size()<<"frame:"<<flame<<"buffsize:"<<BuffSize;
    headPack.insert(7, (uchar)(flame/0x100));
    headPack.insert(8, (uchar)(flame%0x100));
    headPack.insert(9, file.fileName().toLatin1().data());
    headPack.data()[headPack.size()-1] = protocalCheckData(headPack);

    QFile readFile(fileName.toLatin1().data());
    if(!readFile.open(QIODevice::ReadOnly))
    {
        groupWarning(QObject::tr("file fail to open!"));
        return ;
    }
    if(readFile.isOpen())
    {
        QByteArray fileData =  readFile.readAll();

        QByteArray* data = new QByteArray[flame+1];
        for(int i=0; i<flame; i++){
            data[i] = makeCommonDate(DownloadAction);
            if(i != flame-1){
                data[i].insert(7, fileData.mid(BuffSize*i, BuffSize));
            }else{
                data[i].insert(7, fileData.mid(BuffSize*i));
            }
            data[i][4] = (uchar)((data[i].size()-8)/0x100);
            data[i][5] = (uchar)((data[i].size()-8)%0x100);
            data[i][data[i].size()-1] = protocalCheckData(data[i]);
        }

        data[flame] = makeCommonDate(DownloadAction);
        data[flame][5] = 0x02;
        data[flame].insert(7, QByteArray(2, (char)0xFF));
        data[flame][9] = protocalCheckData(data[flame]);

        readFile.close();
        insDeque.push_back(Ins(headPack));
        for(int i=0; i<=flame; i++){
            insDeque.push_back(Ins(data[i]));
        }
        emit sendControlSignal();
        delete data;
    }else{
        groupWarning(QObject::tr("Can't Find the instruction File,Please Check!"));
    }
}

void CommandAnalysis::saveSequenceSlot(QString sequenceName, QStringList actionNames)
{
    int fileNum = actionNames.size();
    QByteArray headPack = makeCommonDate(DownloadSequence);
    headPack[4] = byte((2+sequenceName.size())/0x100);
    headPack[5] = byte((2+sequenceName.size())%0x100);
    headPack.insert(7, (uchar)(fileNum/0x100));
    headPack.insert(8, (uchar)(fileNum%0x100));
    headPack.insert(9, sequenceName.toLatin1().data());
    headPack[headPack.size()-1] = protocalCheckData(headPack);

    QByteArray *data = new QByteArray[actionNames.size()+1];
    QString temp;
    for(int i=0; i < fileNum; i++){
        temp = actionNames.at(i);
        data[i] = makeCommonDate(DownloadSequence);
        data[i][4] = byte(temp.size()/0x100);
        data[i][5] = byte(temp.size()%0x100);
        data[i].insert(7, temp.toLatin1().data());
        data[i][data[i].size()-1] = protocalCheckData(data[i]);
    }
    data[fileNum] = makeCommonDate(DownloadSequence);
    data[fileNum][5] = byte(0x02);
    data[fileNum].insert(7, QByteArray(2, (char)0xFF));
    data[fileNum][9] = protocalCheckData(data[fileNum]);

    insDeque.push_back(Ins(headPack));
    for(int i=0; i<=fileNum; i++){
        insDeque.push_back(Ins(data[i]));
    }
    emit sendControlSignal();
    delete data;
}

void CommandAnalysis::execMiniActionSlot(QString fileName)
{
    QByteArray data = makeCommonDate(ExecuteAction);
    data[4] = (byte)(fileName.size()/0x100);
    data[5] = (byte)(fileName.size()%0x100);
    data.insert(7, fileName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::uploadSequenceFileSlot(QString fileName)
{
    QByteArray data = makeCommonDate(UpLoadSequence);
    data[4] = (byte)(fileName.size()/0x100);
    data[5] = (byte)(fileName.size()%0x100);
    data.insert(7, fileName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::execSequenceSlot(QString fileName)
{
    QByteArray data = makeCommonDate(ExecuteSequence);
    data[4] = (byte)(fileName.size()/0x100);
    data[5] = (byte)(fileName.size()%0x100);
    data.insert(7, fileName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::deleteActionSlot(QString fileName)
{
    QByteArray data = makeCommonDate(DeleteAction);
    data[4] = (byte)(fileName.size()/0x100);
    data[5] = (byte)(fileName.size()%0x100);
    data.insert(7, fileName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::deleteSequenceSlot(QString fileName)
{
    QByteArray data = makeCommonDate(DeleteSequence);
    data[4] = (byte)(fileName.size()/0x100);
    data[5] = (byte)(fileName.size()%0x100);
    data.insert(7, fileName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::renameActionSlot(QString oldName, QString newName)
{
    QByteArray data = makeCommonDate(ReNameAction);
    data[4] = (byte)((oldName.size()+newName.size()+2)/0x100);
    data[5] = (byte)((oldName.size()+newName.size()+2)%0x100);
    data.insert(7,(byte)oldName.size());
    data.insert(8,(byte)newName.size());
    data.insert(9, oldName.toLatin1().data());
    data.insert(oldName.size()+8, newName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::renameSequenceSlot(QString oldName, QString newName)
{
    QByteArray data = makeCommonDate(RenameSequence);
    data[4] = (byte)((oldName.size()+newName.size()+2)/0x100);
    data[5] = (byte)((oldName.size()+newName.size()+2)%0x100);
    data.insert(7,(byte)oldName.size());
    data.insert(8,(byte)newName.size());
    data.insert(9, oldName.toLatin1().data());
    data.insert(oldName.size()+8, newName.toLatin1().data());
    data[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

void CommandAnalysis::moveServoSolt(QVector<ServoInfo> servoPos)
{
    QByteArray data = makeCommonDate(MoveServo);
    data[4] = (servoPos.size()*3)/0x100;
    data[5] = (servoPos.size()*3)%0x100;
    data.insert(7, QByteArray(servoPos.size()*3, '\0'));
    for(int i=0; i<servoPos.size(); i++){
        data[7+i*3] = (byte)servoPos.at(i).servoId;
        data[8+i*3] = (byte)(servoPos.at(i).currentPos/0x100);
        data[9+i*3] = (byte)(servoPos.at(i).currentPos%0x100);
    }
    data.data()[data.size()-1] = protocalCheckData(data);
    insDeque.push_back(Ins(data));
    emit sendControlSignal();
}

quint8 CommandAnalysis::protocalCheckData(QByteArray data)
{
    quint16 check = 0;
    for(int i=0;i<data.size()-1;i++)
    {
        check = (check+(quint8)data.at(i))%255;
    }
    return (quint8)check;
}

bool CommandAnalysis::checkParityFromDown(QByteArray data)
{
    quint8 lcheck = protocalCheckData(data);
    quint8 dcheck = (quint8)data.at(data.size()-1);
    return (lcheck==dcheck);
}

bool CommandAnalysis::checkHostFromDown(QByteArray data)
{
    return (localHost == (unsigned char)data.at(3) && downHost == (unsigned char)data.at(2));
}

void CommandAnalysis::groupWarning(QString str)
{
    QMessageBox::warning(zhMainWindow,tr("Wrong!"),str);
    return;
}

QByteArray CommandAnalysis::makeCommonDate(uchar command)
{
    QByteArray data(8, (char)0xFF);
    data[0] = 0xFF;
    data[1] = quint8(localCmdCount%0x100);
    data[2] = localHost;
    data[3] = downHost;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = command;
    data[7] = protocalCheckData(data);

    localCmdCount = (localCmdCount+1)%0x100;

    return data;
}

void CommandAnalysis::initCmdDefine()
{
    QList<int> cmdValue;
    cmdValue<<RECEIVE_OK<<LENGTH_Error<<CHECK_Error<<UploadServoConnection<<UploadServoPosition
           <<EnableDigitalServo<<DisableDigitalServo<<MoveServo<<DownloadAction<<ExecuteAction
          <<DeleteAction<<ReNameAction<<UploadActionNames<<DownloadSequence<<ExecuteSequence
         <<UpLoadSequNames<<UpLoadSequence<<RenameSequence<<DeleteSequence<<ReplyServoConnection
        <<ReplyServoPosition<<ReplyActionNames<<ReplySequNames<<ReplySequContants
       <<GOOD_MESSAGE<<BAD_MESSAGE;
    QList<QString> cmdName;
    cmdName<<"RECEIVE_OK"<<"LENGTH_Error"<<"CHECK_Error"<<"UploadServoConnection"<<"UploadServoPosition"
           <<"EnableDigitalServo"<<"DisableDigitalServo"<<"MoveServo"<<"DownloadAction"<<"ExecuteAction"
          <<"DeleteAction"<<"ReNameAction"<<"UploadActionNames"<<"DownloadSequence"<<"ExecuteSequence"
         <<"UpLoadSequNames"<<"UpLoadSequence"<<"RenameSequence"<<"DeleteSequence"<<"ReplyServoConnection"
        <<"ReplyServoPosition"<<"ReplyActionNames"<<"ReplySequNames"<<"ReplySequContants"
       <<"GOOD_MESSAGE"<<"BAD_MESSAGE";
    for(int i=0; i<cmdValue.size(); i++){
        cmdMap.insert(cmdValue.at(i), cmdName.at(i));
    }
}

