#ifndef MOCK_CLASSICAL_SESSION_H
#define MOCK_CLASSICAL_SESSION_H

#include "classical_session.h"
#include "key.h"
#include <vector>

namespace Cascade {

class vector;

class MockClassicalSession : public ClassicalSession
{
public:
    MockClassicalSession(Key& correct_key);
    virtual ~MockClassicalSession();
    virtual void start_iteration(unsigned iteration_nr, uint64_t shuffle_seed);
    virtual void ask_correct_parities();
private:
    Key& correct_key;
    std::vector<KeyPtr> shuffled_correct_keys;   // Indexed by iteration_nr
};

} /* namespace Cascade */

#endif /* ifndef MOCK_CLASSICAL_SESSION_H */
