#include "experiments.h"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>

// This code for parsing for the experiments json file is not robust. We just did the bare minimum
// to be able to correctly parse the same json files that cascade-python is using.

static std::string dl_str(const std::vector<double>& dl) {
    std::string str;
    for (auto it = dl.begin(); it != dl.end(); ++it) {
        str += std::to_string(*it) + " ";
    }
    return str;
}

static std::string strl_str(const std::vector<std::string>& strl) {
    std::string str;
    for (auto it = strl.begin(); it != strl.end(); ++it) {
        str += *it + " ";
    }
    return str;
}

Experiments::Experiments(std::string file_name)
{
    pt::ptree root;
    pt::read_json(file_name, root);
    for (auto iter = root.begin(); iter != root.end(); iter++) {
        pt::ptree tree = iter->second;
        Experiment experiment;
        experiment.independent_variable = tree.get<std::string>("independent_variable");
        experiment.runs = tree.get<int>("runs");
        experiment.algorithms = this->parse_strings_attrib(tree, "algorithm");
        experiment.key_sizes = this->parse_scalars_attrib(tree, "key_size");
        experiment.error_rates = this->parse_scalars_attrib(tree, "error_rate");

        std::cout << "** EXPERIMENT **" << std::endl;
        std::cout << "independent_variable = " << experiment.independent_variable << std::endl;
        std::cout << "algorithms = " << strl_str(experiment.algorithms) << std::endl;
        std::cout << "runs = " << experiment.runs << std::endl;
        std::cout << "key_sizes = " << dl_str(experiment.key_sizes) << std::endl;
        std::cout << "error_rates = " << dl_str(experiment.error_rates) << std::endl;
    }
}

std::vector<std::string> Experiments::parse_strings_attrib(pt::ptree& tree, std::string attribute)
{
    assert (tree.count(attribute) == 1);
    std::vector<std::string> values;

    // Try parsing as a list of strings.
    auto sub_tree = tree.get_child(attribute);
    for (auto it = sub_tree.begin(); it != sub_tree.end(); ++it) {
        std::string value = it->second.get_value<std::string>();
        values.push_back(value);
    }
    if (!values.empty()) {
        return values;
    }

    // If attribute is a scalar value, return a list with that value only.
    try {
        std::string value = tree.get<std::string>(attribute);
        values.push_back(value);
        return values;
    }
    catch (boost::exception const&) {
    }

    assert(false);
}

std::vector<double> Experiments::parse_scalars_attrib(pt::ptree& tree, std::string attribute)
{
    assert (tree.count(attribute) == 1);
    auto sub_tree = tree.get_child(attribute);
    return this->parse_scalars_tree(sub_tree);
}

std::vector<double> Experiments::parse_scalars_tree(pt::ptree& tree)
{
    std::vector<double> values;

    // If sub-tree is a scalar value, return a list with that value only.
    try {
        double value = tree.get_value<double>();
        values.push_back(value);
        return values;
    }
    catch (boost::exception const&) { }

    // Try parsing it as a {start, end, step_size} or {start, end, step_factor} range.
    if (tree.count("start") != 0) {
        double start = tree.get<double>("start");
        double end = tree.get<double>("end");
        if (tree.count("step_size") != 0) {
            // Parse as {start, end, step_size} 
            double step_size = tree.get<double>("step_size");
            double value = start;
            while (value < end) {
                values.push_back(value);
                value += step_size;
            }
            return values;
        } else if (tree.count("step_factor") != 0) {
            // Parse as {start, end, step_factor} 
            double step_factor = tree.get<double>("step_factor");
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
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        auto more_values = this->parse_scalars_tree(it->second);
        values.insert(values.end(), more_values.begin(), more_values.end());
    }
    return values;
}
