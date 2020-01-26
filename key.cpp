#include "key.h"
#include <assert.h>
#include <iostream>
#include <random>

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

static uint64_t random_uint64(void)
{
    return dist(mt);
}

void Key::set_seed(uint64_t seed)
{
    mt.seed(seed);
}

Key::Key(size_t nr_bits)
{
    assert(nr_bits > 0);
    this->nr_bits = nr_bits;
    this->nr_words = (nr_bits - 1) / 64 + 1;
    this->words = new uint64_t[this->nr_words];
    for (size_t word_nr = 0; word_nr < this->nr_words; word_nr++) {
        this->words[word_nr] = random_uint64();
    }
    if (nr_bits % 64 != 0) {
        size_t nr_trailing_zero_bits = 64 - (nr_bits % 64);
        uint64_t mask = 0xffffffffffffffffull << nr_trailing_zero_bits;
        this->words[this->nr_words - 1] &= mask;
    }
}

Key::~Key()
{
    delete[] this->words;
}

std::string Key::to_string()
{
    std::string string;
    for (size_t bit_nr = 0; bit_nr < this->nr_bits; ++bit_nr) {
        string += this->get_bit(bit_nr) ? "1" : "0";
    }
    return string;
}

bool Key::get_bit(size_t bit_nr)
{
    assert(bit_nr < this->nr_bits);
    size_t word_nr = bit_nr / 64;
    size_t bit_nr_in_word = 63 - (bit_nr % 64);
    uint64_t mask = 1 << bit_nr_in_word;
    return (this->words[word_nr] & mask) != 0;
}
