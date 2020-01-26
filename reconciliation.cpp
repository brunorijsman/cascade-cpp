#include "reconciliation.h"
#include "algorithm.h"
#include <assert.h>

using namespace Cascade;

Reconciliation::Reconciliation(std::string algorithm_name,
                               const Key& noisy_key,
                               double estimated_bit_error_rate):
    noisy_key(noisy_key),
    estimated_bit_error_rate(estimated_bit_error_rate),
    reconciliated_key(noisy_key)
{
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    
    this->algorithm = Algorithm::get_by_name(algorithm_name);
    assert(this->algorithm != NULL);
}

void Reconciliation::reconcile(void)
{
    // unsigned iteration_nr = 1;

    // for (unsigned cascade_iteration_nr = 1;
    //      cascade_iteration_nr <= this->algorithm->nr_cascade_iterations;
    //      ++cascade_iteration_nr) {
        
    // }
}
