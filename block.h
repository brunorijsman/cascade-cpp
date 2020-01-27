#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>

namespace Cascade {

class ShuffledKey;

class Block
{
public:
    Block(ShuffledKey& shuffled_key, size_t start_bit_nr, size_t end_bit_nr);
    int compute_current_parity() const;
private:
    ShuffledKey& shuffled_key;
    size_t start_bit_nr;
    size_t end_bit_nr;
};

} /* namespace Cascade */

#endif /* ifndef BLOCK_H */
