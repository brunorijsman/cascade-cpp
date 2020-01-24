#ifndef KEY_H
#define KEY_H

#include <stddef.h>
#include <stdint.h>

class Key 
{
public:
    Key(size_t nr_bits);
    ~Key();
private:
    size_t nr_bits;
    size_t nr_blocks;
    uint64_t *blocks;
};

#endif /* ifndef KEY_H */
