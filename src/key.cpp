#include "key.h"
#include "random.h"
#include "shuffle.h"
#include <assert.h>
#include <cstring>
#include <iostream>
#include <unordered_set>

using namespace Cascade;

// Key bits are stored into 64-bit unsigned "words" as follows:
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

static uint64_t start_word_mask(size_t start_bit_nr)
{
    size_t nr_unused_bits = start_bit_nr % 64;
    return 0xffffffffffffffffull << nr_unused_bits;
}

static uint64_t end_word_mask(size_t end_bit_nr)
{
    size_t nr_unused_bits = 64 - ((end_bit_nr + 1) % 64);
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

Key::Key(size_t nr_bits)
{
    // Construct a key with the bits set to random values.
    assert(nr_bits > 0);
    this->nr_bits = nr_bits;
    this->nr_words = (nr_bits - 1) / 64 + 1;
    this->words = new uint64_t[this->nr_words];
    for (size_t word_nr = 0; word_nr < this->nr_words; word_nr++) {
        this->words[word_nr] = random_uint64();
    }
    this->words[this->nr_words - 1] &= end_word_mask(nr_bits - 1);
}

Key::Key(const Key& key)
{
    this->nr_bits = key.nr_bits;
    this->nr_words = key.nr_words;
    this->words = new uint64_t[this->nr_words];
    std::memcpy(this->words, key.words, this->nr_words * sizeof(this->words[0]));
}

Key::Key(const Key& orig_key, const Shuffle& shuffle)
{  
    this->nr_bits = orig_key.nr_bits;
    this->nr_words = orig_key.nr_words;
    this->words = new uint64_t[this->nr_words];
    for (size_t orig_bit_nr = 0; orig_bit_nr < orig_key.nr_bits; ++orig_bit_nr) {
        size_t shuffle_bit_nr = shuffle.orig_to_shuffle(orig_bit_nr);
        int bit_value = orig_key.get_bit(orig_bit_nr);
        this->set_bit(shuffle_bit_nr, bit_value);
    }
}

Key::~Key()
{
    delete[] this->words;
}

std::string Key::to_string() const
{
    // MSB: bit_nr=nr_bits-1    LSB: bit_nr=0
    // v                        v
    // 01011010010110010010101001
    std::string string = "";
    size_t bit_nr = this->nr_bits - 1;
    while (true) {
        string += this->get_bit(bit_nr) ? "1" : "0";
        if (bit_nr == 0) {
            break;
        }
        bit_nr -= 1;    
    }
    return string;
}

size_t Key::get_nr_bits() const
{
    return this->nr_bits;
}

int Key::get_bit(size_t bit_nr) const
{
    assert(bit_nr < this->nr_bits);
    size_t word_nr = bit_nr / 64;
    size_t bit_nr_in_word = bit_nr % 64;
    uint64_t mask = 1ull << bit_nr_in_word;
    return (this->words[word_nr] & mask) ? 1 : 0;
}

void Key::set_bit(size_t bit_nr, int value)
{
    assert(bit_nr < this->nr_bits);
    size_t word_nr = bit_nr / 64;
    size_t bit_nr_in_word = bit_nr % 64;
    uint64_t mask = 1ull << bit_nr_in_word;
    if (value == 1) {
        this->words[word_nr] |= mask;
    } else if (value == 0) {
        this->words[word_nr] &= ~mask;
    } else {
        assert(false);
    }
}

void Key::flip_bit(size_t bit_nr)
{
    assert(bit_nr < this->nr_bits);
    size_t word_nr = bit_nr / 64;
    size_t bit_nr_in_word = bit_nr % 64;
    uint64_t mask = 1ull << bit_nr_in_word;
    this->words[word_nr] ^= mask;
}

void Key::swap_bits(size_t bit_nr_1, size_t bit_nr_2)
{
    assert(bit_nr_1 < this->nr_bits);
    assert(bit_nr_2 < this->nr_bits);
    int bit_1_value = this->get_bit(bit_nr_1);
    int bit_2_value = this->get_bit(bit_nr_2);
    this->set_bit(bit_nr_1, bit_2_value);
    this->set_bit(bit_nr_2, bit_1_value);
}

void Key::apply_noise(double bit_error_rate)
{
    // Uses Bob Floyd sampling to select a sample of bits to flip.
    // See https://stackoverflow.com/questions/28287138 for details.
    size_t nr_bit_errors = bit_error_rate * this->nr_bits + 0.5;
    std::unordered_set<size_t> error_bits;
    for (size_t d = this->nr_bits - nr_bit_errors; d < this->nr_bits; ++d) {
        size_t t = random_bit_nr(0, d);
        if (error_bits.find(t) == error_bits.end()) {
            error_bits.insert(t);
        } else {
            error_bits.insert(d);
        }
    }
    for (auto it = error_bits.begin(); it != error_bits.end(); ++it) {
        this->flip_bit(*it);
    }
}

int Key::compute_range_parity(size_t start_bit_nr, size_t end_bit_nr) const
{
    assert(start_bit_nr < this->nr_bits);
    assert(end_bit_nr < this->nr_bits);
    size_t start_word_nr = start_bit_nr / 64;
    size_t end_word_nr = end_bit_nr / 64;
    uint64_t xor_words = 0;
    for (size_t word_nr = start_word_nr; word_nr <= end_word_nr; ++word_nr) {
        xor_words ^= this->words[word_nr];
    }
    // Undo bits that we did not want to include in first word.
    uint64_t unwanted_mask = ~start_word_mask(start_bit_nr);
    uint64_t unwanted_bits = this->words[start_word_nr] & unwanted_mask;
    xor_words ^= unwanted_bits;
    // Undo bits that we did not want to include in first word.
    unwanted_mask = ~end_word_mask(end_bit_nr);
    unwanted_bits = this->words[end_word_nr] & unwanted_mask;
    xor_words ^= unwanted_bits;
    return word_parity(xor_words);
}