#include <gtest/gtest.h>
#include "algorithm.h"
#include "mock_classical_session.h"
#include "reconciliation.h"

using namespace Cascade;

TEST (Reconciliation, test_1)
{
    Key correct_key(16);
    Key noisy_key = correct_key;
    double bit_error_rate = 0.73 / 4.0;
    noisy_key.apply_noise(bit_error_rate);
    MockClassicalSession classical_session(correct_key);
    Reconciliation reconciliation("original", classical_session, noisy_key, bit_error_rate);
    reconciliation.reconcile();
}
