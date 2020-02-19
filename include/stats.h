#ifndef STATS_H
#define STATS_H

namespace Cascade {

class Stats
{
public:
    Stats();
    double elapsed_process_time;
    double elapsed_real_time;
    long normal_iterations;
    long biconf_iterations;
    long ask_parity_messages;
    long ask_parity_blocks;
    long ask_parity_bits;
    long reply_parity_bits;
    double unrealistic_efficiency;
    double realistic_efficiency;

};

} /* namespace Cascade */

#endif /* ifndef STATS_H */
