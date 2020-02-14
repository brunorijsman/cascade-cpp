#include "algorithm.h"
#include "series.h"
#include "experiments.h"
#include <algorithm>

#include <iostream>

using namespace Cascade;

Series::Series(const Experiments& experiments, int max_runs)
{
    for (auto it = experiments.experiments.begin(); it != experiments.experiments.end(); ++it) {
        std::vector<Serie> experiment_series;
        const Experiment& experiment = (*it);
        if (experiment.independent_variable == "error_rate") {
            experiment_series = this->error_rate_series(experiment, max_runs);
        }
        else if (experiment.independent_variable == "key_size") {
            experiment_series = this->key_size_series(experiment, max_runs);            
        } else {
            assert(false);
        }
        this->series.insert(this->series.begin(), experiment_series.begin(),
                            experiment_series.end());
    }
}

static std::vector<int> dvec_to_ivec(const std::vector<double> dvec)
{
    std::vector<int> ivec;
    for (auto dval: dvec) {
        int ival = std::round(dval);
        ivec.push_back(ival);
    }
    return ivec;
}

static std::string error_rate_to_str(double value)
{
    std::string str = std::to_string(value);
    while (str.back() == '0') {
        str.pop_back();
    }
    return str;
}

std::vector<std::string> experiment_algorithms(const Experiment& experiment)
{
    if (experiment.algorithms.size() == 1 && experiment.algorithms[0] == "all") {
        return Algorithm::get_all_algorithm_names();
    } else {
        return experiment.algorithms;
    }
}

std::vector<Serie> Series::error_rate_series(const Experiment& experiment, int max_runs)
{
    std::vector<Serie> series;
    for (auto algorithm: experiment_algorithms(experiment)) {
        for (auto key_size: experiment.key_sizes) {
            Serie serie;
            int rounded_key_size = std::round(key_size);
            serie.name = "algorithm=" + algorithm + ";" +
                         "key_size=" + std::to_string(rounded_key_size) + ";" +
                         "error_rate=vary";
            if (max_runs == 0) {
                serie.runs = experiment.runs;
            } else {
                serie.runs = std::min(experiment.runs, max_runs);
            }
            serie.algorithm = algorithm;
            serie.key_sizes.push_back(rounded_key_size);
            serie.error_rates = experiment.error_rates;
            series.push_back(serie);
        }
    }
    return series;
}

std::vector<Serie> Series::key_size_series(const Experiment& experiment, int max_runs)
{
    std::vector<Serie> series;
    for (auto algorithm: experiment_algorithms(experiment)) {
        for (auto error_rate: experiment.error_rates) {
            Serie serie;
            serie.name = "algorithm=" + algorithm + ";" +
                         "key_size=vary;" +
                         "error_rate=" + error_rate_to_str(error_rate);
            serie.runs = std::min(experiment.runs, max_runs);
            serie.algorithm = algorithm;
            serie.key_sizes = dvec_to_ivec(experiment.key_sizes);
            serie.error_rates.push_back(error_rate);
            series.push_back(serie);
        }
    }
    return series;
}

