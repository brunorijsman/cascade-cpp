#ifndef ITERATION_H
#define ITERATION_H

#include "block.h"
#include "key.h"
#include "shuffle.h"
#include "shuffled_key.h"
#include <map>
#include <vector>

namespace Cascade {

class Reconciliation;

class Iteration
{
public:
    Iteration(Reconciliation& reconciliation, int iteration_nr, bool biconf);
    static ShufflePtr init_shuffle(Reconciliation& reconciliation, int iteration_nr);
    ~Iteration();
    Reconciliation& get_reconciliation() const;
    int get_iteration_nr() const;
    bool get_biconf() const;
    ShufflePtr get_shuffle() const;
    ShuffledKey& get_shuffled_key();
    void schedule_initial_work();
    int try_correct_block(BlockPtr block, bool correct_right_sibling, bool cascade);
    BlockPtr get_cascade_block(int orig_key_bit_nr) const;
    void flip_parity_in_all_blocks_containing_bit(int orig_key_bit_nr);
private:
    void schedule_initial_work_cascade();
    void schedule_initial_work_biconf();
    int try_correct_right_sibling_block(BlockPtr block, bool cascade);
    Reconciliation& reconciliation;
    int iteration_nr;
    bool biconf;
    int nr_key_bits;
    ShufflePtr shuffle;
    ShuffledKey shuffled_key;
    int block_size;
    std::vector<BlockPtr> top_blocks;
};

typedef std::shared_ptr<Iteration> IterationPtr;

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
