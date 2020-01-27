#ifndef CLASSICAL_SESSION_H
#define CLASSICAL_SESSION_H

namespace Cascade {

class ClassicalSession
{
public:
    virtual ~ClassicalSession() { }
    virtual void start_iteration(unsigned iteration_nr) = 0;
    virtual void end_iteration(unsigned iteration_nr) = 0;
    virtual void ask_correct_parities(void) = 0;
};

} /* namespace Cascade */

#endif /* ifndef CLASSICAL_SESSION_H */
