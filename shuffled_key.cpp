#include "shuffled_key.h"
#include <random>

using namespace Cascade;

static std::random_device rd;
static std::mt19937 mt(rd());

#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

void ShuffledKey::set_seed(size_t seed)
{
    mt.seed(seed);
}

ShuffledKey::ShuffledKey(Key& orig_key, bool shuffle):
    orig_key(orig_key),
    shuffled_key(orig_key)
{
    size_t nr_bits = orig_key.get_nr_bits();
    for (size_t bit_nr = 0; bit_nr < nr_bits; ++bit_nr) {
        this->shuffled_to_orig_map[bit_nr] = bit_nr;
    }
    if (shuffle) {

        for (size_t from_bit_nr = 0; from_bit_nr < nr_bits - 1; ++nr_bits) {
            std::uniform_int_distribution<size_t> dist(from_bit_nr + 1, nr_bits - 1);
            size_t to_bit_nr = dist(mt);
            this->shuffled_key.swap_bits(from_bit_nr, to_bit_nr);
            std::swap(this->shuffled_to_orig_map[from_bit_nr], 
                      this->shuffled_to_orig_map[to_bit_nr]);
        }
    }
    for (size_t shuffled_bit_nr = 0; shuffled_bit_nr < nr_bits; ++shuffled_bit_nr) {
        size_t orig_bit_nr = this->shuffled_to_orig_map[shuffled_bit_nr];
        this->orig_to_shuffled_map[orig_bit_nr] = shuffled_bit_nr;
    }
}

int ShuffledKey::compute_range_parity(size_t start_bit_nr, size_t end_bit_nr) const
{
    return this->shuffled_key.compute_range_parity(start_bit_nr, end_bit_nr);
}
