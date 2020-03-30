#include "block.h"
#include "debug.h"
#include "iteration.h"
#include "shuffled_key.h"

using namespace Cascade;

const int Block::unknown_parity = -1;

Block::Block(Iteration& iteration, int start_bit_nr, int end_bit_nr, Block* parent_block,
             const std::string& name):
    iteration(iteration),
    shuffled_key(iteration.get_shuffled_key()),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr),
    current_parity(Block::unknown_parity),
    correct_parity(Block::unknown_parity),
    parent_block(parent_block),
    name(name),
    left_sub_block(NULL),
    right_sub_block(NULL)
{
    DEBUG("Create block " << name);
}

Iteration& Block::get_iteration() const
{
    return iteration;
}

Block::~Block()
{
    DEBUG("Destroy block " << name);
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

const std::string& Block::get_name() const
{
    return name;
}

int Block::get_current_parity()
{
    const char* action;
    if (current_parity == Block::unknown_parity) {
        action = "Compute";
        current_parity = shuffled_key.compute_range_parity(start_bit_nr, end_bit_nr);
    } else {
        action = "Get";
    }
    DEBUG(action << " current parity:" <<
        " block_name=" << name <<
        " shuffled_key=" << shuffled_key.to_string() <<
        " start_bit_nr=" << start_bit_nr <<
        " end_bit_nr=" << end_bit_nr <<
        " current_parity=" << current_parity);
    return current_parity;
}

void Block::flip_current_parity()
{
    assert(current_parity != Block::unknown_parity);
    current_parity = 1 - current_parity;
    DEBUG("Bob flips current parity" <<
          " block_name=" << name <<
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
    int current_parity = get_current_parity();
    int error_parity;
    if (correct_parity == current_parity) {
        error_parity = 0;
    } else {
        error_parity = 1;
    }
    DEBUG("Compute error parity:" <<
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
    BlockPtr block(new Block(iteration, sub_start_bit_nr, sub_end_bit_nr, this, name + "L"));
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
    BlockPtr block(new Block(iteration, sub_start_bit_nr, sub_end_bit_nr, this, name + "R"));
    right_sub_block = block;
    return block;
}

long Block::encoded_bits() const
{
    return 16 +     // 16 bits for iteration nr
           32 +     // 32 bits for start bit index
           32;      // 32 bits for end bit index
}
