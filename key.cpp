#include "key.h"
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
    this->nr_blocks = (nr_bits - 1) / 64 + 1;
    this->blocks = new uint64_t[this->nr_blocks];
    for (size_t block_nr = 0; block_nr < this->nr_blocks; block_nr++) {
        this->blocks[block_nr] = random_uint64();
    }
    if (nr_bits % 64 != 0) {
        size_t nr_trailing_zero_bits = 64 - (nr_bits % 64);
        uint64_t mask = 0xffffffffffffffffull << nr_trailing_zero_bits;
        this->blocks[this->nr_blocks - 1] &= mask;
    }
}

Key::~Key()
{
    delete[] this->blocks;
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
    size_t block_nr = bit_nr / 64;
    size_t bit_nr_in_block = 63 - (bit_nr % 64);
    uint64_t mask = 1 << bit_nr_in_block;
    return (this->blocks[block_nr] & mask) != 0;
}
