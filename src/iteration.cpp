#include "iteration.h"
#include "algorithm.h"
#include "reconciliation.h"
#include <algorithm>
#include <assert.h>
#include <random>

#include <iostream>  //@@@

using namespace Cascade;

Iteration::Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf),
    nr_key_bits(reconciliation.get_reconciled_key().get_nr_bits()),
    shuffle(this->nr_key_bits, iteration_nr == 1),
    shuffled_key(reconciliation.get_reconciled_key(), this->shuffle)
{
    std::cout << "Iteration::Iteration " << std::endl;  //@@@
}

Iteration::~Iteration()
{
    std::cout << "Iteration::~Iteration " << std::endl;  //@@@
}

Reconciliation& Iteration::get_reconciliation() const
{
    return this->reconciliation;
}

unsigned Iteration::get_iteration_nr() const
{
    return this->iteration_nr;
}

bool Iteration::get_biconf() const
{
    return this->biconf;
}

const Shuffle& Iteration::get_shuffle() const
{
    return this->shuffle;    
}

const Key& Iteration::get_shuffled_key() const
{
    return this->shuffled_key;
}

void Iteration::reconcile()
{
    if (this->biconf) {
        this->reconcile_biconf();
    } else {
        this->reconcile_cascade();
    }
}

void Iteration::flip_orig_key_bit(size_t orig_key_bit_nr)
{
    size_t shuffle_key_bit_nr = this->shuffle.orig_to_shuffle(orig_key_bit_nr);
    this->shuffled_key.flip_bit(shuffle_key_bit_nr);
}

void Iteration::reconcile_cascade()
{
    // Create top blocks, and schedule each one for "ask correct parity".
    double estimated_bit_error_rate = this->reconciliation.get_estimated_bit_error_rate();
    const Algorithm& algorithm = this->reconciliation.get_algorithm();
    size_t block_size = algorithm.block_size_function(this->iteration_nr, estimated_bit_error_rate);
    size_t block_nr = 0;
    size_t start_bit_nr = 0;
    size_t nr_key_bits = this->shuffled_key.get_nr_bits();
    while (start_bit_nr < nr_key_bits) {
        size_t end_bit_nr = std::min(start_bit_nr + block_size, nr_key_bits) - 1;
        BlockPtr block(new Block(*this, NULL, block_nr, start_bit_nr, end_bit_nr));
        this->top_blocks.push_back(block);
        this->reconciliation.schedule_ask_correct_parity(block, false);
        block_nr += 1;
        start_bit_nr += block_size;
    }
}

void Iteration::reconcile_biconf()
{
}

bool Iteration::try_correct_block(BlockPtr block, bool correct_right_sibling, bool cascade)
{
    std::cout << "Try correct " << block->compute_name() << " (cascade=" << cascade << ")" << std::endl;

    // If we don't know the correct parity of the block, we cannot make progress on this block
    // until Alice has told us what the correct parity is.
    if (!block->correct_parity_is_know_or_can_be_inferred()) {
        this->reconciliation.schedule_ask_correct_parity(block, correct_right_sibling);
        return false;
    }

    // If there is an even number of errors in this block, we don't attempt to fix any errors
    // in this block. But if asked to do so, we will attempt to fix an error in the right
    // sibling block.
    int error_parity = block->compute_error_parity();
    assert(error_parity != Block::unknown_parity);
    if (error_parity == 0) {
        if (correct_right_sibling) {
            std::cout << "- Even error parity; try correct right sibling" << std::endl;
            return this->try_correct_right_sibling_block(block, cascade);
        } else {
            std::cout << "- Even error parity; do nothing" << std::endl;
            return false;
        }
    }

    // If this block contains a single bit, we have finished the recursion and found an error.
    // Correct the error by flipping the key bit that corresponds to this block.
    if (block->get_nr_bits() == 1) {
        size_t orig_key_bit_nr = this->shuffle.shuffle_to_orig(block->get_start_bit_nr());
        std::cout << "- Correct bit " << orig_key_bit_nr << std::endl;
        this->reconciliation.flip_orig_key_bit(orig_key_bit_nr);
        // @@@ Cascade effect???
        return true;
    }

    // If we get here, it means that there is an odd number of errors in this block and that the
    // block is bigger than 1 bit.
    std::cout << "- Recurse into left sub-block" << std::endl;
    BlockPtr left_sub_block = block->get_left_sub_block();
    if (!left_sub_block) {
        left_sub_block = block->create_left_sub_block();    
    }
    return this->try_correct_block(left_sub_block, true, cascade);
}

bool Iteration::try_correct_right_sibling_block(BlockPtr block, bool cascade)
{
    Block* parent_block = block->get_parent_block();
    if (!parent_block) {
        std::cout << "- no parent???" << std::endl;
        return false;
    }
    BlockPtr right_sibling_block = parent_block->get_right_sub_block();
    if (!right_sibling_block) {
        right_sibling_block = parent_block->create_right_sub_block();    
    }
    std::cout << "- right sibling is " << right_sibling_block->compute_name() << std::endl;
    return this->try_correct_block(right_sibling_block, false, cascade);
}