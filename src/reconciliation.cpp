#include "reconciliation.h"
#include "algorithm.h"
#include <assert.h>

#include <iostream> //@@@

using namespace Cascade;

Reconciliation::Reconciliation(std::string algorithm_name,
                               ClassicalSession& classical_session, 
                               const Key& noisy_key,
                               double estimated_bit_error_rate):
    classical_session(classical_session),
    noisy_key(noisy_key),
    estimated_bit_error_rate(estimated_bit_error_rate),
    reconciled_key(noisy_key)
{
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    
    this->algorithm = Algorithm::get_by_name(algorithm_name);
    assert(this->algorithm != NULL);
}

Reconciliation::~Reconciliation()
{
    std::cout << "Reconciliation::~Reconciliation " << std::endl;  //@@@
}

const Algorithm& Reconciliation::get_algorithm() const
{
    return *(this->algorithm);
}

double Reconciliation::get_estimated_bit_error_rate() const
{
    return this->estimated_bit_error_rate;
}

Key& Reconciliation::get_reconciled_key()
{
    return this->reconciled_key;
}

void Reconciliation::reconcile()
{
    unsigned iteration_nr = 0;
    for (unsigned i = 0; i < this->algorithm->nr_cascade_iterations; ++i) {
        ++iteration_nr;
        IterationPtr iteration = IterationPtr(new Iteration(*this, iteration_nr, false));
        this->iterations.push_back(iteration);
        this->service_all_pending_work();
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        IterationPtr iteration = IterationPtr(new Iteration(*this, iteration_nr, true));
        this->iterations.push_back(iteration);
        this->service_all_pending_work();
    }
}

void Reconciliation::schedule_ask_correct_parity(BlockPtr block)
{
    std::cout << "Schedule ask correct parity " << " " << block->get_name() << std::endl;  //@@@
    this->pending_ask_correct_parity_blocks.push(block);
}

void Reconciliation::service_all_pending_work()
{
    while (!this->pending_ask_correct_parity_blocks.empty() ||
           !this->pending_try_correct_blocks.empty()) {
        this->service_pending_try_correct();
        this->service_pending_ask_correct_parity();
    }
}

void Reconciliation::service_pending_try_correct()
{
    while (!this->pending_try_correct_blocks.empty()) {
        this->pending_try_correct_blocks.pop();
        // CONTINUE FROM HERE
    }
}

void Reconciliation::service_pending_ask_correct_parity()
{
    while (!this->pending_ask_correct_parity_blocks.empty()) {
        BlockPtr block = this->pending_ask_correct_parity_blocks.front();
        // CONTINUE: Name is missing for half of blocks per iteration
        std::cout << "Ask correct parity " << block->get_name() << std::endl;  //@@@
        this->pending_ask_correct_parity_blocks.pop();
        // CONTINUE FROM HERE
    }
}

