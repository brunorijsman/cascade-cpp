#include "shuffle.h"
#include "random.h"
#include <assert.h>
#include <map>
#include <mutex>
#include <random>

using namespace Cascade;

static std::random_device rd;
static std::mt19937 global_mt(rd());

// Creating a new shuffle is expensive in terms of CPU usage, so optionally keep a cache of
// previously created shuffles.

struct ShuffleIndex {
    ShuffleIndex(int iteration_nr, int nr_bits);
    int iteration_nr;
    int nr_bits;
};

ShuffleIndex::ShuffleIndex(int iteration_nr, int nr_bits):
    iteration_nr(iteration_nr),
    nr_bits(nr_bits)
{
}

bool operator<(const ShuffleIndex& lhs, const ShuffleIndex& rhs) {
    if (lhs.iteration_nr < rhs.iteration_nr)
        return true;
    if (lhs.iteration_nr > rhs.iteration_nr)
        return false;
    if (lhs.nr_bits < rhs.nr_bits)
        return true;
    return false;
}

typedef std::map<ShuffleIndex, ShufflePtr> ShuffleCache;

static ShuffleCache cache;

static std::mutex cache_mutex;

ShufflePtr Shuffle::new_random_shuffle(int iteration_nr, int nr_bits, bool assign_seed, bool cache)
{
    std::lock_guard<std::mutex> guard(cache_mutex);
    ShufflePtr shuffle;
    if (cache)
        shuffle = cache_search(iteration_nr, nr_bits);
    if (shuffle)
        return shuffle;
    shuffle = ShufflePtr(new Shuffle(iteration_nr, nr_bits, assign_seed));
    if (cache)
        cache_add(shuffle);
    return shuffle;
}

ShufflePtr Shuffle::new_shuffle_from_seed(int iteration_nr, int nr_bits, uint64_t seed, bool cache)
{
    std::lock_guard<std::mutex> guard(cache_mutex);
    ShufflePtr shuffle;
    if (cache)
        shuffle = cache_search(iteration_nr, nr_bits);
    if (shuffle)
        return shuffle;
    shuffle = ShufflePtr(new Shuffle(iteration_nr, nr_bits, seed));
    if (cache)
        cache_add(shuffle);
    return shuffle;
}

Shuffle::Shuffle(int iteration_nr, int nr_bits, bool assign_seed):
    iteration_nr(iteration_nr),
    nr_bits(nr_bits),
    has_seed(false),
    seed(0),
    orig_to_shuffled_map(nr_bits, -1)
{
    initialize(assign_seed);
}

Shuffle::Shuffle(int iteration_nr, int nr_bits, uint64_t seed):
    iteration_nr(iteration_nr),
    nr_bits(nr_bits),
    has_seed(true),
    seed(seed),
    orig_to_shuffled_map(nr_bits, -1)
{
    initialize(false);
}

void Shuffle::initialize(bool assign_seed)
{
    // The initial mapping of shuffled key bits to original key bits is one-on-one.
    shuffled_to_orig_map.reserve(nr_bits);
    for (int bit_nr = 0; bit_nr < nr_bits; ++bit_nr) {
        shuffled_to_orig_map.push_back(bit_nr);
    }

    // Shuffle (except in iteration 1).
    if (iteration_nr != 1) {
        if (assign_seed) {
            assert(!has_seed);
            has_seed = true;
            seed = random_uint64();
        }
        if (has_seed) {
            std::mt19937 local_mt(rd());
            local_mt.seed(seed);
            std::shuffle(std::begin(shuffled_to_orig_map),
                         std::end(shuffled_to_orig_map),
                         local_mt);
        } else {
            // If we are not using a seed avoid constructing a local random generator because it
            // is expensive (performance wise).
            std::shuffle(std::begin(shuffled_to_orig_map),
                         std::end(shuffled_to_orig_map),
                         global_mt);
        }
    }

    // Compute the reverse mapping of original key bits to shuffled key bits.
    orig_to_shuffled_map.reserve(nr_bits);
    for (int shuffled_bit_nr = 0; shuffled_bit_nr < nr_bits; ++shuffled_bit_nr) {
        int orig_bit_nr = shuffled_to_orig_map[shuffled_bit_nr];
        orig_to_shuffled_map[orig_bit_nr] = shuffled_bit_nr;
    }
}

Shuffle::~Shuffle()
{
}

ShufflePtr Shuffle::cache_search(int iteration_nr, int nr_bits)
{
    ShuffleIndex index(iteration_nr, nr_bits);
    ShuffleCache::iterator it = cache.find(index);
    if (it == cache.end())
        return NULL;
    else
        return it->second;
}

void Shuffle::cache_add(ShufflePtr shuffle)
{
    ShuffleIndex index = {shuffle->iteration_nr, shuffle->nr_bits};
    cache[index] = shuffle;
}

uint64_t Shuffle::get_seed() const
{
    return seed;
}

int Shuffle::get_nr_bits() const
{
    return nr_bits;
}

int Shuffle::orig_to_shuffle(int orig_bit_nr) const
{
    return orig_to_shuffled_map.at(orig_bit_nr);
}

int Shuffle::shuffle_to_orig(int shuffle_bit_nr) const
{
    return shuffled_to_orig_map.at(shuffle_bit_nr);
}
