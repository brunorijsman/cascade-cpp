#include "block.h"
#include "iteration.h"

#include <iostream> //@@@
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

using namespace Cascade;

Block::Block(std::string name, Iteration& iteration, size_t start_bit_nr, size_t end_bit_nr):
    name(name),
    iteration(iteration),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr)
{
    std::cout << "Create block " << " " << name << std::endl;
}

Block::~Block()
{
    std::cout << "Destroy block " << " " << name << std::endl;
}

std::string Block::get_name(void) const
{
    return this->name;
}

int Block::compute_current_parity() const
{
    const Key& shuffled_key = this->iteration.get_shuffled_key();
    return shuffled_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}
