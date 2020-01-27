#ifndef KEY_H
#define KEY_H

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace Cascade {

class Shuffle;

class Key 
{
public:
    Key(size_t nr_bits);
    Key(const Key& key);
    Key(const Key& orig_key, const Shuffle& shuffle);
    ~Key();
    std::string to_string() const;
    size_t get_nr_bits(void) const;
    int get_bit(size_t bit_nr) const;
    void set_bit(size_t bit_nr, int value);
    void swap_bits(size_t bit_nr_1, size_t bit_nr_2);
    int compute_range_parity(size_t start_bit_nr, size_t end_bit_nr) const;
private:
    size_t nr_bits;
    size_t nr_words;
    uint64_t *words;
};

} /* namespace Cascade */

#endif /* ifndef KEY_H */
