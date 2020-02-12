#include "data_points.h"
#include "experiments.h"
#include "options.h"
#include <iostream>

// TODO: Add support for algorithm name "all"
// TODO: implement multi_processing
// TODO: implement output_directory

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    Experiments experiments(options.experiments_file);
    DataPoints data_points(experiments, options.max_runs);
    return 0;
}
