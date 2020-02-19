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

};

} /* namespace Cascade */

#endif /* ifndef STATS_H */
