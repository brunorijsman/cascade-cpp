#include "data_point.h"
#include "stats.h"
#include <iomanip>
#include <sstream>

DataPoint::DataPoint(const std::string& algorithm_name, int key_size,
                     double requested_bit_error_rate):
    algorithm_name(algorithm_name),
    key_size(key_size),
    requested_bit_error_rate(requested_bit_error_rate),
    reconciliations(0)
{
}

void DataPoint::record_reconciliation_stats(const Cascade::Stats& stats)
{
    reconciliations += 1;
    elapsed_process_time.record_value(stats.elapsed_process_time);
    elapsed_process_time.record_value(stats.elapsed_process_time);
    normal_iterations.record_value(stats.normal_iterations);
    biconf_iterations.record_value(stats.biconf_iterations);
    ask_parity_messages.record_value(stats.ask_parity_messages);
    ask_parity_blocks.record_value(stats.ask_parity_blocks);
    ask_parity_bits.record_value(stats.ask_parity_bits);
    reply_parity_bits.record_value(stats.reply_parity_bits);
    unrealistic_efficiency.record_value(stats.unrealistic_efficiency);
    realistic_efficiency.record_value(stats.realistic_efficiency);
    infer_parity_blocks.record_value(stats.infer_parity_blocks);
}

std::string DataPoint::to_json() const
{
    std::string json = "{";

    // Actual bit error rate
    json += "\"actual_bit_error_rate\": " + actual_bit_error_rate.to_json() + ", ";

    // Actual bit errors
    json += "\"actual_bit_errors\": " + actual_bit_errors.to_json() + ", ";

    // Algorithm name
    json += "\"algorithm_name\": \"" + algorithm_name + "\", ";

    // Ask parity bits
    json += "\"ask_parity_bits\": " + ask_parity_bits.to_json() + ", ";

    // Ask parity blocks
    json += "\"ask_parity_blocks\": " + ask_parity_blocks.to_json() + ", ";

    // Ask parity messages
    json += "\"ask_parity_messages\": " + ask_parity_messages.to_json() + ", ";

    // BICONF iterations
    json += "\"biconf_iterations\": " + biconf_iterations.to_json() + ", ";

    // Execution time: the date and time at which the data point was produced, i.e. now.
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream now_str;
    now_str << std::put_time(&tm, "%Y-%m-%d %H:%M:%S %Z");
    json += "\"execution_time\": \"" + now_str.str() + "\", ";

    // Infer parity blocks
    json += "\"infer_parity_blocks\": " + infer_parity_blocks.to_json() + ", ";

    // Key size
    json += "\"key_size\": \"" + std::to_string(key_size) + "\", ";

    // Normal iterations
    json += "\"normal_iterations\": " + normal_iterations.to_json() + ", ";

    // Realistic efficiency
    json += "\"realistic_efficiency\": " + realistic_efficiency.to_json() + ", ";

    // Reconciliations
    json += "\"reconciliations\": \"" + std::to_string(reconciliations) + "\", ";

    // Remaining bit error rate
    json += "\"remaining_bit_error_rate\": " + remaining_bit_error_rate.to_json() + ", ";

    // Remaining bit errors
    json += "\"remaining_bit_errors\": " + remaining_bit_errors.to_json() + ", ";

    // Remaining frame error rate
    json += "\"remaining_frame_error_rate\": " + remaining_frame_error_rate.to_json() + ", ";

    // Reply parity bits
    json += "\"reply_parity_bits\": " + reply_parity_bits.to_json() + ", ";

    // Requested bit error rate
    json += "\"requested_bit_error_rate\": \"" + std::to_string(requested_bit_error_rate) + "\", ";

    // Unrealistic efficiency
    json += "\"unrealistic_efficiency\": " + unrealistic_efficiency.to_json();

    json += "}";
    return json;
}
