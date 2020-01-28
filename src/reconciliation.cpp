#include "reconciliation.h"
#include "algorithm.h"
#include "classical_session.h"
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
        this->classical_session.start_iteration(iteration_nr, iteration->get_shuffle_seed());
        iteration->reconcile();
        this->service_all_pending_work();
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        IterationPtr iteration = IterationPtr(new Iteration(*this, iteration_nr, true));
        this->iterations.push_back(iteration);
        iteration->reconcile();
        this->service_all_pending_work();
    }
}

void Reconciliation::schedule_ask_correct_parity(BlockPtr block)
{
    std::cout << "Schedule ask correct parity " << " " << block->get_name() << std::endl;  //@@@
    this->pending_ask_correct_parity_blocks.push_back(block);
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
        this->pending_try_correct_blocks.pop_front();
        // CONTINUE FROM HERE
    }
}

void Reconciliation::service_pending_ask_correct_parity()
{
    // Ask Alice for the correct parity for each block on the ask-parity list.
    this->classical_session.ask_correct_parities(this->pending_ask_correct_parity_blocks);
    // Move the blocks over to the try-correct list.
    while (!this->pending_ask_correct_parity_blocks.empty()) {
        BlockPtr block = this->pending_ask_correct_parity_blocks.front();
        this->pending_ask_correct_parity_blocks.pop_front();
        // CONTINUE FROM HERE: schedule try correct
    }
}
