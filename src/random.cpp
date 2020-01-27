#include "random.h"
#include <random>

namespace Cascade {

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

void random_seed(uint64_t seed)
{
    mt.seed(seed);
}

uint64_t random_uint64(void)
{
    return dist(mt);
}

size_t random_bit_nr(size_t start_bit_nr, size_t end_bit_nr)
{
    static std::uniform_int_distribution<size_t> dist(start_bit_nr, end_bit_nr);
    return dist(mt);
}

} /* namespace Cascade */