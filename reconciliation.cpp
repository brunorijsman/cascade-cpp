#include "reconciliation.h"
#include "algorithm.h"
#include "biconf_iteration.h"
#include "cascade_iteration.h"
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
        IterationPtr iteration_ptr(new CascadeIteration(iteration_nr));
        this->iterations[iteration_nr] = iteration_ptr;
        iteration_ptr->reconcile();
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        IterationPtr iteration_ptr(new BiconfIteration(iteration_nr));
        this->iterations[iteration_nr] = iteration_ptr;
        iteration_ptr->reconcile();
    }
}
