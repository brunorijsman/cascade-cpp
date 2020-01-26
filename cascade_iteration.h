#ifndef CASCADE_ITERATION_H
#define CASCADE_ITERATION_H

#include "iteration.h"

namespace Cascade {

class CascadeIteration: public Iteration
{
public:
    CascadeIteration(unsigned iteration_nr);
    virtual void reconcile();
};

} /* namespace Cascade */

#endif /* ifndef CASCADE_ITERATION_H */
