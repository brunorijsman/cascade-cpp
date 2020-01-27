#include "iteration.h"
#include "algorithm.h"
#include "reconciliation.h"
#include <algorithm>
#include <random>

#include <iostream>  //@@@


// TODO: Don't have multiple random devices
static std::random_device rd;
static std::mt19937 mt(rd());

using namespace Cascade;

Iteration::Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf),
    shuffled_key(reconciliation.get_reconciled_key())
{
    std::cout << "Iteration::Iteration " << this << std::endl;  //@@@

    // The initial mapping of shuffled key bits to original key bits is one-on-one.
    size_t nr_bits = shuffled_key.get_nr_bits();
    for (size_t bit_nr = 0; bit_nr < nr_bits; ++bit_nr) {
        this->shuffled_to_orig_map[bit_nr] = bit_nr;
    }
    // Shuffle the key (except in the first iteration).
    if (iteration_nr > 1) {
        for (size_t from_bit_nr = 0; from_bit_nr < nr_bits - 1; ++from_bit_nr) {
            std::uniform_int_distribution<size_t> dist(from_bit_nr + 1, nr_bits - 1);
            size_t to_bit_nr = dist(mt);
            this->shuffled_key.swap_bits(from_bit_nr, to_bit_nr);
            std::swap(this->shuffled_to_orig_map[from_bit_nr], 
                      this->shuffled_to_orig_map[to_bit_nr]);
        }
    }
    // Compute the reverse mapping of original key bits to shuffled key bits.
    for (size_t shuffled_bit_nr = 0; shuffled_bit_nr < nr_bits; ++shuffled_bit_nr) {
        size_t orig_bit_nr = this->shuffled_to_orig_map[shuffled_bit_nr];
        this->orig_to_shuffled_map[orig_bit_nr] = shuffled_bit_nr;
    }
    // Do the reconciliation.
    if (this->biconf) {
        this->reconcile_biconf();
    } else {
        this->reconcile_cascade();
    }
}

Iteration::~Iteration()
{
    std::cout << "Iteration::~Iteration " << this << std::endl;  //@@@
}

const Key& Iteration::get_shuffled_key() const
{
    return this->shuffled_key;
}

void Iteration::reconcile_cascade(void)
{
    // Create top blocks, and schedule each one for "ask correct parity".
    double estimated_bit_error_rate = this->reconciliation.get_estimated_bit_error_rate();
    const Algorithm& algorithm = this->reconciliation.get_algorithm();
    size_t block_size = algorithm.block_size_function(this->iteration_nr, estimated_bit_error_rate);
    size_t block_nr = 0;
    size_t start_bit_nr = 0;
    size_t nr_key_bits = this->shuffled_key.get_nr_bits();
    while (start_bit_nr < nr_key_bits) {
        size_t end_bit_nr = std::min(start_bit_nr + block_size, nr_key_bits) - 1;
        std::string name = "c:" + std::to_string(this->iteration_nr) + ":" + 
                           std::to_string(block_nr);
        BlockPtr block = BlockPtr(new Block(name, *this, start_bit_nr, end_bit_nr));
        this->top_blocks.push_back(block);
        this->reconciliation.schedule_ask_correct_parity(block);
        block_nr += 1;
        start_bit_nr += block_size;
    }
}

void Iteration::reconcile_biconf(void)
{
}
