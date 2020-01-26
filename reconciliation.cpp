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
    unsigned iteration_nr = 0;
    for (unsigned i = 0; i < this->algorithm->nr_cascade_iterations; ++i) {
        ++iteration_nr;
        this->cascade_iteration(iteration_nr);
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        this->biconf_iteration(iteration_nr);
    }
}

void Reconciliation::cascade_iteration(unsigned)
{

}

void Reconciliation::biconf_iteration(unsigned)
{

}
