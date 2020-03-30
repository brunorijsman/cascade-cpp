#include "pending_item.h"

using namespace Cascade;

PendingItem::PendingItem(BlockPtr block, bool correct_right_sibling):
    block(block),
    correct_right_sibling(correct_right_sibling)
{
}
