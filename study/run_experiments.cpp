#include "options.h"
#include <iostream>

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    // parse_experiments_file(experiments_file);

    std::cout << "multi_processing=" << options.multi_processing << std::endl;
    std::cout << "max_runs=" << options.max_runs << std::endl;
    std::cout << "output_directory=" << options.output_directory << std::endl;
    std::cout << "experiments_file=" << options.experiments_file << std::endl;

    return 0;
}
