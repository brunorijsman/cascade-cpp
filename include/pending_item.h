#ifndef PENDING_ITEM_H
#define PENDING_ITEM_H

#include "block.h"
#include <deque>

namespace Cascade {

class PendingItem {
public:
    PendingItem(BlockPtr block, bool correct_right_sibling);
    BlockPtr block;
    bool correct_right_sibling;
};

typedef std::deque<PendingItem> PendingItemQueue;

} /* namespace Cascade */

#endif /* ifndef PENDING_ITEM_H */
