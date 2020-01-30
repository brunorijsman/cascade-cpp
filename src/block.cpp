#include "block.h"
#include "iteration.h"
#include "reconciliation.h"

#include <iostream> //@@@
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

using namespace Cascade;

const int Block::unknown_parity = -1;

Block::Block(Iteration& iteration, Block* parent_block, unsigned block_nr, size_t start_bit_nr,
             size_t end_bit_nr):
    iteration(iteration),
    block_nr(block_nr),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr),
    correct_parity(Block::unknown_parity),
    parent_block(parent_block),
    left_sub_block(NULL),
    right_sub_block(NULL)

{
    std::cout << "Create block " << this->compute_name() << std::endl;
}

Block::~Block()
{
    std::cout << "Destroy block " << this->compute_name() << std::endl;
}

std::string Block::compute_name() const
{
    std::string name;
    if (parent_block) {
        name = parent_block->compute_name();
        if (this->block_nr == 0) {
            name += "L";
        } else {
            name += "R";
        }
        return name;
    }
    if (this->iteration.get_biconf()) {
        name = "b:";
    } else {
        name = "c:";
    }
    name += std::to_string(this->iteration.get_iteration_nr()) + ":" + 
            std::to_string(this->block_nr);
    return name;
}

size_t Block::get_nr_bits() const
{
    return this->end_bit_nr - this->start_bit_nr + 1;
}

Iteration& Block::get_iteration() const
{
    return this->iteration;
}

unsigned Block::get_iteration_nr() const
{
    return this->iteration.get_iteration_nr();
}

size_t Block::get_start_bit_nr() const
{
    return this->start_bit_nr;
}

size_t Block::get_end_bit_nr() const
{
    return this->end_bit_nr;
}

int Block::compute_current_parity() const
{
    const Key& shuffled_key = this->iteration.get_shuffled_key();
    return shuffled_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}

int Block::compute_parity_for_key(const Key& correct_key) const
{
    return correct_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}

void Block::set_correct_parity(int parity)
{
    this->correct_parity = parity;
}

int Block::compute_error_parity() const
{
    if (this->correct_parity == Block::unknown_parity) {
        return Block::unknown_parity;
    }
    if (this->correct_parity == this->compute_current_parity()) {
        return 0;
    } else {
        return 1;
    }
}

bool Block::correct_parity_is_know_or_can_be_inferred()
{
    if (this->correct_parity != Block::unknown_parity) {
        return true;
    }
    // TODO: Implement correct parity inference
    return false;
}


Block* Block::get_parent_block() const
{
    return this->parent_block;
}

BlockPtr Block::get_left_sub_block() const
{
    return this->left_sub_block;
}

BlockPtr Block::create_left_sub_block()
{
    unsigned block_nr = 0;
    size_t start_bit_nr = this->start_bit_nr;
    size_t end_bit_nr = start_bit_nr + (this->get_nr_bits() / 2) - 1;
    BlockPtr block(new Block(this->iteration, this, block_nr, start_bit_nr, end_bit_nr));
    this->left_sub_block = block;
    return block;
}

BlockPtr Block::get_right_sub_block() const
{
    return this->right_sub_block;
}

BlockPtr Block::create_right_sub_block()
{
    unsigned block_nr = 1;
    size_t start_bit_nr = this->start_bit_nr + (this->get_nr_bits() / 2);
    size_t end_bit_nr = this->end_bit_nr;
    BlockPtr block(new Block(this->iteration, this, block_nr, start_bit_nr, end_bit_nr));
    this->right_sub_block = block;
    return block;
}
