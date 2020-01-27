#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>
#include <string>

namespace Cascade {

class ShuffledKey;

class Block
{
public:
    Block(std::string name, ShuffledKey& shuffled_key, size_t start_bit_nr, size_t end_bit_nr);
    std::string get_name(void) const;
    int compute_current_parity() const;
private:
    std::string name;
    ShuffledKey& shuffled_key;
    size_t start_bit_nr;
    size_t end_bit_nr;
};

} /* namespace Cascade */

#endif /* ifndef BLOCK_H */
