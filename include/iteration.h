#ifndef ITERATION_H
#define ITERATION_H

#include "block.h"
#include "key.h"
#include <map>
#include <vector>

namespace Cascade {

class Reconciliation;

class Iteration
{
public:
    Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf);
    ~Iteration();
    unsigned get_iteration_nr() const;
    const Key& get_shuffled_key() const;
    bool get_biconf() const;
private:
    void reconcile_cascade(void);
    void reconcile_biconf(void);
    Reconciliation& reconciliation;
    unsigned iteration_nr;
    bool biconf;
    Key shuffled_key;
    typedef std::map<unsigned, unsigned> BitMap;
    BitMap shuffled_to_orig_map;
    BitMap orig_to_shuffled_map;
    std::vector<BlockPtr> top_blocks;
};

typedef std::shared_ptr<Iteration> IterationPtr;

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
