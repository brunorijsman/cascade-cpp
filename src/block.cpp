#include "block.h"
#include "iteration.h"

#include <iostream> //@@@
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

using namespace Cascade;

Block::Block(Iteration& iteration, unsigned block_nr, size_t start_bit_nr, size_t end_bit_nr):
    iteration(iteration),
    block_nr(block_nr),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr)
{
    std::cout << "Create block " << this->get_name() << std::endl;
}

Block::~Block()
{
    std::cout << "Destroy block " << this->get_name() << std::endl;
}

std::string Block::get_name() const
{
    std::string name;
    if (this->iteration.get_biconf()) {
        name = "b:";
    } else {
        name = "c:";
    }
    name += std::to_string(this->iteration.get_iteration_nr()) + ":" + 
            std::to_string(this->block_nr);
    return name;
}

int Block::compute_current_parity() const
{
    const Key& shuffled_key = this->iteration.get_shuffled_key();
    return shuffled_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}
