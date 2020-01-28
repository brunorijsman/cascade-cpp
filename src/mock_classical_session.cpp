#include "mock_classical_session.h"
#include "shuffle.h"

#include <iostream> //@@@

using namespace Cascade;

MockClassicalSession::MockClassicalSession(Key& correct_key):
    correct_key(correct_key),
    shuffled_correct_keys()
{
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
    for (std::deque<BlockPtr>::iterator it = ask_correct_parity_blocks.begin();
         it != ask_correct_parity_blocks.end();
         ++it) {
        BlockPtr block = *it;
        std::cout << "Computing correct parity " << block->get_name() << std::endl;
        // CONTINUE: Actually do it.
    }
}
