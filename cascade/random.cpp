#include "random.h"
#include <random>

namespace Cascade {

static std::random_device rd;
static std::mt19937 mt(rd());

void set_random_uint32_seed(uint32_t seed)
{
    mt.seed(seed);
}

uint32_t random_uint32()
{
    return mt();
}

int random_bit_nr(int start_bit_nr, int end_bit_nr)
{
    // Don't use std::uniform_int_distribution because the sequence of random numbers that is 
    // generated for given seed is not the same across different compilers
    // (see https://stackoverflow.com/questions/61069954)
    int nr_bits = end_bit_nr - start_bit_nr + 1;
    return start_bit_nr + random_uint32() % nr_bits;
}

} /* namespace Cascade */