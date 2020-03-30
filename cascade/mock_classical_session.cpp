#include "mock_classical_session.h"
#include "debug.h"
#include "iteration.h"
#include "key.h"
#include "shuffle.h"
#include "shuffled_key.h"

#include <iostream> //@@@
#include <string>   //@@@

using namespace Cascade;

MockClassicalSession::MockClassicalSession(Key& correct_key):
    correct_key(correct_key)
{
    DEBUG("Create MockClassicalSession: correct_key=" << correct_key.to_string());
}

MockClassicalSession::~MockClassicalSession()
{
}

void MockClassicalSession::start_iteration(int iteration_nr, uint64_t shuffle_seed)
{
    int nr_key_bits = correct_key.get_nr_bits();
    bool identity = (iteration_nr == 1);
    Shuffle shuffle(nr_key_bits, identity, shuffle_seed);
    ShuffledKeyPtr shuffled_key(new ShuffledKey(correct_key, shuffle));
    shuffled_keys[iteration_nr] = shuffled_key;
}

void MockClassicalSession::ask_correct_parities(PendingItemQueue& ask_correct_parity_blocks)
{
    // Once we implement the real classical session, we will need to keep track of the blocks
    // for which we asked Alice the correct parity, but for which we have not yet received the
    // answer from Alice. For now, assume we get the answer immediately.
    for (auto it = ask_correct_parity_blocks.begin(); it != ask_correct_parity_blocks.end(); ++it) {
        PendingItem pending_item(*it);
        BlockPtr block = pending_item.block;
        int iteration_nr = block->get_iteration().get_iteration_nr();
        ShuffledKeyPtr shuffled_key = shuffled_keys[iteration_nr];
        int correct_parity = shuffled_key->compute_range_parity(block->get_start_bit_nr(),
                                                                block->get_end_bit_nr());
        block->set_correct_parity(correct_parity);
        DEBUG("Ask correct parity:" << 
              " block=" << block->debug_str() <<
              " correct_parity=" << correct_parity);
    }
}
