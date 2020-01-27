#ifndef SHUFFLE_H
#define SHUFFLE_H

#include <map>

namespace Cascade {

class Key;

class Shuffle
{
public:
    Shuffle(size_t nr_bits, bool identity);
    Shuffle(size_t nr_bits, bool identity, uint64_t seed);
    ~Shuffle();
    size_t orig_to_shuffle(size_t orig_bit_nr) const;
    size_t shuffle_to_orig(size_t shuffle_bit_nr) const;
private:
    size_t nr_bits;
    bool identity;
    uint64_t seed;
    typedef std::map<unsigned, unsigned> BitMap;
    BitMap shuffled_to_orig_map;
    BitMap orig_to_shuffled_map;
};

} /* namespace Cascade */

#endif /* ifndef SHUFFLE_H */
