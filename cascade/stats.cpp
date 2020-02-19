#include "stats.h"

using namespace Cascade;

Stats::Stats():
    elapsed_process_time(0.0),
    elapsed_real_time(0.0),
    normal_iterations(0),
    biconf_iterations(0),
    ask_parity_messages(0),
    ask_parity_blocks(0)
{
}
