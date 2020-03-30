#ifndef PENDING_ITEM_H
#define PENDING_ITEM_H

#include "block.h"
#include <deque>

namespace Cascade {

typedef struct {
    BlockPtr block;
    bool correct_right_sibling;
} PendingItem;

typedef std::deque<PendingItem> PendingItemQueue;

} /* namespace Cascade */

#endif /* ifndef PENDING_ITEM_H */
