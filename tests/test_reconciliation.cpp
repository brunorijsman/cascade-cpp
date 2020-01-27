#include <gtest/gtest.h>
#include "algorithm.h"
#include "reconciliation.h"

using namespace Cascade;

TEST (Reconciliation, test_1)
{
    Key noisy_key(16);
    double estimated_bit_error_rate = 0.73 / 4.0;
    Reconciliation reconciliation("original", noisy_key, estimated_bit_error_rate);
    reconciliation.reconcile();
}
