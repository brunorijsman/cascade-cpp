#ifndef ITERATION_H
#define ITERATION_H

namespace Cascade {

class Iteration
{
public:
    Iteration(unsigned iteration_nr);
private:
#pragma GCC diagnostic ignored "-Wunused-private-field"
    unsigned iteration_nr;
};

} /* namespace Cascade */

#endif /* ifndef ITERATION_H */
