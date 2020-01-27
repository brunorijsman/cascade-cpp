#ifndef SHUFFLED_KEY_H
#define SHUFFLED_KEY_H

#include "key.h"
#include <map>
#include <stddef.h>

namespace Cascade {

class ShuffledKey 
{
public:
    static void set_seed(size_t seed);
    ShuffledKey(Key& orig_key, bool shuffle);
    int compute_range_parity(size_t start_bit_nr, size_t end_bit_nr) const;
private:
    Key& orig_key;
    Key shuffled_key;
    typedef std::map<unsigned, unsigned> BitMap;
    BitMap shuffled_to_orig_map;
    BitMap orig_to_shuffled_map;
};

} /* namespace Cascade */

#endif /* ifndef SHUFFLED_KEY_H */
