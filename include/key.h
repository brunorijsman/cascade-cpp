#ifndef KEY_H
#define KEY_H

#include <memory>
#include <string>

namespace Cascade {

class Shuffle;

class Key 
{
public:
    Key(int nr_bits);
    Key(const Key& key);
    Key(const Key& orig_key, const Shuffle& shuffle);
    ~Key();
    std::string to_string() const;
    int get_nr_bits() const;
    int get_bit(int bit_nr) const;
    void set_bit(int bit_nr, int value);
    void flip_bit(int bit_nr);
    void apply_noise(double bit_error_rate);
    int compute_range_parity(int start_bit_nr, int end_bit_nr) const;
    int nr_bits_different(const Key& other_key) const;
private:
    int nr_bits;
    int nr_words;
    uint64_t *words;
};

typedef std::shared_ptr<Key> KeyPtr;

} /* namespace Cascade */

#endif /* ifndef KEY_H */
