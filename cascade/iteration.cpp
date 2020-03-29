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
    nr_key_bits(reconciliation.get_reconciled_key().get_nr_bits()),
    shuffle(nr_key_bits, iteration_nr == 1),
    shuffled_key(reconciliation.get_reconciled_key(), shuffle),
    block_size(reconciliation.get_algorithm().block_size_function(
        iteration_nr,
        reconciliation.get_estimated_bit_error_rate(),
        nr_key_bits))
{
    DEBUG("Start " << (biconf ? "biconf" : "cascade") << " iteration " << iteration_nr);
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

const Shuffle& Iteration::get_shuffle() const
{
    return shuffle;    
}

const Key& Iteration::get_shuffled_key() const
{
    return shuffled_key;
}

void Iteration::reconcile()
{
    if (biconf) {
        reconcile_biconf();
    } else {
        reconcile_cascade();
    }
}

void Iteration::correct_orig_key_bit(int orig_key_bit_nr)
{
    int shuffle_key_bit_nr = shuffle.orig_to_shuffle(orig_key_bit_nr);
    shuffled_key.flip_bit(shuffle_key_bit_nr);
}

void Iteration::reconcile_cascade()
{
    // Create top blocks, and schedule each one for "ask correct parity".
    int block_nr = 0;
    int start_bit_nr = 0;
    int nr_key_bits = shuffled_key.get_nr_bits();
    while (start_bit_nr < nr_key_bits) {
        int end_bit_nr = std::min(start_bit_nr + block_size, nr_key_bits) - 1;
        BlockPtr block(new Block(*this, NULL, block_nr, start_bit_nr, end_bit_nr));
        top_blocks.push_back(block);
        reconciliation.schedule_ask_correct_parity(block, false);
        block_nr += 1;
        start_bit_nr += block_size;
    }
}

void Iteration::reconcile_biconf()
{
    // Randomly select half of the bits in the key. Since the key was shuffled for this iteration,
    // just selecting the first half of the bits in the shuffled key is the same as randomly
    // selecting half of the bits in the original unshuffled key.
    int key_size = shuffled_key.get_nr_bits();
    int mid_index = key_size / 2;
    BlockPtr block(new Block(*this, NULL, 0, 0, mid_index));
    top_blocks.push_back(block);

    // Ask Alice what the correct parity of the selected block is.
    reconciliation.schedule_ask_correct_parity(block, false);

    // If the algorithm wants it, also create the complementary block and ask Alice for it's parity.
    if (reconciliation.get_algorithm().biconf_correct_complement) {
        BlockPtr complement_block(new Block(*this, NULL, 0, mid_index+1, key_size-1));
        top_blocks.push_back(complement_block);
        reconciliation.schedule_ask_correct_parity(complement_block, false);
    }
}

bool Iteration::try_correct_block(BlockPtr block, bool correct_right_sibling, bool cascade)
{
    DEBUG("Try to correct block " << block->compute_name());

    // If we don't know the correct parity of the block, we cannot make progress on this block
    // until Alice has told us what the correct parity is.
    if (!block->correct_parity_is_know_or_can_be_inferred()) {
        reconciliation.schedule_ask_correct_parity(block, correct_right_sibling);
        return false;
    }

    // If there is an even number of errors in this block, we don't attempt to fix any errors
    // in this block. But if asked to do so, we will attempt to fix an error in the right
    // sibling block.
    int error_parity = block->compute_error_parity();
    assert(error_parity != Block::unknown_parity);
    if (error_parity == 0) {
        if (correct_right_sibling) {
            DEBUG("Even error parity: try to correct right sibling");
            return try_correct_right_sibling_block(block, cascade);
        } else {
            DEBUG("Even error parity: do nothing");
            return false;
        }
    }

    // If this block contains a single bit, we have finished the recursion and found an error.
    // Correct the error by flipping the key bit that corresponds to this block.
    if (block->get_nr_bits() == 1) {
        int orig_key_bit_nr = shuffle.shuffle_to_orig(block->get_start_bit_nr());
        DEBUG("Correct single bit:" <<
              " shuffle_bit_nr=" << block->get_start_bit_nr() <<
              " orig_key_bit_nr=" << orig_key_bit_nr);
        reconciliation.correct_orig_key_bit(orig_key_bit_nr, iteration_nr, cascade);
        return true;              
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

bool Iteration::try_correct_right_sibling_block(BlockPtr block, bool cascade)
{
    Block* parent_block = block->get_parent_block();
    assert(parent_block);
    BlockPtr right_sibling_block = parent_block->get_right_sub_block();
    if (!right_sibling_block) {
        right_sibling_block = parent_block->create_right_sub_block();    
    }
    DEBUG("Right sibling is " << right_sibling_block->compute_name());
    return try_correct_block(right_sibling_block, false, cascade);
}

BlockPtr Iteration::get_cascade_block(int orig_key_bit_nr) const
{
    // TODO: Go deeper if re-use sub-blocks is enabled.

    int shuffle_key_bit_nr = shuffle.orig_to_shuffle(orig_key_bit_nr);

    DEBUG("Looking for cascading block:" <<
          " iteration=" << iteration_nr <<
          " orig_key_bit_nr=" << orig_key_bit_nr <<
          " shuffle_key_bit_nr=" << shuffle_key_bit_nr);

    int block_nr = shuffle_key_bit_nr / block_size;

    BlockPtr block;
    try {
        block = top_blocks.at(block_nr);
    }
    catch (const std::out_of_range&) {
        // Corner case: when running BICONF with cascading enabled and complementary blocks disabled
        // it is possible that iteration X corrects a bit, but that bit is not part of the chosen //// block in some other iteration Y.
        block = NULL;
    }

    DEBUG("Selected cascading block:"
          " block_nr=" << block_nr <<
          " iteration=" << block->get_iteration().iteration_nr <<
          " start_bit_nr=" << block->get_start_bit_nr() <<
          " end_bit_nr=" << block->get_end_bit_nr());

    return block;
}
