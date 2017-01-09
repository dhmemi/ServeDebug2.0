#include "actionmanger.h"

#include "source/servodebug/SplineSample/myspline.h"
#include "source/servodebug/Graph/graphscene.h"
#include "source/globaldefine/upDefine.h"

#include <QTableWidget>
#include <QMessageBox>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <math.h>

#include <QDebug>

ActionManger::ActionManger(QVariantMap param, QTableWidget *parent) :
    QObject(parent), paramMap(param)
{
    zhScene = NULL;
    servoType = Digital_UBT12HB;
    if(paramMap["isInterpolateTimeSet"].toBool()){
        timeSpace = paramMap["interpolateTime"].toInt();
    }else{
        timeSpace = DEFAULT_INTERPPLATE_TIME;
    }
    intSpace =  1;

    groupSignals();
}

ActionManger::~ActionManger()
{
}

void ActionManger::setParam(QVariantMap param)
{
    paramMap = param;
    if(paramMap["isInterpolateTimeSet"].toBool()){
        timeSpace = paramMap["interpolateTime"].toInt();
    }
}

void ActionManger::setzhScenePointer(GraphScene *p)
{
    zhScene = p;
}

void ActionManger::setServoInfo(uchar type, ServoInfoMap info)
{
    servoType = type;
    zhServoInfoMap.clear();
    zhServoInfoMap = info;
}

void ActionManger::setServoTypeSlot(uchar type)
{
    servoType = type;
}

void ActionManger::setServoPos(ServoInfoMap info)
{
    if(info.size() != zhServoInfoMap.size()){
        groupWarning("servo number changed !");
        return;
    }
    zhServoInfoMap.clear();
    zhServoInfoMap = info;
}

void ActionManger::servoGoto(int time)
{
    bool tag = false;
    for(int i=0; i< zhACInfoMap[0].keyPoint.size(); i++){
        if(zhACInfoMap[0].keyPoint[i].x == time){
            QVector<ServoInfo> posinfo;
            for(int j : zhACInfoMap.keys()){
                zhServoInfoMap[j].currentPos = (qint16)zhACInfoMap[j].keyPoint[i].y;
                posinfo.push_back(zhServoInfoMap[j]);
            }
            emit servoGotoSignal(posinfo);
            tag = true;
            break;
        }
    }
    if(!tag){
        groupWarning(QObject::tr("cannot find data, please refresh data !"));
    }
}





