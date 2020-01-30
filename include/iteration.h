#ifndef ITERATION_H
#define ITERATION_H

#include "block.h"
#include "key.h"
#include "shuffle.h"
#include <map>
#include <vector>

namespace Cascade {

class Reconciliation;

class Iteration
{
public:
    Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf);
    ~Iteration();
    Reconciliation& get_reconciliation() const;
    unsigned get_iteration_nr() const;
    bool get_biconf() const;
    const Shuffle& get_shuffle() const;
    const Key& get_shuffled_key() const;
    void reconcile();
    void flip_orig_key_bit(size_t orig_key_bit_nr);
    bool try_correct_block(BlockPtr block, bool cascade);
private:
    void reconcile_cascade();
    void reconcile_biconf();
    bool try_correct_right_sibling_block(BlockPtr block, bool cascade);
    Reconciliation& reconciliation;
    unsigned iteration_nr;
    bool biconf;
    size_t nr_key_bits;
    Shuffle shuffle;
    Key shuffled_key;
    std::vector<BlockPtr> top_blocks;
};

typedef std::shared_ptr<Iteration> IterationPtr;

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
