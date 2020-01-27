#include "block.h"
#include "shuffled_key.h"

#include <iostream> //@@@
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

using namespace Cascade;

Block::Block(std::string name, Iteration& iteration, size_t start_bit_nr, size_t end_bit_nr):
    name(name),
    iteration(iteration),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr)
{
    std::cout << "Create block " << this << " " << name << std::endl;
}

Block::~Block()
{
    std::cout << "Destroy block " << this << " " << name << std::endl;
}

std::string Block::get_name(void) const
{
    return this->name;
}

int Block::compute_current_parity() const
{
    // return this->shuffled_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
    //@@@
    return 0;
}
