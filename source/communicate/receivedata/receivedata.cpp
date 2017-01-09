#include "receivedata.h"

#include <QTimer>
#include <QByteArray>

ReceiveData::ReceiveData(QObject *parent) :
    QObject(parent)
{
    state = WAIT;
    timeOut = 3000;
    overTime = NULL;
    headTargetReadBytes = 5;
    insTargetReadBytes = -1;
    localHost = BROADCAST;
    downHost = BROADCAST;

    groupSignals();
}

ReceiveData::~ReceiveData()
{
    currentHeader.clear();
    currentIntData.clear();
    completeIns.clear();
}

//header length ServoType Type
//ins insType insData check
void ReceiveData::setParseData(QByteArray data)
{
    for(int i=0;i<(int)data.size();i++)
    {
        unsigned char aByte = static_cast<unsigned char>(data.at(i));

        switch(state)
        {
        case WAIT:
            if(aByte == 0xFF)
            {
                clearBufInformetion();
                state = READHEAD;
            }
            break;
        case READHEAD:
            if(overTime==NULL)
            {
                overTime = new QTimer;
            }else{
                emit readTimeout();
                delete overTime;
                overTime = new QTimer;
                connect(overTime,SIGNAL(timeout()),this,SLOT(clearInformation()),Qt::UniqueConnection);
                connect(overTime,SIGNAL(timeout()),this,SLOT(clearBufInformetion()),Qt::UniqueConnection);
            }
            overTime->start(timeOut);

            currentHeader.append(aByte);

            if(currentHeader.size()==headTargetReadBytes)
            {
                if(localHost!=(uchar)currentHeader.at(2) || downHost!=(uchar)currentHeader.at(1)){
                    state = WAIT;
                }else{
                    int length = 0x100*(int)currentHeader.at(3) + (int)currentHeader.at(4);
                    if(length >= 0) {
                        insTargetReadBytes = length + 2;
                        state = READDATA;
                    }else{
                        state = WAIT;
                    }
                }
            }
            break;
        case READDATA:
            currentIntData.append(aByte);
            if(currentIntData.size()==insTargetReadBytes)
            {
                insTargetReadBytes = -1;

                completeIns.clear();
                completeIns.append(uchar(0xff));
                completeIns.append(currentHeader);
                completeIns.append(currentIntData);

                if(overTime != NULL)
                {
                    overTime->stop();
                    delete overTime;
                    overTime = NULL;
                }
                state = WAIT;
                emit readFinished();
            }
            break;
        }
    }
}

QByteArray ReceiveData::getcomplateInsData()
{
    return completeIns;
}

void ReceiveData::setDownHost(uchar host)
{
    downHost = host;
}

void ReceiveData::setLocalHost(uchar host)
{
    localHost = host;
}

void ReceiveData::clearInformation()
{
    state = WAIT;
    emit readTimeout();
}

void ReceiveData::groupSignals()
{
    //connect(this,SIGNAL(readFinished()),this,SLOT());
}

void ReceiveData::clearBufInformetion()
{
    currentHeader.clear();
    currentIntData.clear();
    completeIns.clear();
    insTargetReadBytes = -1;
}
