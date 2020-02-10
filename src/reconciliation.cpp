#include "reconciliation.h"
#include "algorithm.h"
#include "classical_session.h"
#include "debug.h"
#include <assert.h>

using namespace Cascade;

Reconciliation::Reconciliation(std::string algorithm_name,
                               ClassicalSession& classical_session, 
                               const Key& noisy_key,
                               double estimated_bit_error_rate):
    classical_session(classical_session),
    estimated_bit_error_rate(estimated_bit_error_rate),
    reconciled_key(noisy_key)
{
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    
    this->algorithm = Algorithm::get_by_name(algorithm_name);
    assert(this->algorithm != NULL);
    DEBUG("Start reconciliation:" <<
          " noisy_key=" << noisy_key.to_string());
}

Reconciliation::~Reconciliation()
{
    DEBUG("End reconciliation");
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
        this->classical_session.start_iteration(iteration_nr,
                                                iteration->get_shuffle().get_seed());
        iteration->reconcile();
        this->service_all_pending_work(true);
    }
    for (unsigned i = 0; i < this->algorithm->nr_biconf_iterations; ++i) {
        ++iteration_nr;        
        IterationPtr iteration = IterationPtr(new Iteration(*this, iteration_nr, true));
        this->iterations.push_back(iteration);
        this->classical_session.start_iteration(iteration_nr,
                                                iteration->get_shuffle().get_seed());
        iteration->reconcile();
        this->service_all_pending_work(this->algorithm->biconf_cascade);
    }
}

void Reconciliation::schedule_try_correct(BlockPtr block, bool correct_right_sibling)
{
    DEBUG("Schedule try_correct: " <<
          " block=" << block->compute_name());
    BlockAndBool block_and_bool(block, correct_right_sibling);
    this->pending_try_correct_blocks.push_back(block_and_bool);
}

void Reconciliation::schedule_ask_correct_parity(BlockPtr block, bool correct_right_sibling)
{
    DEBUG("Schedule ask_correct_parity: " <<
          " block=" << block->compute_name());
    BlockAndBool block_and_bool(block, correct_right_sibling);
    this->pending_ask_correct_parity_blocks.push_back(block_and_bool);
}

void Reconciliation::correct_orig_key_bit(size_t orig_key_bit_nr,
                                          unsigned triggering_iteration_nr,
                                          bool cascade)
{
    // Update the original unshuffled key.
    this->reconciled_key.flip_bit(orig_key_bit_nr);

    // Update the shuffled key for each iteration.
    for (auto it = this->iterations.begin(); it != this->iterations.end(); ++it) {
        (*it)->correct_orig_key_bit(orig_key_bit_nr);
    }

    // If asked to do so, perform the cascade effect.
    if (cascade) {
        // Re-visit every cascade iteration up to now, except the one that triggered this cascade
        // effect.
        for (auto it = this->iterations.begin(); it != this->iterations.end(); ++it) {
            IterationPtr iteration(*it);
            if (iteration->get_iteration_nr() != triggering_iteration_nr) {
                // Each iteration can contribute at most one block to cascade into. If there is
                // such a block, schedule it for a try-correct.
                BlockPtr block(iteration->get_cascade_block(orig_key_bit_nr));
                if (block) {
                    this->schedule_try_correct(block, false);
                }
            }
        }
    }
}

void Reconciliation::service_all_pending_work(bool cascade)
{
    while (!this->pending_ask_correct_parity_blocks.empty() ||
           !this->pending_try_correct_blocks.empty()) {
        this->service_pending_try_correct(cascade);
        this->service_pending_ask_correct_parity();
    }
}

void Reconciliation::service_pending_try_correct(bool cascade)
{
    while (!this->pending_try_correct_blocks.empty()) {
        BlockAndBool block_and_bool = this->pending_try_correct_blocks.front();
        BlockPtr block = block_and_bool.first;
        bool correct_right_sibling = block_and_bool.second;
        this->pending_try_correct_blocks.pop_front();
        Iteration& iteration = block->get_iteration();
        iteration.try_correct_block(block, correct_right_sibling, cascade);
    }
}

void Reconciliation::service_pending_ask_correct_parity()
{
    // Ask Alice for the correct parity for each block on the ask-parity list.
    this->classical_session.ask_correct_parities(this->pending_ask_correct_parity_blocks);
    // Move all blocks over to the try-correct list.
    while (!this->pending_ask_correct_parity_blocks.empty()) {
        BlockAndBool block_and_bool = this->pending_ask_correct_parity_blocks.front();
        BlockPtr block = block_and_bool.first;
        bool correct_right_sibling = block_and_bool.second;
        this->pending_ask_correct_parity_blocks.pop_front();
        this->schedule_try_correct(block, correct_right_sibling);
    }
}
