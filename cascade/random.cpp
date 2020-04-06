#include "random.h"
#include <random>

namespace Cascade {

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

void set_random_uint64_seed(uint64_t seed)
{
    mt.seed(seed);
}

uint64_t random_uint64()
{
    return dist(mt);
}

int random_bit_nr(int start_bit_nr, int end_bit_nr)
{
    std::uniform_int_distribution<int> dist(start_bit_nr, end_bit_nr);
    return dist(mt);
}

} /* namespace Cascade */