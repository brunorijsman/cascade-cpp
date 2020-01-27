#ifndef ITERATION_H
#define ITERATION_H

namespace Cascade {

class Reconciliation;

class Iteration 
{
public:
    Iteration(Reconciliation& reconciliation, unsigned iteration_nr, bool biconf);
private:
    Reconciliation& reconciliation;
    unsigned iteration_nr;
    bool biconf;
};

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
