#include "key.h"
#include <assert.h>
#include <iostream>
#include <random>

using namespace Cascade;

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

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

static uint64_t random_uint64(void)
{
    return dist(mt);
}

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

void Key::set_seed(uint64_t seed)
{
    mt.seed(seed);
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
    memcpy((void*) this->words, (void*) key.words, this->nr_words * sizeof(this->words[0]));
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
