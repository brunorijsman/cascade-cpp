#include "iteration.h"
#include "algorithm.h"
#include "reconciliation.h"
#include <algorithm>
#include <random>

#include <iostream>  //@@@

using namespace Cascade;

Iteration::Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf),
    nr_key_bits(reconciliation.get_reconciled_key().get_nr_bits()),
    shuffle(this->nr_key_bits, iteration_nr == 1),
    shuffled_key(reconciliation.get_reconciled_key(), this->shuffle)
{
    std::cout << "Iteration::Iteration " << std::endl;  //@@@

    // @@@@ shuffle
    // Do the reconciliation.
    if (this->biconf) {
        this->reconcile_biconf();
    } else {
        this->reconcile_cascade();
    }
}

Iteration::~Iteration()
{
    std::cout << "Iteration::~Iteration " << std::endl;  //@@@
}

unsigned Iteration::get_iteration_nr() const
{
    return this->iteration_nr;
}

const Key& Iteration::get_shuffled_key() const
{
    return this->shuffled_key;
}

bool Iteration::get_biconf() const
{
    return this->biconf;
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
        BlockPtr block = BlockPtr(new Block(*this, block_nr, start_bit_nr, end_bit_nr));
        this->top_blocks.push_back(block);
        this->reconciliation.schedule_ask_correct_parity(block);
        block_nr += 1;
        start_bit_nr += block_size;
    }
}

void Iteration::reconcile_biconf(void)
{
}
