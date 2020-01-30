#ifndef BLOCK_H
#define BLOCK_H

#include <memory>
#include <stddef.h>
#include <string>

namespace Cascade {

class Iteration;
class Key;

class Block
{
public:
    static const int unknown_parity;
    Block(Iteration& iteration, unsigned block_nr, size_t start_bit_nr, size_t end_bit_nr);
    ~Block();
    std::string get_name() const;
    size_t get_nr_bits() const;
    unsigned get_iteration_nr() const;
    int compute_current_parity() const;
    int compute_parity_for_key(const Key& correct_key) const;
    void set_correct_parity(int parity);
    int compute_error_parity() const;
    bool try_correct(bool cascade);
    bool correct_parity_is_know_or_can_be_inferred();
private:
    Iteration& iteration;
    unsigned block_nr;
    size_t start_bit_nr;
    size_t end_bit_nr;
    int correct_parity;
};

typedef std::shared_ptr<Block> BlockPtr;

} /* namespace Cascade */

#endif /* ifndef BLOCK_H */
