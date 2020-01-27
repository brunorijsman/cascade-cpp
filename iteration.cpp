#include "iteration.h"
#include "algorithm.h"
#include "reconciliation.h"
#include <algorithm>

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

void Iteration::reconcile_cascade(void)
{
    // Create top blocks.
    double estimated_bit_error_rate = 0.001;  // TODO
    const Algorithm& algorithm = this->reconciliation.get_algorithm();
    size_t block_size = algorithm.block_size_function(this->iteration_nr, estimated_bit_error_rate);
    size_t block_nr = 0;
    size_t start_bit_nr = 0;
    while (start_bit_nr < this->nr_key_bits) {
        size_t end_bit_nr = std::min(start_bit_nr + block_size, this->nr_key_bits) - 1;
        this->top_blocks.emplace_back(this->shuffled_key, start_bit_nr, end_bit_nr);
        block_nr += 1;
    }
}

void Iteration::reconcile_biconf(void)
{
}
