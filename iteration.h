#ifndef ITERATION_H
#define ITERATION_H

namespace Cascade {

class Iteration
{
public:
    Iteration(unsigned iteration_nr);
    virtual ~Iteration();
    virtual void reconcile(void) = 0;
private:
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO
    unsigned iteration_nr;
};

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
