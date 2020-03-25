#include <gtest/gtest.h>
#include "block.h"
#include "key.h"
#include "mock_classical_session.h"
#include "reconciliation.h"

using namespace Cascade;

TEST (Block, constructor_and_destructor)
{
    Key correct_key(100);
    Key noisy_key = correct_key;
    double bit_error_rate = 0.1;
    noisy_key.apply_noise(bit_error_rate);
    MockClassicalSession classical_session(correct_key);
    Reconciliation reconciliation("original", classical_session, noisy_key, bit_error_rate);
    Iteration iteration(reconciliation, 1, false);
    Block* parent_block = NULL;
    Block block(iteration, parent_block, 0, 0, 99);
}
