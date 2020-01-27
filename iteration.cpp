#include "iteration.h"
#include "reconciliation.h"

using namespace Cascade;

#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

Iteration::Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf):
    reconciliation(reconciliation),
    iteration_nr(iteration_nr),
    biconf(biconf),
    shuffled_key(reconciliation.get_reconciled_key(), iteration_nr != 1)
{
    if (this->biconf) {
        this->reconcile_biconf();
    } else {
        this->reconcile_cascade();
    }
}

void Iteration::reconcile_cascade(void)
{
}

void Iteration::reconcile_biconf(void)
{
}
