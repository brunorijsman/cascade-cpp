#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace pt = boost::property_tree;

class Experiment
{
public:
    std::string independent_variable;
    int runs;
    std::vector<std::string> algorithms;
    std::vector<double> key_sizes;
    std::vector<double> error_rates;
};

class Experiments
{
public:
    Experiments(std::string file_name);
    std::vector<Experiment> experiments;
private:
    std::vector<std::string> parse_strings_attrib(pt::ptree& tree, std::string attribute);
    std::vector<double> parse_scalars_attrib(pt::ptree& tree, std::string attribute);
    std::vector<double> parse_scalars_tree(pt::ptree& tree);
};

#endif /* ifndef EXPERIMENTS_H */
