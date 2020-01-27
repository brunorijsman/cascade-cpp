#include "iteration.h"
#include "algorithm.h"
#include "reconciliation.h"
#include <algorithm>

#include <iostream>  //@@@

using namespace Cascade;

#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

Iteration::Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf),
    shuffled_key(reconciliation.get_reconciled_key(), iteration_nr != 1), 
    nr_key_bits(reconciliation.get_reconciled_key().get_nr_bits())
{
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

void Iteration::reconcile_cascade(void)
{
    // Create top blocks, and schedule each one for "ask correct parity".
    double estimated_bit_error_rate = this->reconciliation.get_estimated_bit_error_rate();
    const Algorithm& algorithm = this->reconciliation.get_algorithm();
    size_t block_size = algorithm.block_size_function(this->iteration_nr, estimated_bit_error_rate);
    size_t block_nr = 0;
    size_t start_bit_nr = 0;
    while (start_bit_nr < this->nr_key_bits) {
        size_t end_bit_nr = std::min(start_bit_nr + block_size, this->nr_key_bits) - 1;
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
