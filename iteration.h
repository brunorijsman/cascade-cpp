#ifndef ITERATION_H
#define ITERATION_H

#include "block.h"
#include "shuffled_key.h"           // TODO: Fold that into this class?
#include <vector>

namespace Cascade {

class Reconciliation;

class Iteration
{
public:
    Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf);
private:
    void reconcile_cascade(void);
    void reconcile_biconf(void);
    Reconciliation& reconciliation;
    unsigned iteration_nr;
    bool biconf;
    ShuffledKey shuffled_key;
    size_t nr_key_bits;
    std::vector<BlockPtr> top_blocks;
};

typedef std::shared_ptr<Iteration> IterationPtr;

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
