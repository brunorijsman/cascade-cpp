#include "shuffle.h"
#include "random.h"
#include <random>

using namespace Cascade;

static std::random_device rd;

Shuffle::Shuffle(int nr_bits, bool identity):
    Shuffle(nr_bits, identity, random_uint64())
{
}

Shuffle::Shuffle(int nr_bits_param, bool identity_param, uint64_t seed):
    nr_bits(nr_bits_param),
    identity(identity_param),
    seed(seed),
    orig_to_shuffled_map(nr_bits, -1)
{
    // The initial mapping of shuffled key bits to original key bits is one-on-one.
    shuffled_to_orig_map.reserve(nr_bits);
    for (int bit_nr = 0; bit_nr < nr_bits; ++bit_nr) {
        shuffled_to_orig_map.push_back(bit_nr);
    }

    // Shuffle.
    if (!identity) {
        std::mt19937 mt(rd());
        mt.seed(seed);
        std::shuffle(std::begin(shuffled_to_orig_map), std::end(shuffled_to_orig_map), mt);
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