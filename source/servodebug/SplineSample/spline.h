int spline (int n, int end1, int end2,
            double slope1, double slope2,
            double x[], double y[],
            double b[], double c[], double d[],
            int *iflag);
double seval (int n, double u,
              double x[], double y[],
              double b[], double c[], double d[],
              int *last);
double deriv (int n, double u,
              double x[],
              double b[], double c[], double d[],
              int *last);

/*
  parameter:
    sample point:
        n_sample -------------- sample point number
        x --------------------- sample point x(independent variable)
        y --------------------- sample point y(function value)
    endpoint choose:
        end1, end2 ------------ initial point(end1)and last point(end2)appoint slope, 0-not, 1-yes
        slope1, slope2 -------- initial point(end1)and last point(end2)slope(parameter is 1 and effect)
    interpolating point:
        n_interp -------------- interpolating point number
        ux -------------------- interpolating point x
        uy -------------------- out parameter , save result in uy.
  */
int calcSpline(int n_sample, double x[], double y[],
               int end1, double slope1, int end2, double slope2,
               int n_interp, double ux[], double uy[]);
