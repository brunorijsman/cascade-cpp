#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

namespace Cascade {

void set_random_uint32_seed(uint32_t seed);
uint32_t random_uint32();
int random_bit_nr(int start_bit_nr, int end_bit_nr);

} /* namespace Cascade */

#endif /* ifndef RANDOM_H */
