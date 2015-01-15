#ifndef STATISTICS_H
#define STATISTICS_H

#include <QtMath>

class Statistics
{
public:
    Statistics();
    ~Statistics();

    double getMean(double *  data, int size);
    double getVariance(double * data, int size);
    double getStdDev(double * data, int size);
    double getMedian(double * data, int size);
    void gaussianRandomGererator(double mean, double sdv, int size, double * output);

};

#endif // STATISTICS_H
