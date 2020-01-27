#include "reconciliation.h"
#include "algorithm.h"
#include <assert.h>

#include <iostream> //@@@

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

const Algorithm& Reconciliation::get_algorithm(void) const
{
    return *(this->algorithm);
}

double Reconciliation::get_estimated_bit_error_rate(void) const
{
    return this->estimated_bit_error_rate;
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
        this->service_all_pending_work();
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        this->iterations.emplace_back(*this, iteration_nr, true);
        this->service_all_pending_work();
    }
}

void Reconciliation::schedule_ask_correct_parity(Block* block)
{
    this->pending_ask_correct_parity_blocks.push(block);
}

void Reconciliation::service_all_pending_work(void)
{
    while (!this->pending_ask_correct_parity_blocks.empty() ||
           !this->pending_try_correct_blocks.empty()) {
        this->service_pending_try_correct();
        this->service_pending_ask_correct_parity();
    }
}

void Reconciliation::service_pending_try_correct(void)
{
    while (!this->pending_try_correct_blocks.empty()) {
        this->pending_try_correct_blocks.pop();
        // CONTINUE FROM HERE
    }
}

void Reconciliation::service_pending_ask_correct_parity(void)
{
    while (!this->pending_ask_correct_parity_blocks.empty()) {
        Block* block = this->pending_ask_correct_parity_blocks.front();
        // CONTINUE: Name is missing for half of blocks per iteration
        std::cout << "Ask correct parity " << block << " " << block->get_name() << std::endl;  //@@@
        this->pending_ask_correct_parity_blocks.pop();
        // CONTINUE FROM HERE
    }
}

