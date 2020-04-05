#include "shuffle.h"
#include "random.h"
#include <random>

using namespace Cascade;

static std::random_device rd;
static std::mt19937 global_mt(rd());

Shuffle::Shuffle(int nr_bits, bool identity, bool assign_seed):
    nr_bits(nr_bits),
    identity(identity),
    has_seed(false),
    seed(0),
    orig_to_shuffled_map(nr_bits, -1)
{
    initialize(assign_seed);
}

Shuffle::Shuffle(int nr_bits, bool identity, uint64_t seed):
    nr_bits(nr_bits),
    identity(identity),
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

    // Shuffle.
    if (!identity) {
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

uint64_t Shuffle::get_seed() const
{
    return seed;
}

int Shuffle::orig_to_shuffle(int orig_bit_nr) const
{
    return orig_to_shuffled_map.at(orig_bit_nr);
}

int Shuffle::shuffle_to_orig(int shuffle_bit_nr) const
{
    return shuffled_to_orig_map.at(shuffle_bit_nr);
}