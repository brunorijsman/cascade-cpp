#include "mock_classical_session.h"
#include "debug.h"
#include "shuffle.h"

using namespace Cascade;

MockClassicalSession::MockClassicalSession(Key& correct_key):
    correct_key(correct_key),
    shuffled_correct_keys()
{
    DEBUG("Create mock classical session:" << 
          " correct_key=" << correct_key.to_string());
}

MockClassicalSession::~MockClassicalSession()
{
}

void MockClassicalSession::start_iteration(int iteration_nr, uint64_t shuffle_seed)
{
    int nr_key_bits = correct_key.get_nr_bits();
    bool identity = (iteration_nr == 1);
    Shuffle shuffle(nr_key_bits, identity, shuffle_seed);
    KeyPtr shuffled_correct_key = KeyPtr(new Key(correct_key, shuffle));
    shuffled_correct_keys[iteration_nr] = shuffled_correct_key;
}

void MockClassicalSession::ask_correct_parities(std::deque<BlockAndBool>& ask_correct_parity_blocks)
{
    // Once we implement the real classical session, we will need to keep track of the blocks
    // for which we asked Alice the correct parity, but for which we have not yet received the
    // answer from Alice. For now, assume we get the answer immediately.
    for (auto it = ask_correct_parity_blocks.begin(); it != ask_correct_parity_blocks.end(); ++it) {
        BlockAndBool block_and_bool(*it);
        BlockPtr block = block_and_bool.first;
        int iteration_nr = block->get_iteration_nr();
        KeyPtr shuffled_correct_key = shuffled_correct_keys[iteration_nr];
        int correct_parity = block->compute_parity_for_key(*shuffled_correct_key);
        block->set_correct_parity(correct_parity);
        DEBUG("Ask correct parity:" << 
              " block=" << block->compute_name() <<
              " correct_parity=" << correct_parity);
    }
}
