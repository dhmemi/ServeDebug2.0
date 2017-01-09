#ifndef MAINSTRUCTURE_H
#define MAINSTRUCTURE_H

using namespace std;

#include <vector>
#include <QList>
#include <QMap>

struct Point
{
    double x;
    double y;
    Point(int a = 0, int b = 0)
    {
        x = a;
        y = b;
    }
};
typedef vector<Point> PointList;

struct InstrFrameInfo
{
    quint8* Id;
    quint16* Position;
    quint16*Speed;
    int Size;
};

struct ActionChannelInfo
{
    int id;
    QString name;
    vector<Point> keyPoint;
    vector<Point> InterPoint;
    void clear()
    {
        name.clear();
        keyPoint.clear();
        InterPoint.clear();
    }
};

typedef QMap<int, ActionChannelInfo> ACInfoMap;

typedef struct ServoInfo
{
    quint8 servoId;
    quint8 isEnabled;
    quint16 currentPos;
    QString name;
} *pServoInfo;

typedef QMap<int, ServoInfo> ServoInfoMap;

struct Ins{
    QByteArray data;
    bool priority;
    Ins(QByteArray _Data, bool _Priority = true)
        : data(_Data), priority(_Priority)
    {}
};



#endif // MAINSTRUCTURE_H
