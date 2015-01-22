#include "statistics.h"

#include <random>

Statistics::Statistics()
{

}

Statistics::~Statistics()
{

}

inline double Statistics::getMean(double * data, int size)
{
    double sum = 0.0;
    for(int i=0; i<size; i++)
        sum += data[i];
    return sum/size;
}

inline double Statistics::getVariance(double * data, int size)
{
    double mean = getMean(data, size);
    double temp = 0;
    for(int i=0; i<size; i++)
        temp += (mean-data[i])*(mean-data[i]);
    return temp/size;
}

inline double Statistics::getStdDev(double * data, int size)
{
    return qSqrt(getVariance(data, size));
}

inline double Statistics::getMedian(double * data, int size)
{
    std::vector<double> stdvector;
    stdvector.assign(data, data+size);

    std::sort(stdvector.begin(), stdvector.end(), std::less<double>());

    if (size % 2 == 0)
    {
        return (stdvector[(size/2) - 1] + stdvector[size/2]) / 2.0;
    }
    else
    {
        return stdvector[size/2];
    }
}

inline void Statistics::gaussianRandomGererator(double mean, double sdv, int size, double * output)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution<> d(mean, sdv);

    for(int i=0; i<size; i++)
    {
        output[i] = d(gen);
    }

}


