#ifndef BLOCK_H
#define BLOCK_H

#include <memory>
#include <stddef.h>
#include <string>

namespace Cascade {

class Iteration;
class Key;

class Block;
typedef std::shared_ptr<Block> BlockPtr;

class Block
{
public:
    static const int unknown_parity;
    Block(Iteration& iteration, Block* parent_block, unsigned block_nr, size_t start_bit_nr,
          size_t end_bit_nr);
    ~Block();
    std::string compute_name() const;
    size_t get_nr_bits() const;
    Iteration& get_iteration() const;
    unsigned get_iteration_nr() const;
    size_t get_start_bit_nr() const;
    size_t get_end_bit_nr() const;
    int compute_current_parity() const;
    int compute_parity_for_key(const Key& shuffled_key) const;
    void set_correct_parity(int parity);
    int compute_error_parity() const;
    bool correct_parity_is_know_or_can_be_inferred();
    Block* get_parent_block() const;
    BlockPtr get_left_sub_block() const;
    BlockPtr create_left_sub_block();
    BlockPtr get_right_sub_block() const;
    BlockPtr create_right_sub_block();
private:
    Iteration& iteration;
    unsigned block_nr;
    size_t start_bit_nr;
    size_t end_bit_nr;
    int correct_parity;
    Block* parent_block;
    BlockPtr left_sub_block;
    BlockPtr right_sub_block;
};

} /* namespace Cascade */

#endif /* ifndef BLOCK_H */
