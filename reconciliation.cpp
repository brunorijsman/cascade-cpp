#include "reconciliation.h"
#include "algorithm.h"
#include <assert.h>

using namespace Cascade;

Reconciliation::Reconciliation(std::string algorithm_name,
                               const Key& noisy_key,
                               double estimated_bit_error_rate):
    noisy_key(noisy_key),
    estimated_bit_error_rate(estimated_bit_error_rate),
    reconciled_key(noisy_key)
{
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    
    this->algorithm = Algorithm::get_by_name(algorithm_name);
    assert(this->algorithm != NULL);
}

Key& Reconciliation::get_reconciled_key(void)
{
    return this->reconciled_key;
}

void Reconciliation::reconcile(void)
{
    unsigned iteration_nr = 0;
    for (unsigned i = 0; i < this->algorithm->nr_cascade_iterations; ++i) {
        ++iteration_nr;
        this->iterations.emplace_back(*this, iteration_nr, false);
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        this->iterations.emplace_back(*this, iteration_nr, true);
    }
}
