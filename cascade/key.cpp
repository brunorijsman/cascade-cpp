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
// Note: we use the term "word" instead of te more natural term "block" to avoid confusion with
//       cascade blocks.

static uint64_t end_word_mask(int end_bit_nr)
{
    int nr_unused_bits = 64 - ((end_bit_nr + 1) % 64);
    uint64_t mask = 0xffffffffffffffffull;
    if (nr_unused_bits != 64) {
        mask >>= nr_unused_bits;
    }
    return mask;
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
