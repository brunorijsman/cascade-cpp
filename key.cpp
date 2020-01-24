#include "key.h"
#include <iostream>
#include <random>

static uint64_t random_uint64(void)
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    return dist(mt);
}

Key::Key(size_t nr_bits)
{
    assert(nr_bits > 0);
    this->nr_bits = nr_bits;
    size_t nr_blocks = (nr_bits - 1) / 64 + 1;
    this->blocks = new uint64_t[nr_blocks];
    for (size_t block_nr = 0; block_nr < nr_blocks; block_nr++) {
        this->blocks[block_nr] = random_uint64();
    }
    if (nr_bits % 64 != 0) {
        size_t nr_trailing_zero_bits = 64 - (nr_bits % 64);
        uint64_t mask = 0xffffffffffffffffull << nr_trailing_zero_bits;
        this->blocks[nr_blocks - 1] &= mask;
    }
}

Key::~Key()
{
    delete this->blocks;
}

int main(void)
{
    Key key(129);
    return 0;
}