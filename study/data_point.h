#ifndef DATA_POINT_H
#define DATA_POINT_H

#include "aggregate_stats.h"
#include <string>

namespace Cascade {
    class Stats;
}

class DataPoint
{
public:
    DataPoint(const std::string& algorithm_name, int key_size, double requested_bit_error_rate);
    void record_reconciliation_stats(const Cascade::Stats& stats);
    std::string to_json() const;
    std::string algorithm_name;
    int key_size;
    double requested_bit_error_rate;
    std::string code_version;
    long reconciliations;
    AggregateStats actual_bit_errors;
    AggregateStats actual_bit_error_rate;
    AggregateStats elapsed_process_time;
    AggregateStats elapsed_real_time;
    AggregateStats normal_iterations;
    AggregateStats biconf_iterations;
    AggregateStats ask_parity_messages;
    AggregateStats ask_parity_blocks;
    AggregateStats ask_parity_bits;
    AggregateStats reply_parity_bits;
    AggregateStats unrealistic_efficiency;
    AggregateStats realistic_efficiency;
    AggregateStats infer_parity_blocks;
    AggregateStats remaining_bit_errors;
    AggregateStats remaining_bit_error_rate;
    AggregateStats remaining_frame_error_rate;
};

#endif /* ifndef DATA_POINT_H */
