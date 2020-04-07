#include <gtest/gtest.h>
#include "block.h"
#include "algorithm.h"
#include "key.h"
#include "mock_classical_session.h"
#include "reconciliation.h"

using namespace Cascade;

static const int key_size = 100;

Iteration make_iteration(bool biconf)
{
    Key correct_key(key_size);
    Key noisy_key = correct_key;
    double bit_error_rate = 0.1;
    noisy_key.apply_noise(bit_error_rate);
    const Algorithm* algorithm = Algorithm::get_by_name("original");
    assert(algorithm);
    MockClassicalSession classical_session(correct_key, algorithm->cache_shuffles);
    Reconciliation reconciliation(*algorithm, classical_session, noisy_key, bit_error_rate);
    int iteration_nr = 1;
    Iteration iteration(reconciliation, iteration_nr, biconf);
    return iteration;
}

TEST (Block, constructor_and_destructor)
{
    Iteration iteration = make_iteration(false);
}

TEST (Block, compute_name)
{
    Iteration cascade_iteration = make_iteration(false);
    Block block_top(cascade_iteration, 0, key_size - 1, NULL, 3);
    EXPECT_EQ(block_top.compute_name(), "c:1:3");  /* c = cascade, 1 = iteration, 3 = block nr */
    Block block_l(cascade_iteration, 0, key_size / 2, &block_top, 0);
    EXPECT_EQ(block_l.compute_name(), "c:1:3L");
    Block block_lr(cascade_iteration, key_size / 4, key_size / 2, &block_l, 1);
    EXPECT_EQ(block_lr.compute_name(), "c:1:3LR");
    Iteration biconf_iteration = make_iteration(true);
    Block block_biconf(biconf_iteration, 0, key_size - 1, NULL, 5);
    EXPECT_EQ(block_biconf.compute_name(), "b:1:5");  /* b = biconf, 1 = iteration, 5 = block nr */
}

//@@@
TEST (Block, blah)
{
    std::cout << "Blah allocate!" << std::endl;
    int *ptr = new int;
    *ptr = 10;
    std::cout << "It contains: " << *ptr << std::endl;
    *ptr = 11;
}

