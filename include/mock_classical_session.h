#ifndef MOCK_CLASSICAL_SESSION_H
#define MOCK_CLASSICAL_SESSION_H

#include "classical_session.h"

namespace Cascade {

class MockClassicalSession : public ClassicalSession
{
public:
    MockClassicalSession();
    virtual ~MockClassicalSession();
    virtual void start_iteration(unsigned iteration_nr);
    virtual void end_iteration(unsigned iteration_nr);
    virtual void ask_correct_parities(void);
};

} /* namespace Cascade */

#endif /* ifndef MOCK_CLASSICAL_SESSION_H */
