#include "boost/program_options.hpp"
#include <iostream>

static bool multi_processing = true;
static int max_runs = 0;                    // Zero means no limit
static std::string output_directory = "";

void parse_command_line(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options"); 
    desc.add_options() 
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
    try {          
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        }
        if (vm.count("disable-multi-processing")) {
            multi_processing = false;
        }    
        if (vm.count("max-runs")) {
            int command_line_max_runs = vm["max-runs"].as<int>();
            // TODO: check range
            max_runs = command_line_max_runs;
        }    
        if (vm.count("output-directory")) {
            output_directory = vm["output-directory"].as<std::string>();
        }
    }
    catch(po::error& error) {
      std::cerr << "Error: " << error.what() << std::endl << std::endl;
      exit(1);
    }
}

int main(int argc, char** argv)
{
    parse_command_line(argc, argv);

    std::cout << "multi_processing=" << multi_processing << std::endl;
    std::cout << "max_runs=" << max_runs << std::endl;
    std::cout << "output_directory=" << output_directory << std::endl;

    return 0;
}
