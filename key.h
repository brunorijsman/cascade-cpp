#ifndef KEY_H
#define KEY_H

#include <stddef.h>
#include <stdint.h>
#include <string>

class Key 
{
public:
    static void set_seed(uint64_t seed);
    Key(size_t nr_bits);
    ~Key();
    std::string to_string();
    bool get_bit(size_t bit_nr);
private:
    size_t nr_bits;
    size_t nr_blocks;
    uint64_t *blocks;
};

#endif /* ifndef KEY_H */
