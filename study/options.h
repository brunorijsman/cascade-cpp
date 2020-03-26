#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

class Options
{
public:
    Options();
    void parse(int argc, char** argv);
    bool multi_processing;
    int max_runs;
    bool seed_is_set;
    int seed;
    std::string output_directory;   // TODO: Use Boost path
    std::string experiments_file;
};

#endif /* ifndef OPTIONS_H */
