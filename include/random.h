#ifndef RANDOM_H
#define RANDOM_H

#include <stddef.h>
#include <stdint.h>

namespace Cascade {

void random_seed(uint64_t seed);
uint64_t random_uint64();
int random_bit_nr(int start_bit_nr, int end_bit_nr);

} /* namespace Cascade */

#endif /* ifndef RANDOM_H */
