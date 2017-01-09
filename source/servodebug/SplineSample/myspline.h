#ifndef MYSPLINE_H
#define MYSPLINE_H

#include <vector>

using namespace std;

class MySpline
{
public:
    MySpline();
    ~MySpline();

    //add point,change _num automatic
    bool AddSamplePoint(double x,double y);

    //delete point num is the number
    //from 0
    bool DeleteSamplePoint(int num);

    //change point, num is the number
    //when x<0,means change y only
    bool UpdateSamplePoint(int num,double x,double y);

    //insert point,behind num
    bool InsertSamplePoint(int num, double x,double y);

    //clear all points
    void Clear();

    //caculate by cubic spline
    //devide the point and result
    bool Calculation();

    //get the y for x
    bool GetCal_S(double x,double & y);

    //get the x once derivative
    bool GetCal_DS(double x,double & y);

private:

    vector<double> xlist;
    vector<double> ylist;

    vector<double> b;
    vector<double> c;
    vector<double> d;
};

#endif // MYSPLINE_H
