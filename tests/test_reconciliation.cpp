#include <gtest/gtest.h>
#include "algorithm.h"
#include "debug.h"
#include "mock_classical_session.h"
#include "random.h"
#include "reconciliation.h"

using namespace Cascade;



void run_reconciliation_test(int seed, const char* algorithm)
{
    random_seed(seed);

    Key correct_key(10000);
    Key noisy_key = correct_key;

    double bit_error_rate = 0.1;
    noisy_key.apply_noise(bit_error_rate);
    // TODO: Better way of announcing bit errors
    for (int bit_nr = 0; bit_nr < 16; ++bit_nr) {
        if (correct_key.get_bit(bit_nr) != noisy_key.get_bit(bit_nr)) {
            DEBUG("Bit error in " << bit_nr);
        }
    }
    MockClassicalSession classical_session(correct_key);
    Reconciliation reconciliation(algorithm, classical_session, noisy_key, bit_error_rate);
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
    run_reconciliation_test(33333, "biconf-cascade");
}

TEST (Reconciliation, algorithm_biconf_complement)
{
    run_reconciliation_test(4444, "biconf-complement");
}
