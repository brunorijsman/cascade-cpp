#include "options.h"
#include "boost/program_options.hpp"
#include <iostream>

Options::Options():
    multi_processing(true),
    max_runs(0),                // Zero means no limit
    output_directory(),
    experiments_file()
{
}

void Options::parse(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description normal_options("Options"); 
    normal_options.add_options() 
        ("help",
         "Display help") 
        ("disable-multi-processing,d",
         "Disable multi-processing")
        ("max-runs,m",
         po::value<int>(),
         "Maximum number of reconciliation runs per data point")
        ("output-directory,o",
         po::value<std::string>(),
         "Output directory where to store data__* files");
    po::options_description hidden_options; 
    hidden_options.add_options() 
        ("experiments-file",
         po::value<std::string>(),
         "Input experiments file");
    po::options_description options;
    options.add(normal_options);
    options.add(hidden_options);    
    po::positional_options_description positional;
    positional.add("experiments-file", -1);
    try {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
                  options(options).
                  positional(positional).
                  run(),
                  vm);
        po::notify(vm);
        if (vm.count("help")) {
            std::cout << "Usage: run_experiments [options] experiments-file" << std::endl;
            std::cout << std::endl;
            std::cout << normal_options << std::endl;
            exit(0);
        }
        if (vm.count("disable-multi-processing")) {
            multi_processing = false;
        }    
        if (vm.count("max-runs")) {
            int command_line_max_runs = vm["max-runs"].as<int>();
            if (command_line_max_runs < 1) {
                std::cerr << "Error: --max-runs or -m must be >= 1" << std::endl;
                exit(1);
            }
            max_runs = command_line_max_runs;
        }    
        if (vm.count("output-directory")) {
            output_directory = vm["output-directory"].as<std::string>();
        }
        if (vm.count("experiments-file") < 1) {
            std::cerr << "Error: experiments-file is missing" << std::endl;
            exit(1);
        }
        experiments_file = vm["experiments-file"].as<std::string>();
    }
    catch(po::error& error) {
      std::cerr << "Error: " << error.what() << std::endl << std::endl;
      exit(1);
    }
}
