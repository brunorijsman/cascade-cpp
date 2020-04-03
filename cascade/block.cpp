#include "block.h"
#include "debug.h"
#include "iteration.h"
#include "reconciliation.h"
#include "shuffled_key.h"
#include <assert.h>

using namespace Cascade;

const int Block::unknown_parity = -1;

Block::Block(Iteration& iteration, int start_bit_nr, int end_bit_nr, Block* parent_block,
             int block_nr):
    iteration(iteration),
    shuffled_key(iteration.get_shuffled_key()),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr),
    current_parity(Block::unknown_parity),
    correct_parity(Block::unknown_parity),
    parent_block(parent_block),
    block_nr(block_nr),
    left_sub_block(NULL),
    right_sub_block(NULL)
{
    DEBUG("Create Block " << debug_str());
}

Block::~Block()
{
    DEBUG("Destroy Block " << debug_str());
}

Iteration& Block::get_iteration() const
{
    return iteration;
}

int Block::get_nr_bits() const
{
    return end_bit_nr - start_bit_nr + 1;
}

int Block::get_start_bit_nr() const
{
    return start_bit_nr;
}

int Block::get_end_bit_nr() const
{
    return end_bit_nr;
}

std::string Block::compute_name() const
{
    std::string name;
    if (parent_block) {
        name = parent_block->compute_name();
        if (block_nr == 0) {
            name += "L";
        } else {
            name += "R";
        }
        return name;
    }
    if (iteration.get_biconf()) {
        name = "b:";
    } else {
        name = "c:";
    }
    name += std::to_string(iteration.get_iteration_nr()) + ":" + 
            std::to_string(block_nr);
    return name;
}

std::string Block::debug_str() const
{
    const Key* correct_key = iteration.get_reconciliation().get_correct_key();
    std::string str = compute_name() + "[";
    for (int bit_nr = start_bit_nr; bit_nr <= end_bit_nr; ++bit_nr) {
        int current_bit = shuffled_key.get_bit(bit_nr);
        if (correct_key) {
            int orig_bit_nr = shuffled_key.get_shuffle().shuffle_to_orig(bit_nr);
            int orig_bit = correct_key->get_bit(orig_bit_nr);
            if (current_bit == orig_bit)
                str += ANSI_GREEN;
            else
                str += ANSI_RED;
        } else {
            // Correctness of key bit is unknown
            str += ANSI_BLUE;
        }
        str += shuffled_key.get_bit(bit_nr) ? "1" : "0";
        str += ANSI_RESET;
    }
    str += "]";
    return str;
}

int Block::get_correct_parity()
{
    return correct_parity;
}

int Block::get_or_compute_current_parity()
{
    const char* action;
    if (current_parity == Block::unknown_parity) {
        action = "Compute";
        current_parity = shuffled_key.compute_range_parity(start_bit_nr, end_bit_nr);
    } else {
        action = "Get";
    }
    DEBUG(action << " current parity:" <<
        " block=" << debug_str() <<
        " current_parity=" << current_parity);
    return current_parity;
}

void Block::flip_current_parity()
{
    if (current_parity == Block::unknown_parity) {
        // We can get here in a valid but rare race condition. This block is pending for its first
        // try-correct (so the current parity is unknown) when some other block containing the same
        // bit had a single bit correction. It that case we leave this block alone (its current
        // parity will be computed in due time).
        return;
    }
    current_parity = 1 - current_parity;
    DEBUG("Flips current parity: block=" << debug_str() << 
          " new_current_parity=" << current_parity);
}

void Block::set_correct_parity(int parity)
{
    correct_parity = parity;
}

bool Block::correct_parity_is_known() const
{
    return correct_parity != unknown_parity;
}

bool Block::try_to_infer_correct_parity()
{
    assert(correct_parity == unknown_parity);

    // Try to do a very limited type of inference, using only the parity of the parent block and
    // the sibling block.

    // Cannot infer if there is no parent block.
    if (!parent_block)
        return false;

    // Cannot infer if there is no sibling block (yet).
    BlockPtr sibling_block;
    if (parent_block->left_sub_block.get() == this)
        sibling_block = parent_block->right_sub_block;
    else
        sibling_block = parent_block->left_sub_block;
    if (!sibling_block)
        return false;

    // Cannot infer if the correct parity of the parent or sibling block is unknown.
    if (parent_block->correct_parity == Block::unknown_parity)
        return false;
    if (sibling_block->correct_parity == Block::unknown_parity)
        return false;

    // We have everything we need. Infer the correct parity.
    int correct_block_parity;
    if (parent_block->correct_parity == 1)
        correct_block_parity = 1 - sibling_block->correct_parity;
    else
        correct_block_parity = sibling_block->correct_parity;
    set_correct_parity(correct_block_parity);
    return true;
}

int Block::get_error_parity()
{
    assert(correct_parity != unknown_parity);
    int current_parity = get_or_compute_current_parity();
    int error_parity;
    if (correct_parity == current_parity) {
        error_parity = 0;
    } else {
        error_parity = 1;
    }
    DEBUG("Compute error parity:" <<
          " block=" << debug_str() <<  
          " current_parity=" << current_parity <<
          " correct_parity=" << correct_parity <<
          " error_parity=" << error_parity);
    return error_parity;
}

Block* Block::get_parent_block() const
{
    return parent_block;
}

BlockPtr Block::get_left_sub_block() const
{
    return left_sub_block;
}

BlockPtr Block::create_left_sub_block()
{
    int sub_start_bit_nr = start_bit_nr;
    int sub_end_bit_nr = sub_start_bit_nr + (get_nr_bits() / 2) - 1;
    BlockPtr block(new Block(iteration, sub_start_bit_nr, sub_end_bit_nr, this, 0));
    left_sub_block = block;
    return block;
}

BlockPtr Block::get_right_sub_block() const
{
    return right_sub_block;
}

BlockPtr Block::create_right_sub_block()
{
    int sub_start_bit_nr = start_bit_nr + (get_nr_bits() / 2);
    int sub_end_bit_nr = end_bit_nr;
    BlockPtr block(new Block(iteration, sub_start_bit_nr, sub_end_bit_nr, this, 1));
    right_sub_block = block;
    return block;
}

long Block::encoded_bits() const
{
    return 16 +     // 16 bits for iteration nr
           32 +     // 32 bits for start bit index
           32;      // 32 bits for end bit index
}
