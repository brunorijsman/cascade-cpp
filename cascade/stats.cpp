#include "stats.h"

using namespace Cascade;

Stats::Stats():
    elapsed_process_time(0.0),
    elapsed_real_time(0.0),
    normal_iterations(0),
    biconf_iterations(0),
    start_iteration_messages(0),
    start_iteration_bits(0),
    ask_parity_messages(0),
    ask_parity_blocks(0),
    ask_parity_bits(0),
    reply_parity_bits(0),
    reconciliation_bits(0),
    efficiency(0.0),
    reconciliation_bits_per_key_bit(0.0),
    infer_parity_blocks(0)
{
}
