#include "myspline.h"
#include "spline.h"

MySpline::MySpline()
{

}

MySpline::~MySpline()
{

}

bool MySpline::AddSamplePoint(double x,double y)
{
    xlist.push_back(x);
    ylist.push_back(y);
    return true;
}

bool MySpline::DeleteSamplePoint(int num)
{
    xlist.erase(xlist.begin()+num);
    return true;
}

bool MySpline::UpdateSamplePoint(int num,double x,double y)
{
    xlist[num] = x;
    ylist[num] = y;
    return true;
}

bool MySpline::InsertSamplePoint(int num, double x,double y)
{
    xlist.insert(xlist.begin()+num, x);
    ylist.insert(ylist.begin()+num, y);
    return true;
}

void MySpline::Clear()
{
    xlist.clear();
    ylist.clear();
}

bool MySpline::Calculation()
{
    int ret;
    b.resize (xlist.size()+1);
    c.resize (xlist.size()+1);
    d.resize (xlist.size()+1);
    spline(xlist.size(), 0,0,0,0, &(xlist[0]), &(ylist[0]), &(b[0]), &(c[0]), &(d[0]), &ret);
    return ret==0;
}

bool MySpline::GetCal_S(double x,double & y)
{
    int tmpLast = 0;
    y = seval(xlist.size(), x, &(xlist[0]), &(ylist[0]), &(b[0]), &(c[0]), &(d[0]), &tmpLast);
    return true;
}

bool MySpline::GetCal_DS(double x,double & y)
{
    int tmpLast = 0;
    y = deriv(xlist.size(), x, &(xlist[0]), &(b[0]), &(c[0]), &(d[0]), &tmpLast);
    return true;
}
