#ifndef SHUFFLE_H
#define SHUFFLE_H

#include <algorithm>
#include <memory>
#include <stdint.h>
#include <vector>

namespace Cascade {

class Shuffle;

typedef std::shared_ptr<Shuffle> ShufflePtr;

class Shuffle
{
public:
    static ShufflePtr new_random_shuffle(int iteration_nr, int nr_bits, bool assign_seed,
                                         bool cache);
    static ShufflePtr new_shuffle_from_seed(int iteration_nr, int nr_bits, uint64_t seed,
                                            bool);
    ~Shuffle();
    uint64_t get_seed() const;
    int get_nr_bits() const;
    int orig_to_shuffle(int orig_bit_nr) const;
    int shuffle_to_orig(int shuffle_bit_nr) const;
private:
    Shuffle(int iteration_nr, int nr_bits, bool assign_seed);
    Shuffle(int iteration_nr, int nr_bits, uint64_t seed);
    void initialize(bool assign_seed);
    static ShufflePtr cache_search(int iteration_nr, int nr_bits, bool has_seed);
    static void cache_add(ShufflePtr shuffle);
    int iteration_nr;
    int nr_bits;
    bool has_seed;
    uint64_t seed;
    typedef std::vector<int> BitMap;
    BitMap shuffled_to_orig_map;
    BitMap orig_to_shuffled_map;
};

} /* namespace Cascade */

#endif /* ifndef SHUFFLE_H */
