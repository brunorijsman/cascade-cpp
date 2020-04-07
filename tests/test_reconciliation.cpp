#include <gtest/gtest.h>
#include "algorithm.h"
#include "debug.h"
#include "mock_classical_session.h"
#include "random.h"
#include "reconciliation.h"

using namespace Cascade;

void run_reconciliation_test(int seed, const char* algorithm_name)
{
    const Algorithm* algorithm = Algorithm::get_by_name(algorithm_name);
    assert(algorithm);
    set_random_uint32_seed(seed);
    Key correct_key(10000);
    Key noisy_key = correct_key;
    double bit_error_rate = 0.1;
    noisy_key.apply_noise(bit_error_rate);
    MockClassicalSession classical_session(correct_key, algorithm->cache_shuffles);
    Reconciliation reconciliation(*algorithm, classical_session, noisy_key, bit_error_rate);
    reconciliation.reconcile();
    Key& reconciled_key = reconciliation.get_reconciled_key();
    ASSERT_EQ(correct_key.nr_bits_different(reconciled_key), 0);
}

TEST (Reconciliation, algorithm_original)
{
    run_reconciliation_test(11111, "original");
}

TEST (Reconciliation, algorithm_biconf)
{
    run_reconciliation_test(222222, "biconf");
}

TEST (Reconciliation, algorithm_biconf_cascade)
{
    run_reconciliation_test(444444, "biconf-cascade");
}

TEST (Reconciliation, algorithm_biconf_complement)
{
    run_reconciliation_test(666666, "biconf-complement");
}
