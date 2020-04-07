#include "iteration.h"
#include "algorithm.h"
#include "debug.h"
#include "reconciliation.h"
#include <algorithm>
#include <assert.h>
#include <random>

using namespace Cascade;

Iteration::Iteration(Reconciliation& reconciliation, int iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf),
    nr_key_bits(reconciliation.get_nr_key_bits()),
    shuffle(init_shuffle(reconciliation, iteration_nr)),
    shuffled_key(reconciliation.get_reconciled_key(), shuffle)
{
    if (biconf) {
        block_size = nr_key_bits / 2;
    } else {
        block_size = reconciliation.get_algorithm().block_size_function(iteration_nr,
            reconciliation.get_estimated_bit_error_rate(), nr_key_bits);
    }
    DEBUG("Start " << (biconf ? "biconf" : "cascade") << " iteration " << iteration_nr);
}

ShufflePtr Iteration::init_shuffle(Reconciliation& reconciliation, int iteration_nr)
{
    int nr_key_bits = reconciliation.get_nr_key_bits();
    const Algorithm& algorithm = reconciliation.get_algorithm();
    bool assign_seed = algorithm.ask_correct_parity_using_shuffle_seed;
    bool cache = algorithm.cache_shuffles;
    return Shuffle::new_random_shuffle(iteration_nr, nr_key_bits, assign_seed, cache);
}

Iteration::~Iteration()
{
}

Reconciliation& Iteration::get_reconciliation() const
{
    return reconciliation;
}

int Iteration::get_iteration_nr() const
{
    return iteration_nr;
}

bool Iteration::get_biconf() const
{
    return biconf;
}

ShufflePtr Iteration::get_shuffle() const
{
    return shuffle;
}

ShuffledKey& Iteration::get_shuffled_key()
{
    return shuffled_key;
}

void Iteration::schedule_initial_work()
{
    if (biconf) {
        schedule_initial_work_biconf();
    } else {
        schedule_initial_work_cascade();
    }
}

void Iteration::schedule_initial_work_cascade()
{
    // Create top blocks, and schedule each one for "ask correct parity".
    int block_nr = 0;
    int start_bit_nr = 0;
    while (start_bit_nr < nr_key_bits) {
        int end_bit_nr = std::min(start_bit_nr + block_size, nr_key_bits) - 1;
        BlockPtr block(new Block(*this, start_bit_nr, end_bit_nr, NULL, block_nr));
        top_blocks.push_back(block);
        reconciliation.schedule_ask_correct_parity(block, false);
        block_nr += 1;
        start_bit_nr += block_size;
    }
}

void Iteration::schedule_initial_work_biconf()
{
    // Randomly select half of the bits in the key. Since the key was shuffled for this iteration,
    // just selecting the first half of the bits in the shuffled key is the same as randomly
    // selecting half of the bits in the original unshuffled key.
    BlockPtr block(new Block(*this, 0, block_size-1, NULL, 0));
    top_blocks.push_back(block);

    // Ask Alice what the correct parity of the selected block is.
    reconciliation.schedule_ask_correct_parity(block, false);

    // If the algorithm wants it, also create the complementary block and ask Alice for it's parity.
    if (reconciliation.get_algorithm().biconf_correct_complement) {
        BlockPtr complement_block(new Block(*this, block_size, nr_key_bits-1, NULL, 1));
        top_blocks.push_back(complement_block);
        reconciliation.schedule_ask_correct_parity(complement_block, false);
    }
}

int Iteration::try_correct_block(BlockPtr block, bool correct_right_sibling, bool cascade)
{
    DEBUG("Try to correct block " << block->debug_str());

    // If we don't know the correct parity of the block, we cannot make progress on this block
    // until Alice has told us what the correct parity is.
    if (block->correct_parity_is_known()) {
        DEBUG("Correct parity is known: correct_parity=" << block->get_correct_parity());
    } else {
        if (block->try_to_infer_correct_parity()) {
            Stats& stats = reconciliation.get_stats();
            stats.infer_parity_blocks += 1;
            DEBUG("Correct parity was inferred: correct_parity=" << block->get_correct_parity());
        } else {
            DEBUG("Correct parity is unknown");
            reconciliation.schedule_ask_correct_parity(block, correct_right_sibling);
            return 0;
        }
    }

    // If there is an even number of errors in this block, we don't attempt to fix any errors
    // in this block. But if asked to do so, we will attempt to fix an error in the right
    // sibling block.
    int error_parity = block->get_error_parity();
    assert(error_parity != Block::unknown_parity);
    if (error_parity == 0) {
        if (correct_right_sibling) {
            DEBUG("Even error parity: try to correct right sibling");
            return try_correct_right_sibling_block(block, cascade);
        } else {
            DEBUG("Even error parity: do nothing");
            return 0;
        }
    }

    // If this block contains a single bit, we have finished the recursion and found an error.
    // Correct the error by flipping the key bit that corresponds to this block.
    if (block->get_nr_bits() == 1) {
        int orig_key_bit_nr = shuffle->shuffle_to_orig(block->get_start_bit_nr());
        DEBUG("Correct single bit: block=" << block->debug_str());
        reconciliation.correct_orig_key_bit(orig_key_bit_nr, iteration_nr, cascade);
        return 1;              
    }

    // If we get here, it means that there is an odd number of errors in this block and that the
    // block is bigger than 1 bit.
    DEBUG("Recurse into left sub-block");
    BlockPtr left_sub_block = block->get_left_sub_block();
    if (!left_sub_block) {
        left_sub_block = block->create_left_sub_block();    
    }
    return try_correct_block(left_sub_block, true, cascade);
}

int Iteration::try_correct_right_sibling_block(BlockPtr block, bool cascade)
{
    Block* parent_block = block->get_parent_block();
    assert(parent_block);
    BlockPtr right_sibling_block = parent_block->get_right_sub_block();
    if (!right_sibling_block) {
        right_sibling_block = parent_block->create_right_sub_block();    
    }
    DEBUG("Right sibling is " << right_sibling_block->debug_str());
    return try_correct_block(right_sibling_block, false, cascade);
}

BlockPtr Iteration::get_cascade_block(int orig_key_bit_nr) const
{
    int shuffled_key_bit_nr = shuffle->orig_to_shuffle(orig_key_bit_nr);
    int block_nr = shuffled_key_bit_nr / block_size;
    BlockPtr block;
    try {
        block = top_blocks.at(block_nr);
    }
    catch (const std::out_of_range&) {
        // Corner case: when running BICONF with cascading enabled and complementary blocks disabled
        // it is possible that iteration X corrects a bit, but that bit is not part of the chosen //// block in some other iteration Y.
        block = NULL;
    }
    DEBUG("Select cascading block for iteration:" <<
          " iteration_nr=" << iteration_nr <<
          " block=" << (block ? block->debug_str() : "null"));  
    return block;
}

void Iteration::flip_parity_in_all_blocks_containing_bit(int orig_key_bit_nr)
{
    int shuffled_key_bit_nr = shuffle->orig_to_shuffle(orig_key_bit_nr);
    int block_nr = shuffled_key_bit_nr / block_size;
    BlockPtr block;
    try {
        block = top_blocks.at(block_nr);
    }
    catch (const std::out_of_range&) {
        return;
    }
    block->flip_current_parity();
    while (true) {
        BlockPtr sub_block = block->get_left_sub_block();
        if (!sub_block)
            break;
        assert(shuffled_key_bit_nr >= sub_block->get_start_bit_nr());
        if (shuffled_key_bit_nr <= sub_block->get_end_bit_nr()) {
            sub_block->flip_current_parity();
            block = sub_block;
            continue;
        }
        sub_block = block->get_right_sub_block();
        if (!sub_block)
            break;
        assert(shuffled_key_bit_nr >= sub_block->get_start_bit_nr());
        assert(shuffled_key_bit_nr <= sub_block->get_end_bit_nr());
        sub_block->flip_current_parity();
        block = sub_block;
    }
}
