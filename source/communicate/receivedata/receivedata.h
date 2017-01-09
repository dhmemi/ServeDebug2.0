#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QObject>
#define BROADCAST 0x5F

class QByteArray;
class QTimer;

class ReceiveData : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveData(QObject *parent = 0);
    ~ReceiveData();
    
    void setDownHost(uchar host);
    void setLocalHost(uchar host);
    QByteArray getcomplateInsData();
    void setParseData(QByteArray data); //prepare the data to parse

signals:
    void readTimeout();
    void readFinished();

private:
    void groupSignals();
    
private slots:
    void clearInformation();
    void clearBufInformetion();

private:
    enum STATE
    {
        WAIT,
        READHEAD,
        READDATA
    }state;

    int timeOut;    // over time
    QByteArray currentHeader; //reading header
    int headTargetReadBytes;  //target read bytes of header
    QByteArray currentIntData; //reading ins
    int insTargetReadBytes; //target read bytes of ins
    QByteArray completeIns; //complete ins

    QTimer* overTime;

    uchar downHost;
    uchar localHost;
};

#endif // RECEIVEDATA_H
