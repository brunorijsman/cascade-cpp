#ifndef BLOCK_H
#define BLOCK_H

#include <memory>
#include <string>

namespace Cascade {

class Block;
typedef std::shared_ptr<Block> BlockPtr;

class Iteration;
class ShuffledKey;

class Block
{
public:
    static const int unknown_parity;
    Block(Iteration& iteration, int start_bit_nr, int end_bit_nr, Block* parent_block,
          int block_nr);
    ~Block();
    Iteration& get_iteration() const;
    int get_nr_bits() const;
    int get_start_bit_nr() const;
    int get_end_bit_nr() const;
    std::string compute_name() const;
    std::string debug_str() const;
    int get_correct_parity();
    int get_or_compute_current_parity();
    void flip_current_parity();
    void set_correct_parity(int parity);
    bool correct_parity_is_known() const;
    bool try_to_infer_correct_parity();
    int get_error_parity();
    Block* get_parent_block() const;
    BlockPtr get_left_sub_block() const;
    BlockPtr create_left_sub_block();
    BlockPtr get_right_sub_block() const;
    BlockPtr create_right_sub_block();
    long encoded_bits() const;
    void release_sub_blocks();
private:
    Iteration& iteration;
    ShuffledKey& shuffled_key;
    int start_bit_nr;
    int end_bit_nr;
    int current_parity;
    int correct_parity;
    Block* parent_block;
    int block_nr;
    BlockPtr left_sub_block;
    BlockPtr right_sub_block;
};

} /* namespace Cascade */

#endif /* ifndef BLOCK_H */
