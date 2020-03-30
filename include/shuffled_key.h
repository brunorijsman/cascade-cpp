#ifndef SHUFFLED_KEY_H
#define SHUFFLED_KEY_H

#include <string>
#include "shuffle.h"

namespace Cascade {

class Key;

class ShuffledKey 
{
public:
    ShuffledKey(Key& key, Shuffle& shuffle);
    std::string to_string() const;
    int get_nr_bits() const;
    int get_bit(int bit_nr) const;
    void set_bit(int bit_nr, int value);
    void flip_bit(int bit_nr);
    int compute_range_parity(int start_bit_nr, int end_bit_nr) const;
    const Shuffle& get_shuffle() const;
private:
    Key& key;
    Shuffle shuffle;
};

typedef std::shared_ptr<ShuffledKey> ShuffledKeyPtr;

} /* namespace Cascade */

#endif /* ifndef SHUFFLED_KEY_H */
