#include "experiments.h"
#include "options.h"
#include "series.h"
#include <iostream>

// TODO: implement multi_processing
// TODO: implement output_directory

void run_series(Series& series) {
    for (auto& serie: series.series) {
        for (auto key_size: serie.key_sizes) {
            for (auto error_rate: serie.error_rates) {
                // CONTINUE: Put each on of these in a "reconciliation params" object and then
                // dispatch those over all available hardware threads. Store the produced
                // datapoints into a file.
                std::cout << "algorithm=" << serie.algorithm
                          << " key_size=" << key_size
                          << " error_rate=" << error_rate
                          << " runs=" << serie.runs << std::endl;
            }
        }
    }
}

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    Experiments experiments(options.experiments_file);
    Series series(experiments, options.max_runs);
    run_series(series);
    return 0;
}
