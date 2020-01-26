#include "reconciliation.h"
#include "algorithm.h"
#include <assert.h>
#include <random>

using namespace Cascade;

static std::random_device rd;
static std::mt19937 mt(rd());

void Reconciliation::set_seed(uint64_t seed)
{
    mt.seed(seed);
}

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

void Reconciliation::iteration_common(unsigned iteration_nr)
{
    this->iteration_keys[iteration_nr] = Key(this->reconciliated_key);
    size_t nr_key_bits = this->reconciliated_key.get_nr_bits();
    BitMap bit_map;
    for (unsigned bit_nr = 0; bit_nr < nr_key_bits; ++bit_nr) {
        bit_map[bit_nr] = bit_nr;
    }
    this->iteration_bit_maps[iteration_nr] = bit_map;
    if (iteration_nr > 1) {
        this->shuffle_iteration_key(iteration_nr);
    }
}

void Reconciliation::shuffle_iteration_key(unsigned)
{
    // CONTINUE FROM HERE
}

void Reconciliation::cascade_iteration(unsigned iteration_nr)
{
    this->iteration_common(iteration_nr);
}

void Reconciliation::biconf_iteration(unsigned iteration_nr)
{
    this->iteration_common(iteration_nr);
}
