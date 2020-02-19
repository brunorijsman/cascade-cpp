#ifndef DATA_POINT_H
#define DATA_POINT_H

#include "aggregate_stats.h"
#include <string>

class DataPoint
{
public:
    DataPoint(const std::string& algorithm_name, int key_size, double requested_bit_error_rate);
    std::string to_json() const;
    std::string execution_time;                         // TODO
    std::string algorithm_name;
    int key_size;
    double requested_bit_error_rate;
    std::string code_version;                           // TODO
    AggregateStats actual_bit_errors;                   // TODO
    AggregateStats actual_bit_error_rate;               // TODO
    AggregateStats elapsed_process_time;                // TODO
    AggregateStats elapsed_real_time;                   // TODO
    AggregateStats normal_iterations;                   // TODO
    AggregateStats biconf_iterations;                   // TODO
    AggregateStats ask_parity_messages;                 // TODO
    AggregateStats ask_parity_blocks;                   // TODO
    AggregateStats ask_parity_bits;                     // TODO
    AggregateStats reply_parity_bits;                   // TODO
    AggregateStats unrealistic_efficiency;              // TODO
    AggregateStats realistic_efficiency;                // TODO
    AggregateStats infer_parity_blocks;                 // TODO
    AggregateStats remaining_bit_errors;                // TODO
    AggregateStats remaining_bit_error_rate;            // TODO
    AggregateStats remaining_frame_error_rate;          // TODO
};

#endif /* ifndef DATA_POINT_H */
