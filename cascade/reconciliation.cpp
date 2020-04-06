#include "reconciliation.h"
#include "algorithm.h"
#include "classical_session.h"
#include "debug.h"
#include <assert.h>
#include <math.h>
#include <time.h>

using namespace Cascade;

Reconciliation::Reconciliation(const Algorithm& algorithm,
                               ClassicalSession& classical_session, 
                               const Key& noisy_key,
                               double estimated_bit_error_rate,
                               const Key* correct_key):
    algorithm(algorithm),
    classical_session(classical_session),
    estimated_bit_error_rate(estimated_bit_error_rate),
    reconciled_key(noisy_key),
    correct_key(correct_key),
    nr_key_bits(noisy_key.get_nr_bits())
{
    DEBUG("Start reconciliation: noisy_key=" << noisy_key.to_string());
}

Reconciliation::~Reconciliation()
{
    DEBUG("End reconciliation");
}

const Algorithm& Reconciliation::get_algorithm() const
{
    return algorithm;
}

double Reconciliation::get_estimated_bit_error_rate() const
{
    return estimated_bit_error_rate;
}

Key& Reconciliation::get_reconciled_key()
{
    return reconciled_key;
}

const Key* Reconciliation::get_correct_key() const
{
    return correct_key;
}

int Reconciliation::get_nr_key_bits() const
{
    return nr_key_bits;
}

Stats& Reconciliation::get_stats()
{
    return stats;
}

static double elapsed_time(const struct timespec& start, const struct timespec& end)
{
    double d_start = double(start.tv_sec) + double(start.tv_nsec) / 1000000000.0;
    double d_end = double(end.tv_sec) + double(end.tv_nsec) / 1000000000.0;
    return d_end - d_start;
}

double Reconciliation::compute_efficiency(long reconciliation_bits) const
{
    double eps = estimated_bit_error_rate;
    double shannon_efficiency = -eps * log2(eps) - (1.0 - eps) * log2(1.0 - eps);
    int key_size = reconciled_key.get_nr_bits();
    double efficiency = double(reconciliation_bits) / (double(key_size) * shannon_efficiency);
    return efficiency;
}

void Reconciliation::reconcile()
{
    // Record start time.
    struct timespec start_process_time;
    int rc = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_process_time);
    assert(rc == 0);
    struct timespec start_real_time;
    rc = clock_gettime(CLOCK_MONOTONIC, &start_real_time);
    assert(rc == 0);

    // Normal cascade iterations.
    int iteration_nr = 0;
    for (int i = 0; i < algorithm.nr_cascade_iterations; ++i) {
        ++stats.normal_iterations;
        ++iteration_nr;
        start_iteration_common(iteration_nr, false);
        service_all_pending_work(true);
    }

    // BICONF iterations (if any).
    for (int i = 0; i < algorithm.nr_biconf_iterations; ++i) {
        ++stats.biconf_iterations;
        ++iteration_nr;
        start_iteration_common(iteration_nr, true);
        service_all_pending_work(algorithm.biconf_cascade);
    }

    // Record end time.
    struct timespec end_process_time;
    rc = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end_process_time);
    assert(rc == 0);
    struct timespec end_real_time;
    rc = clock_gettime(CLOCK_MONOTONIC, &end_real_time);
    assert(rc == 0);

    // Compute elapsed time.
    stats.elapsed_process_time = elapsed_time(start_process_time, end_process_time);
    stats.elapsed_real_time = elapsed_time(start_real_time, end_real_time);

    // Compute efficiencies.
    stats.unrealistic_efficiency = compute_efficiency(stats.ask_parity_blocks);
    long reconciliation_bits = stats.start_iteration_bits + stats.ask_parity_bits + 
                               stats.reply_parity_bits;
    stats.realistic_efficiency = compute_efficiency(reconciliation_bits);
}

void Reconciliation::start_iteration_common(int iteration_nr, bool biconf)
{
    IterationPtr iteration(new Iteration(*this, iteration_nr, biconf));
    iterations.push_back(iteration);
    stats.start_iteration_messages += 1;
    if (algorithm.ask_correct_parity_using_shuffle_seed) {
        classical_session.start_iteration_with_shuffle_seed(iteration_nr, 
                                                            iteration->get_shuffle()->get_seed());
        stats.start_iteration_bits += 32 + 64;
    } else {
        ShufflePtr shuffle = iteration->get_shuffle();
        classical_session.start_iteration_with_shuffle(iteration_nr, shuffle);
        stats.start_iteration_bits += 32 + 32 * shuffle->get_nr_bits();
    }
    iteration->reconcile();
}

void Reconciliation::schedule_try_correct(BlockPtr block, bool correct_right_sibling)
{
    DEBUG("Schedule try_correct: block=" << block->debug_str());
    PendingItem pending_item(block, correct_right_sibling);
    pending_try_correct_blocks.push_back(pending_item);
}

void Reconciliation::schedule_ask_correct_parity(BlockPtr block, bool correct_right_sibling)
{
    DEBUG("Schedule ask_correct_parity: block=" << block->debug_str());
    stats.ask_parity_bits += block->encoded_bits();
    PendingItem pending_item(block, correct_right_sibling);
    pending_ask_correct_parity_blocks.push_back(pending_item);

}

void Reconciliation::correct_orig_key_bit(int orig_key_bit_nr, int triggering_iteration_nr,
                                          bool cascade)
{
    reconciled_key.flip_bit(orig_key_bit_nr);
    for (IterationPtr iteration: iterations)
        iteration->flip_parity_in_all_blocks_containing_bit(orig_key_bit_nr);
    if (cascade)
        cascade_effect(orig_key_bit_nr, triggering_iteration_nr);
}

void Reconciliation::cascade_effect(int orig_key_bit_nr, int triggering_iteration_nr)
{
    // Re-visit every cascade iteration up to now, except the one that triggered this cascade.
    for (IterationPtr iteration: iterations) {    
        if (iteration->get_iteration_nr() != triggering_iteration_nr) {
            // Each iteration can contribute at most one block to cascade into. If there is
            // such a block, schedule it for a try-correct.
            BlockPtr block(iteration->get_cascade_block(orig_key_bit_nr));
            if (block) {
                schedule_try_correct(block, false);
            }
        }
    }
}

void Reconciliation::service_all_pending_work(bool cascade)
{
    while (!pending_ask_correct_parity_blocks.empty() || !pending_try_correct_blocks.empty()) {
        service_pending_try_correct(cascade);
        service_pending_ask_correct_parity();
    }
}

void Reconciliation::service_pending_try_correct(bool cascade)
{
    while (!pending_try_correct_blocks.empty()) {
        PendingItem pending_item = pending_try_correct_blocks.front();
        pending_try_correct_blocks.pop_front();
        Iteration& iteration = pending_item.block->get_iteration();
        iteration.try_correct_block(pending_item.block, pending_item.correct_right_sibling,
                                    cascade);
    }
}

void Reconciliation::service_pending_ask_correct_parity()
{
    // Ask Alice for the correct parity for each block on the ask-parity list.
    stats.ask_parity_messages += 1;
    stats.ask_parity_blocks += pending_ask_correct_parity_blocks.size();
    stats.reply_parity_bits += pending_ask_correct_parity_blocks.size();
    classical_session.ask_correct_parities(pending_ask_correct_parity_blocks);

    // Move all blocks over to the try-correct list.
    while (!pending_ask_correct_parity_blocks.empty()) {
        PendingItem pending_item = pending_ask_correct_parity_blocks.front();
        pending_ask_correct_parity_blocks.pop_front();
        schedule_try_correct(pending_item.block, pending_item.correct_right_sibling);
    }
}
