#include <gtest/gtest.h>
#include "algorithm.h"
#include "debug.h"
#include "mock_classical_session.h"
#include "reconciliation.h"

using namespace Cascade;

TEST (Reconciliation, test_1)
{
    Key correct_key(16);
    Key noisy_key = correct_key;

    double bit_error_rate = 0.73 / 4.0;
    noisy_key.apply_noise(bit_error_rate);
    // TODO: Better way of announcing bit errors
    for (size_t bit_nr = 0; bit_nr < 16; ++bit_nr) {
        if (correct_key.get_bit(bit_nr) != noisy_key.get_bit(bit_nr)) {
            DEBUG("Bit error in " << bit_nr);
        }
    }
    MockClassicalSession classical_session(correct_key);
    Reconciliation reconciliation("original", classical_session, noisy_key, bit_error_rate);
    reconciliation.reconcile();
}
