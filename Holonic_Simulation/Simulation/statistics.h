#ifndef STATISTICS_H
#define STATISTICS_H

#include <QtMath>

class Statistics
{
public:
    Statistics();
    ~Statistics();

    inline double getMean(double *  data, int size);
    inline double getVariance(double * data, int size);
    inline double getStdDev(double * data, int size);
    inline double getMedian(double * data, int size);
    inline void gaussianRandomGererator(double mean, double sdv, int size, double * output);

};

#endif // STATISTICS_H
