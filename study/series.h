#ifndef SERIES_H
#define SERIES_H

#include <string>
#include <vector>

class Experiment;
class Experiments;

class Serie
{
public:
    std::string name;
    std::string algorithm;
    std::vector<int> key_sizes;
    std::vector<double> error_rates;
    int runs;
};

class Series
{
public:
    Series(const Experiments& experiments, int max_runs);
    std::vector<Serie> series;
private:
    std::vector<Serie> error_rate_series(const Experiment& experiment, int max_runs);
    std::vector<Serie> key_size_series(const Experiment& experiment, int max_runs);
};

#endif /* ifndef SERIES_H */
