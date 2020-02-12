#ifndef DATA_POINTS_H
#define DATA_POINTS_H

#include <string>
#include <vector>

class Experiment;
class Experiments;

class DataPoint
{
public:
    std::string name;
    std::string algorithm;
    std::vector<int> key_sizes;
    std::vector<double> error_rates;
    int runs;
};

class DataPoints
{
public:
    DataPoints(const Experiments& experiments, int max_runs);
    std::vector<DataPoint> data_points;
private:
    std::vector<DataPoint> error_rate_data_points(const Experiment& experiment, int max_runs);
    std::vector<DataPoint> key_size_data_points(const Experiment& experiment, int max_runs);
};

#endif /* ifndef DATA_POINTS_H */
