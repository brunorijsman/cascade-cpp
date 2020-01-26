#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

class Algorithm
{
public:
    Algorithm(std::string name, 
              unsigned cascade_iterations,
              size_t (*block_size_function)(unsigned iteration_nr, double bit_error_rate),
              unsigned biconf_iterations,
              bool biconf_error_free_streak,
              bool biconf_correct_complement,
              bool biconf_cascade,
              bool sub_block_reuse,
              bool block_parity_inference);
    static Algorithm *get_by_name(std::string name);
    std::string name;
    unsigned cascade_iterations;
    size_t (*block_size_function)(unsigned iteration_nr, double bit_error_rate);
    unsigned biconf_iterations;
    bool biconf_error_free_streak;
    bool biconf_correct_complement;
    bool biconf_cascade;
    bool sub_block_reuse;
    bool block_parity_inference;
};

#endif /* ifndef ALGORITHM_H */
