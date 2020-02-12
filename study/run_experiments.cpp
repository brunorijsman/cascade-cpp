#include "experiments.h"
#include "options.h"
#include "series.h"
#include <iostream>

// TODO: Add support for algorithm name "all"
// TODO: implement multi_processing
// TODO: implement output_directory

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    Experiments experiments(options.experiments_file);
    Series series(experiments, options.max_runs);

    for (auto serie: series.series) {
        std::cout << "SERIE:" << std::endl;
        std::cout << "  name = " << serie.name << std::endl;
        std::cout << "  algorithm = " << serie.algorithm << std::endl;
        std::cout << "  key_sizes = " << serie.key_sizes.size() << std::endl;
        std::cout << "  error_rates = " << serie.error_rates.size() << std::endl;
    }
    return 0;
}
