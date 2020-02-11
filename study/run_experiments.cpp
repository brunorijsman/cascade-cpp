#include "experiments.h"
#include "options.h"
#include <iostream>

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    Experiments experiments(options.experiments_file);
    // TODO: implement multi_processing
    // TODO: implement max_runs (also in Python?)
    // TODO: implement output_directory
    return 0;
}
