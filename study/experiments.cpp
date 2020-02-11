#include "experiments.h"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>

static std::string dl_str(const std::vector<double>& dl) {
    std::string str;
    for (auto it = dl.begin(); it != dl.end(); ++it) {
        str += std::to_string(*it) + " ";
    }
    return str;
}

Experiments::Experiments(std::string file_name)
{
    pt::ptree root;
    pt::read_json(file_name, root);
    for (auto iter = root.begin(); iter != root.end(); iter++) {
        pt::ptree sub_tree = iter->second;
        Experiment experiment;
        experiment.independent_variable = sub_tree.get<std::string>("independent_variable");
        experiment.runs = sub_tree.get<int>("runs");
        experiment.key_sizes = this->parse_values(sub_tree, "key_size");
        experiment.error_rates = this->parse_values(sub_tree, "error_rate");

        std::cout << "independent_variable = " << experiment.independent_variable << std::endl;
        std::cout << "runs = " << experiment.runs << std::endl;
        std::cout << "key_sizes = " << dl_str(experiment.key_sizes) << std::endl;
        std::cout << "error_rates = " << dl_str(experiment.error_rates) << std::endl;
    }
}

std::vector<double> Experiments::parse_values(pt::ptree& tree, std::string attribute)
{
    if (tree.count(attribute) == 0) {    
        return std::vector<double>();
    }
    auto sub_tree = tree.get_child(attribute);
    return this->parse_sub_tree_values(sub_tree);
}

std::vector<double> Experiments::parse_sub_tree_values(pt::ptree& sub_tree)
{
    std::vector<double> values;

    // If sub-tree is a scalar value, return a list with that value only.
    try {
        double value = sub_tree.get_value<double>();
        values.push_back(value);
        return values;
    }
    catch (boost::exception const&) { }

    // Try parsing it as a {start, end, step_size} or {start, end, step_factor} range.
    if (sub_tree.count("start") != 0) {
        double start = sub_tree.get<double>("start");
        double end = sub_tree.get<double>("end");
        if (sub_tree.count("step_size") != 0) {
            // Parse as {start, end, step_size} 
            double step_size = sub_tree.get<double>("step_size");
            double value = start;
            while (value < end) {
                values.push_back(value);
                value += step_size;
            }
            return values;
        } else if (sub_tree.count("step_factor") != 0) {
            // Parse as {start, end, step_factor} 
            double step_factor = sub_tree.get<double>("step_factor");
            double value = start;
            while (value < end) {
                values.push_back(value);
                value *= step_factor;
            }
            return values;
        } else {
            std::cerr << "expected step_size or step_factor" << std::endl;
            exit(1);
        }
    }

    // Try recursively parsing it as a list of any of the above.
    for (auto it = sub_tree.begin(); it != sub_tree.end(); ++it) {
        auto more_values = this->parse_sub_tree_values(it->second);
        values.insert(values.end(), more_values.begin(), more_values.end());
    }
    return values;
}
