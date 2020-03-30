#ifndef CLASSICAL_SESSION_H
#define CLASSICAL_SESSION_H

#include "block.h"
#include "pending_item.h"
#include <stdint.h>

namespace Cascade {

class ClassicalSession
{
public:
    virtual ~ClassicalSession() = 0;
    virtual void start_iteration(int iteration_nr, uint64_t shuffle_seed) = 0;
    virtual void ask_correct_parities(PendingItemQueue& ask_correct_parity_blocks) = 0;
};

} /* namespace Cascade */

#endif /* ifndef CLASSICAL_SESSION_H */
