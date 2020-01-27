#include "block.h"
#include "shuffled_key.h"

using namespace Cascade;

Block::Block(ShuffledKey& shuffled_key, size_t start_bit_nr, size_t end_bit_nr):
    shuffled_key(shuffled_key),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr)
{
}

int Block::compute_current_parity() const
{
    return this->shuffled_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}
