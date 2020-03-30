#include "shuffled_key.h"
#include "key.h"
#include "shuffle.h"

using namespace Cascade;

ShuffledKey::ShuffledKey(Key& key, Shuffle& shuffle):
    key(key),
    shuffle(shuffle)
{
}

std::string ShuffledKey::to_string() const
{
    std::string string = "";
    int bit_nr = 0;
    while (bit_nr < get_nr_bits()) {
        string += get_bit(bit_nr) ? "1" : "0";
        bit_nr += 1;    
    }
    return string;
}

int ShuffledKey::get_nr_bits() const
{
    return key.get_nr_bits();
}

int ShuffledKey::get_bit(int bit_nr) const
{
    int orig_bit_nr = shuffle.shuffle_to_orig(bit_nr);
    return key.get_bit(orig_bit_nr);
}

void ShuffledKey::set_bit(int bit_nr, int value)
{
    int orig_bit_nr = shuffle.shuffle_to_orig(bit_nr);
    key.set_bit(orig_bit_nr, value);
}

void ShuffledKey::flip_bit(int bit_nr)
{
    int orig_bit_nr = shuffle.shuffle_to_orig(bit_nr);
    key.flip_bit(orig_bit_nr);
}

int ShuffledKey::compute_range_parity(int start_bit_nr, int end_bit_nr) const
{
    int parity = 0;
    for (int bit_nr = start_bit_nr; bit_nr <= end_bit_nr; ++bit_nr) {
        int orig_bit_nr = shuffle.shuffle_to_orig(bit_nr);
        parity = 1 - key.get_bit(orig_bit_nr);
    }
    return parity;
}

