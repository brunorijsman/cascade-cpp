#ifndef SHUFFLE_H
#define SHUFFLE_H

#include <map>

namespace Cascade {

class Key;

class Shuffle
{
public:
    Shuffle(int nr_bits, bool identity);
    Shuffle(int nr_bits, bool identity, uint64_t seed);
    ~Shuffle();
    uint64_t get_seed() const;
    int orig_to_shuffle(int orig_bit_nr) const;
    int shuffle_to_orig(int shuffle_bit_nr) const;
private:
    int nr_bits;
    bool identity;
    uint64_t seed;
    typedef std::map<int, int> BitMap;
    BitMap shuffled_to_orig_map;
    BitMap orig_to_shuffled_map;
};

} /* namespace Cascade */

#endif /* ifndef SHUFFLE_H */
