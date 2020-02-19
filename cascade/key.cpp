#include "key.h"
#include "random.h"
#include "shuffle.h"
#include <assert.h>
#include <cstring>
#include <iostream>
#include <unordered_set>

using namespace Cascade;

// Key bits are stored into 64-bit int "words" as follows:
//
// +----+----+----+-//-+----+----+----+   +----+----+----+-//-+----+----+----+   +---...
// | 63 | 62 | 61 |    |  2 |  1 |  0 |   | 127| 126| 125|    | 66 | 65 | 64 |   |
// +----+----+----+-//-+----+----+----+   +----+----+----+-//-+----+----+----+   +---...
//   63   62   61         2    1    0       63   62   61         2    1    0       
//   MSB                          LSB       MSB                          LSB
//                Word 0                                 Word 1
//
// Note: we don't use the more natural term "block" instead of "word" to avoid confusion with
//       cascade blocks.

static uint64_t start_word_mask(int start_bit_nr)
{
    int nr_unused_bits = start_bit_nr % 64;
    return 0xffffffffffffffffull << nr_unused_bits;
}

static uint64_t end_word_mask(int end_bit_nr)
{
    int nr_unused_bits = 64 - ((end_bit_nr + 1) % 64);
    uint64_t mask = 0xffffffffffffffffull;
    if (nr_unused_bits != 64) {
        mask >>= nr_unused_bits;
    }
    return mask;
}

static int word_parity(uint64_t word)
{
    static int byte_parity[256] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
                                   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
    int parity = 0;
    parity ^= byte_parity[word & 0xff];
    parity ^= byte_parity[word >> 8 & 0xff];
    parity ^= byte_parity[word >> 16 & 0xff];
    parity ^= byte_parity[word >> 24 & 0xff];
    parity ^= byte_parity[word >> 32 & 0xff];
    parity ^= byte_parity[word >> 40 & 0xff];
    parity ^= byte_parity[word >> 48 & 0xff];
    parity ^= byte_parity[word >> 56 & 0xff];
    return parity;
}

Key::Key(int nr_bits_param)
{
    // Construct a key with the bits set to random values.
    nr_bits = nr_bits_param;
    nr_words = (nr_bits_param - 1) / 64 + 1;
    words = new uint64_t[nr_words];
    for (int word_nr = 0; word_nr < nr_words; word_nr++) {
        words[word_nr] = random_uint64();
    }
    words[nr_words - 1] &= end_word_mask(nr_bits - 1);
}

Key::Key(const Key& key)
{
    nr_bits = key.nr_bits;
    nr_words = key.nr_words;
    words = new uint64_t[nr_words];
    std::memcpy(words, key.words, nr_words * sizeof(words[0]));
}

Key::Key(const Key& orig_key, const Shuffle& shuffle)
{  
    nr_bits = orig_key.nr_bits;
    nr_words = orig_key.nr_words;
    words = new uint64_t[nr_words];
    for (int orig_bit_nr = 0; orig_bit_nr < orig_key.nr_bits; ++orig_bit_nr) {
        int shuffle_bit_nr = shuffle.orig_to_shuffle(orig_bit_nr);
        int bit_value = orig_key.get_bit(orig_bit_nr);
        set_bit(shuffle_bit_nr, bit_value);
    }
}

Key::~Key()
{
    delete[] words;
}

std::string Key::to_string() const
{
    // LSB: bit_nr=0            MSB: bit_nr=nr_bits-1
    // v                        v
    // 01011010010110010010101001
    std::string string = "";
    int bit_nr = 0;
    while (bit_nr < nr_bits) {
        string += get_bit(bit_nr) ? "1" : "0";
        bit_nr += 1;    
    }
    return string;
}

int Key::get_nr_bits() const
{
    return nr_bits;
}

int Key::get_bit(int bit_nr) const
{
    assert(bit_nr < nr_bits);
    int word_nr = bit_nr / 64;
    int bit_nr_in_word = bit_nr % 64;
    uint64_t mask = 1ull << bit_nr_in_word;
    return (words[word_nr] & mask) ? 1 : 0;
}

void Key::set_bit(int bit_nr, int value)
{
    assert(bit_nr < nr_bits);
    int word_nr = bit_nr / 64;
    int bit_nr_in_word = bit_nr % 64;
    uint64_t mask = 1ull << bit_nr_in_word;
    if (value == 1) {
        words[word_nr] |= mask;
    } else if (value == 0) {
        words[word_nr] &= ~mask;
    } else {
        assert(false);
    }
}

void Key::flip_bit(int bit_nr)
{
    assert(bit_nr < nr_bits);
    int word_nr = bit_nr / 64;
    int bit_nr_in_word = bit_nr % 64;
    uint64_t mask = 1ull << bit_nr_in_word;
    words[word_nr] ^= mask;
}

void Key::swap_bits(int bit_nr_1, int bit_nr_2)
{
    assert(bit_nr_1 < nr_bits);
    assert(bit_nr_2 < nr_bits);
    int bit_1_value = get_bit(bit_nr_1);
    int bit_2_value = get_bit(bit_nr_2);
    set_bit(bit_nr_1, bit_2_value);
    set_bit(bit_nr_2, bit_1_value);
}

void Key::apply_noise(double bit_error_rate)
{
    // Uses Bob Floyd sampling to select a sample of bits to flip.
    // See https://stackoverflow.com/questions/28287138 for details.
    int nr_bit_errors = bit_error_rate * nr_bits + 0.5;
    std::unordered_set<int> error_bits;
    for (int d = nr_bits - nr_bit_errors; d < nr_bits; ++d) {
        int t = random_bit_nr(0, d);
        if (error_bits.find(t) == error_bits.end()) {
            error_bits.insert(t);
        } else {
            error_bits.insert(d);
        }
    }
    for (auto it = error_bits.begin(); it != error_bits.end(); ++it) {
        flip_bit(*it);
    }
}

int Key::compute_range_parity(int start_bit_nr, int end_bit_nr) const
{
    assert(start_bit_nr < nr_bits);
    assert(end_bit_nr < nr_bits);
    int start_word_nr = start_bit_nr / 64;
    int end_word_nr = end_bit_nr / 64;
    uint64_t xor_words = 0;
    for (int word_nr = start_word_nr; word_nr <= end_word_nr; ++word_nr) {
        xor_words ^= words[word_nr];
    }
    // Undo bits that we did not want to include in first word.
    uint64_t unwanted_mask = ~start_word_mask(start_bit_nr);
    uint64_t unwanted_bits = words[start_word_nr] & unwanted_mask;
    xor_words ^= unwanted_bits;
    // Undo bits that we did not want to include in first word.
    unwanted_mask = ~end_word_mask(end_bit_nr);
    unwanted_bits = words[end_word_nr] & unwanted_mask;
    xor_words ^= unwanted_bits;
    return word_parity(xor_words);
}

int Key::nr_bits_different(const Key& other_key) const
{
    int difference = 0;
    assert(nr_bits == other_key.nr_bits);
    for (int bit_nr = 0; bit_nr < nr_bits; ++bit_nr) {
        if (get_bit(bit_nr) != other_key.get_bit(bit_nr)) {
            ++difference;
        }
    }
    return difference;
}
