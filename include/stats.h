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
    long start_iteration_messages;
    long start_iteration_bits;
    long ask_parity_messages;
    long ask_parity_blocks;
    long ask_parity_bits;
    long reply_parity_bits;
    long reconciliation_bits;
    double efficiency;
    double reconciliation_bits_per_key_bit;
    long infer_parity_blocks; // Only simple inference; not yet Full Block Parity Inference (BPI)

};

} /* namespace Cascade */

#endif /* ifndef STATS_H */
