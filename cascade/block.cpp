#include "block.h"
#include "debug.h"
#include "iteration.h"
#include "reconciliation.h"

#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

using namespace Cascade;

const int Block::unknown_parity = -1;

Block::Block(Iteration& iteration_param, Block* parent_block, int block_nr_param,
             int start_bit_nr_param, int end_bit_nr_param):
    iteration(iteration_param),
    block_nr(block_nr_param),
    start_bit_nr(start_bit_nr_param),
    end_bit_nr(end_bit_nr_param),
    correct_parity(Block::unknown_parity),
    parent_block(parent_block),
    left_sub_block(NULL),
    right_sub_block(NULL)

{
    DEBUG("Create block " << compute_name());
}

Block::~Block()
{
    DEBUG("Destroy block " << compute_name());
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

int Block::get_nr_bits() const
{
    return end_bit_nr - start_bit_nr + 1;
}

Iteration& Block::get_iteration() const
{
    return iteration;
}

int Block::get_iteration_nr() const
{
    return iteration.get_iteration_nr();
}

int Block::get_start_bit_nr() const
{
    return start_bit_nr;
}

int Block::get_end_bit_nr() const
{
    return end_bit_nr;
}

int Block::compute_current_parity() const
{
    const Key& shuffled_key = iteration.get_shuffled_key();
    int shuffled_start_bit_nr = start_bit_nr;
    int shuffled_end_bit_nr = end_bit_nr;
    int parity = shuffled_key.compute_range_parity(shuffled_start_bit_nr, shuffled_end_bit_nr);
    DEBUG("Bob computes current parity:" <<
          " iteration_nr=" << iteration.get_iteration_nr() <<
          " shuffled_key=" << shuffled_key.to_string() <<
          " shuffled_start_bit_nr=" << shuffled_start_bit_nr <<
          " shuffled_end_bit_nr=" << shuffled_end_bit_nr <<
          " parity=" << parity);
    return parity;
}

int Block::compute_parity_for_key(const Key& shuffled_key) const
{
    int shuffled_start_bit_nr = start_bit_nr;
    int shuffled_end_bit_nr = end_bit_nr;
    int parity = shuffled_key.compute_range_parity(shuffled_start_bit_nr, shuffled_end_bit_nr);
    DEBUG("Alice computes correct parity:" <<
          " iteration_nr=" << iteration.get_iteration_nr() <<
          " shuffled_key=" << shuffled_key.to_string() <<
          " shuffled_start_bit_nr=" << shuffled_start_bit_nr <<
          " shuffled_end_bit_nr=" << shuffled_end_bit_nr <<
          " parity=" << parity);
    return parity;
}

void Block::set_correct_parity(int parity)
{
    correct_parity = parity;
}

int Block::compute_error_parity() const
{
    if (correct_parity == Block::unknown_parity) {
        return Block::unknown_parity;
    }
    int current_parity = compute_current_parity();
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

bool Block::correct_parity_is_know_or_can_be_inferred()
{
    if (correct_parity != Block::unknown_parity) {
        return true;
    }
    // TODO: Implement correct parity inference
    return false;
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
    int block_nr = 0;
    int sub_start_bit_nr = start_bit_nr;
    int sub_end_bit_nr = sub_start_bit_nr + (get_nr_bits() / 2) - 1;
    BlockPtr block(new Block(iteration, this, block_nr, sub_start_bit_nr, sub_end_bit_nr));
    left_sub_block = block;
    return block;
}

BlockPtr Block::get_right_sub_block() const
{
    return right_sub_block;
}

BlockPtr Block::create_right_sub_block()
{
    int block_nr = 1;
    int sub_start_bit_nr = start_bit_nr + (get_nr_bits() / 2);
    int sub_end_bit_nr = end_bit_nr;
    BlockPtr block(new Block(iteration, this, block_nr, sub_start_bit_nr, sub_end_bit_nr));
    right_sub_block = block;
    return block;
}

long Block::encoded_bits() const
{
    return 16 +     // 16 bits for iteration nr
           32 +     // 32 bits for start bit index
           32;      // 32 bits for end bit index
}

