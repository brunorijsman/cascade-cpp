#ifndef BICONF_ITERATION_H
#define BICONF_ITERATION_H

#include "iteration.h"

namespace Cascade {

class BiconfIteration: public Iteration
{
public:
    BiconfIteration(unsigned iteration_nr);
    virtual void reconcile();
};

} /* namespace Cascade */

#endif /* ifndef BICONF_ITERATION_H */
