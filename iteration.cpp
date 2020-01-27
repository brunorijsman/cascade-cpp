#include "iteration.h"

using namespace Cascade;

#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

Iteration::Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf)
{
}
