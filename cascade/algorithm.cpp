#include "algorithm.h"
#include <map>
#include <math.h>
#include <cmath>


using namespace Cascade;

typedef std::map<std::string, Algorithm*> Algorithms;

static Algorithms algorithms;

static double min_estimated_bit_error_rate = 0.00001;

Algorithm::Algorithm(std::string name, 
                     int nr_cascade_iterations,
                     int (*block_size_function)(int iteration_nr,
                                                double estimated_bit_error_rate, int key_size),
                     int nr_biconf_iterations,
                     bool biconf_error_free_streak,
                     bool biconf_correct_complement,
                     bool biconf_cascade,
                     bool sub_block_reuse,
                     bool block_parity_inference,
                     bool ask_correct_parity_using_shuffle_seed):
    name(name),
    nr_cascade_iterations(nr_cascade_iterations),
    block_size_function(block_size_function),
    nr_biconf_iterations(nr_biconf_iterations),
    biconf_error_free_streak(biconf_error_free_streak),
    biconf_correct_complement(biconf_correct_complement),
    biconf_cascade(biconf_cascade),
    sub_block_reuse(sub_block_reuse),
    block_parity_inference(block_parity_inference),
    ask_correct_parity_using_shuffle_seed(ask_correct_parity_using_shuffle_seed)
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

std::vector<std::string> Algorithm::get_all_algorithm_names()
{
    std::vector<std::string> names;
    for (auto pair: algorithms) {
        names.push_back(pair.first);
    }
    return names;

}

static int original_block_size_function(int iteration_nr, double estimated_bit_error_rate,
                                        int key_size)
{
    if (estimated_bit_error_rate < min_estimated_bit_error_rate) {
        estimated_bit_error_rate = min_estimated_bit_error_rate;
    }
    if (iteration_nr == 1) {
        return ceil(0.73 / estimated_bit_error_rate);
    }
    return 2 * original_block_size_function(iteration_nr - 1, estimated_bit_error_rate, key_size);
}

Algorithm original_algorithm(
    "original",
    4,                                      // nr_cascade_iterations
    original_block_size_function,           // block_size_function
    0,                                      // nr_biconf_iterations
    false,                                  // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    false,                                  // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed


// Name in Demystifying paper: Cascade mod. (1)
// Name in Andre Reis Thesis : biconf

static int biconf_block_size_function(int iteration_nr, double estimated_bit_error_rate,
                                      int key_size)
{
    if (estimated_bit_error_rate < min_estimated_bit_error_rate) {
        estimated_bit_error_rate = min_estimated_bit_error_rate;
    }
    if (iteration_nr == 1) {
        return ceil(0.92 / estimated_bit_error_rate);
    }
    return 3 * biconf_block_size_function(iteration_nr - 1, estimated_bit_error_rate, key_size);
}

Algorithm biconf_algorithm(
    "biconf",
    2,                                      // nr_cascade_iterations
    biconf_block_size_function,             // block_size_function
    10,                                     // nr_biconf_iterations
    true,                                   // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    false,                                  // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Name in Demystifying paper: -
// Name in Andre Reis Thesis : -

Algorithm biconf_cascade_algorithm(
    "biconf-cascade",
    2,                                      // nr_cascade_iterations
    biconf_block_size_function,             // block_size_function
    10,                                     // nr_biconf_iterations
    true,                                   // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    true,                                   // biconf_cascade
    false,                                  // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Name in Demystifying paper: -
// Name in Andre Reis Thesis : -

Algorithm biconf_complement_algorithm(
    "biconf-complement",
    2,                                      // nr_cascade_iterations
    biconf_block_size_function,             // block_size_function
    10,                                     // nr_biconf_iterations
    true,                                   // biconf_error_free_streak
    true,                                   // biconf_correct_complement
    false,                                  // biconf_cascade
    false,                                  // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Name in Demystifying paper: Cascade opt. (2)
// Name in Andre Reis Thesis : yanetal (Yan et al.)

static int yanetal_block_size_function(int iteration_nr, double estimated_bit_error_rate,
                                       int key_size)
{
    if (estimated_bit_error_rate < min_estimated_bit_error_rate) {
        estimated_bit_error_rate = min_estimated_bit_error_rate;
    }
    if (iteration_nr == 1) {
        return ceil(0.80 / estimated_bit_error_rate);
    }
    if (iteration_nr == 2) {
        return 5 * yanetal_block_size_function(iteration_nr - 1, estimated_bit_error_rate,
                                               key_size);
    }
    return key_size / 2;
}

Algorithm yanetal_algorithm(
    "yanetal",
    10,                                     // nr_cascade_iterations
    yanetal_block_size_function,            // block_size_function
    0,                                      // nr_biconf_iterations
    false,                                  // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    false,                                  // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Name in Demystifying paper: Cascade opt. (3)
// Name in Andre Reis Thesis : -

static int option3456_block_size_function(int iteration_nr, double estimated_bit_error_rate,
                                          int key_size)
{
    if (estimated_bit_error_rate < min_estimated_bit_error_rate) {
        estimated_bit_error_rate = min_estimated_bit_error_rate;
    }
    if (iteration_nr == 1) {
        return ceil(1.00 / estimated_bit_error_rate);
    }
    if (iteration_nr == 2) {
        return 2 * option3456_block_size_function(iteration_nr - 1, estimated_bit_error_rate,
                                                  key_size);
    }
    return key_size / 2;
}

Algorithm option3_algorithm(
    "option3",
    16,                                     // nr_cascade_iterations
    option3456_block_size_function,         // block_size_function
    0,                                      // nr_biconf_iterations
    false,                                  // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    false,                                  // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Name in Demystifying paper: Cascade opt. (4)
// Name in Andre Reis Thesis : -

Algorithm option4_algorithm(
    "option4",
    16,                                     // nr_cascade_iterations
    option3456_block_size_function,         // block_size_function
    0,                                      // nr_biconf_iterations
    false,                                  // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    true,                                   // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Note: Cascade opt. (5) from the Demystifying paper is not supported yet:
// Need to add support for deterministic shuffling

// Note: Cascade opt. (6) from the Demystifying paper is not supported yet:
// Need to add support for singleton block removal

// Name in Demystifying paper: Cascade opt. (7)
// Name in Andre Reis Thesis : option-7

static int option7_block_size_function(int iteration_nr, double estimated_bit_error_rate,
                                       int key_size)
{
    if (estimated_bit_error_rate < min_estimated_bit_error_rate) {
        estimated_bit_error_rate = min_estimated_bit_error_rate;
    }
    if (iteration_nr == 1) {
        return ceil(pow(2.0, ceil(log2(1.00 / estimated_bit_error_rate))));
    }
    if (iteration_nr == 2) {
        return 4 * option7_block_size_function(iteration_nr - 1, estimated_bit_error_rate,
                                               key_size);
    }
    return key_size / 2;
}

Algorithm option7_algorithm(
    "option7",
    14,                                     // nr_cascade_iterations
    option7_block_size_function,            // block_size_function
    0,                                      // nr_biconf_iterations
    false,                                  // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    true,                                   // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed

// Name in Demystifying paper: Cascade opt. (8)
// Name in Andre Reis Thesis : option-8

static int option8_block_size_function(int iteration_nr, double estimated_bit_error_rate,
                                       int key_size)
{
    if (estimated_bit_error_rate < min_estimated_bit_error_rate) {
        estimated_bit_error_rate = min_estimated_bit_error_rate;
    }
    double alpha = log2(1.00 / estimated_bit_error_rate) - 0.5;
    if (iteration_nr == 1) {
        return ceil(pow(2.0, ceil(alpha)));
    }
    if (iteration_nr == 2) {
        return ceil(pow(2.0, ceil((alpha + 12.0) / 2.0)));
    }
    if (iteration_nr == 3) {
        return 4096;
    }
    return key_size / 2;
}

Algorithm option8_algorithm(
    "option8",
    14,                                     // nr_cascade_iterations
    option8_block_size_function,            // block_size_function
    0,                                      // nr_biconf_iterations
    false,                                  // biconf_error_free_streak
    false,                                  // biconf_correct_complement
    false,                                  // biconf_cascade
    true,                                   // sub_block_reuse
    false,                                  // block_parity_inference
    true);                                  // ask_correct_parity_using_shuffle_seed
