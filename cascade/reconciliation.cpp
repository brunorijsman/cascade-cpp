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
    all_normal_cascade_iterations();

    // BICONF iterations (if any).
    all_biconf_iterations();

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

    // Compute efficiency.
    stats.efficiency = compute_efficiency(stats.ask_parity_blocks);

    // Compute number of reconciliation message bits per key bit.
    stats.reconciliation_bits = stats.start_iteration_bits + stats.ask_parity_bits + 
                                stats.reply_parity_bits;
    stats.reconciliation_bits_per_key_bit = (double) stats.reconciliation_bits /
                                            (double) this->nr_key_bits;
}

void Reconciliation::all_normal_cascade_iterations()
{
    int iteration_nr = 0;
    for (int i = 0; i < algorithm.nr_cascade_iterations; ++i) {
        ++stats.normal_iterations;
        ++iteration_nr;
        start_iteration_common(iteration_nr, false);
        service_all_pending_work(true);
    }
}

void Reconciliation::all_biconf_iterations()
{
    if (algorithm.nr_biconf_iterations == 0)
        return;
    int iterations_to_go = algorithm.nr_biconf_iterations;
    int iteration_nr = algorithm.nr_cascade_iterations;
    while (iterations_to_go > 0) {
        ++stats.biconf_iterations;
        ++iteration_nr;
        start_iteration_common(iteration_nr, true);
        int errors_corrected = service_all_pending_work(algorithm.biconf_cascade);
        if (algorithm.biconf_error_free_streak and errors_corrected > 0)
            iterations_to_go = algorithm.nr_biconf_iterations;
        else
            iterations_to_go -= 1;
    }
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
    iteration->schedule_initial_work();
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

int Reconciliation::service_all_pending_work(bool cascade)
{
    int errors_corrected = 0;
    while (!pending_ask_correct_parity_blocks.empty() || !pending_try_correct_blocks.empty()) {
        errors_corrected += service_pending_try_correct(cascade);
        service_pending_ask_correct_parity();
    }
    return errors_corrected;
}

int Reconciliation::service_pending_try_correct(bool cascade)
{
    int errors_corrected = 0;
    while (!pending_try_correct_blocks.empty()) {
        PendingItem pending_item = pending_try_correct_blocks.front();
        pending_try_correct_blocks.pop_front();
        Iteration& iteration = pending_item.block->get_iteration();
        errors_corrected += iteration.try_correct_block(pending_item.block,
                                                        pending_item.correct_right_sibling,
                                                        cascade);
    }
    return errors_corrected;
}

static long block_bits()
{
    return 16 +     // 16 bits for iteration nr
           32 +     // 32 bits for start bit index
           32;      // 32 bits for end bit index
}

static long ask_parity_message_bits(long nr_blocks)
{
    return 16 +                        // Assumed overhead for header
           nr_blocks * block_bits();   // Size of blocks that parity is being asked for
}

static long reply_parity_message_bits(long nr_blocks)
{
    return 16 +        // Assumed overhead for header
           nr_blocks;  // One parity bit for each block
}

void Reconciliation::service_pending_ask_correct_parity()
{
    // Ask Alice for the correct parity for each block on the ask-parity list.
    long nr_blocks = pending_ask_correct_parity_blocks.size();
    if (nr_blocks == 0) {
        return;
    }
    stats.ask_parity_messages += 1;
    stats.ask_parity_blocks += nr_blocks;
    stats.ask_parity_bits += ask_parity_message_bits(nr_blocks);
    stats.reply_parity_bits += reply_parity_message_bits(nr_blocks);
    classical_session.ask_correct_parities(pending_ask_correct_parity_blocks);

    // Move all blocks over to the try-correct list.
    while (!pending_ask_correct_parity_blocks.empty()) {
        PendingItem pending_item = pending_ask_correct_parity_blocks.front();
        pending_ask_correct_parity_blocks.pop_front();
        schedule_try_correct(pending_item.block, pending_item.correct_right_sibling);
    }
}
