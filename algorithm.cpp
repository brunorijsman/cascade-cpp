#include "algorithm.h"
#include <map>
#include <math.h>

using namespace Cascade;

typedef std::map<std::string, Algorithm*> Algorithms; 

static Algorithms algorithms;

static double min_bit_error_rate = 0.00001;

Algorithm::Algorithm(std::string name, 
                     unsigned cascade_iterations,
                     size_t (*block_size_function)(unsigned iteration_nr, double bit_error_rate),
                     unsigned biconf_iterations,
                     bool biconf_error_free_streak,
                     bool biconf_correct_complement,
                     bool biconf_cascade,
                     bool sub_block_reuse,
                     bool block_parity_inference):
    name(name),
    cascade_iterations(cascade_iterations),
    block_size_function(block_size_function),
    biconf_iterations(biconf_iterations),
    biconf_error_free_streak(biconf_error_free_streak),
    biconf_correct_complement(biconf_correct_complement),
    biconf_cascade(biconf_cascade),
    sub_block_reuse(sub_block_reuse),
    block_parity_inference(block_parity_inference)
{
    algorithms[name] = this;
}

Algorithm* Algorithm::get_by_name(std::string name)
{
    Algorithms::const_iterator it = algorithms.find(name);
    if (it == algorithms.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

static size_t original_block_size_function(unsigned iteration_nr, double bit_error_rate)
{
    if (bit_error_rate < min_bit_error_rate) {
        bit_error_rate = min_bit_error_rate;
    }
    if (iteration_nr == 1) {
        return ceil(0.73 / bit_error_rate);
    }
    return 2 * original_block_size_function(iteration_nr - 1, bit_error_rate);
}

Algorithm original_algorithm("original",
                             4,                                 // cascade_iterations
                             original_block_size_function,      // block_size_function
                             0,                                 // biconf_iterations
                             false,                             // biconf_error_free_streak
                             false,                             // biconf_correct_complement
                             false,                             // biconf_cascade
                             false,                             // sub_block_reuse
                             false);                            // block_parity_inference
