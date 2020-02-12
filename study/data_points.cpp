#include "data_points.h"
#include "experiments.h"
#include <algorithm>

#pragma GCC diagnostic ignored "-Wunused-parameter"   // TODO

DataPoints::DataPoints(const Experiments& experiments, int max_runs)
{
    for (auto it = experiments.experiments.begin(); it != experiments.experiments.end(); ++it) {
        std::vector<DataPoint> experiment_data_points;
        const Experiment& experiment = (*it);
        if (experiment.independent_variable == "error_rate") {
            experiment_data_points = this->error_rate_data_points(experiment, max_runs);
        }
        else if (experiment.independent_variable == "key_size") {
            experiment_data_points = this->key_size_data_points(experiment, max_runs);            
        } else {
            assert(false);
        }
    }
}

std::vector<DataPoint> DataPoints::error_rate_data_points(const Experiment& experiment,
                                                          int max_runs)
{
    // TODO
    return std::vector<DataPoint>();
}

std::vector<DataPoint> DataPoints::key_size_data_points(const Experiment& experiment,
                                                        int max_runs)
{
    // TODO
    return std::vector<DataPoint>();
}

