#ifndef MOCK_CLASSICAL_SESSION_H
#define MOCK_CLASSICAL_SESSION_H

#include "classical_session.h"
#include "shuffled_key.h"
#include <map>

namespace Cascade {

class MockClassicalSession : public ClassicalSession
{
public:
    MockClassicalSession(Key& correct_key);
    virtual ~MockClassicalSession();
    virtual void start_iteration_with_shuffle_seed(int iteration_nr, uint64_t shuffle_seed);
    virtual void start_iteration_with_shuffle(int iteration_nr, ShufflePtr shuffle);
    virtual void ask_correct_parities(PendingItemQueue& ask_correct_parity_blocks);
private:
    Key& correct_key;
    std::map<int, ShuffledKeyPtr> shuffled_keys;
};

} /* namespace Cascade */

#endif /* ifndef MOCK_CLASSICAL_SESSION_H */
