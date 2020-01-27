#ifndef ITERATION_H
#define ITERATION_H

#include "shuffled_key.h"           // TODO: Fold that into this class?

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
};

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
