#include "boost/program_options.hpp"
#include <iostream>

static bool multi_processing = true;

void parse_command_line(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options"); 
    desc.add_options() 
        ("help", "Display help") 
        ("disable-multi-processing,d", "Disable multi-processing");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return;
    }    
    if (vm.count("disable-multi-processing")) {
        multi_processing = false;
    }    
}

int main(int argc, char** argv)
{
    parse_command_line(argc, argv);

    std::cout << "multi_processing=" << multi_processing << std::endl;

    return 0;
}
