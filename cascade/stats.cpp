#include "stats.h"

using namespace Cascade;

Stats::Stats():
    elapsed_process_time(0.0),
    elapsed_real_time(0.0),
    normal_iterations(0),
    biconf_iterations(0),
    ask_parity_messages(0),
    ask_parity_blocks(0),
    ask_parity_bits(0),
    reply_parity_bits(0),
    unrealistic_efficiency(0.0),
    realistic_efficiency(0.0),
    infer_parity_blocks(0)
{
}
