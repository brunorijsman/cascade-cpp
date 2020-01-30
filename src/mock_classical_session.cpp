#include "mock_classical_session.h"
#include "shuffle.h"

#include <iostream> //@@@

using namespace Cascade;

MockClassicalSession::MockClassicalSession(Key& correct_key):
    correct_key(correct_key),
    shuffled_correct_keys()
{
    std::cout << "Correct key = " << correct_key.to_string() << std::endl; // @@@
}

MockClassicalSession::~MockClassicalSession()
{
}

void MockClassicalSession::start_iteration(unsigned iteration_nr, uint64_t shuffle_seed)
{
    size_t nr_key_bits = this->correct_key.get_nr_bits();
    bool identity = (iteration_nr == 1);
    Shuffle shuffle(nr_key_bits, identity, shuffle_seed);
    KeyPtr shuffled_correct_key = KeyPtr(new Key(this->correct_key, shuffle));
    this->shuffled_correct_keys[iteration_nr] = shuffled_correct_key;
}

void MockClassicalSession::ask_correct_parities(std::deque<BlockPtr>& ask_correct_parity_blocks)
{
    // TODO: Once we implement the real classical session, we will need to keep track of the blocks
    // for which we asked Alice the correct parity, but for which we have not yet received the
    // answer from Alice. For now, assume we get the answer immediately.
    for (std::deque<BlockPtr>::iterator it = ask_correct_parity_blocks.begin();
         it != ask_correct_parity_blocks.end();
         ++it) {
        BlockPtr block = *it;
        unsigned iteration_nr = block->get_iteration_nr();
        KeyPtr shuffled_correct_key = this->shuffled_correct_keys[iteration_nr];
        int correct_parity = block->compute_parity_for_key(*shuffled_correct_key);
        block->set_correct_parity(correct_parity);
        std::cout << "Correct parity for " << block->compute_name() << " is "
                  << correct_parity << std::endl;
    }
}
