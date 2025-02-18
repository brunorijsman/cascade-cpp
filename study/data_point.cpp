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
    elapsed_real_time.record_value(stats.elapsed_real_time);
    normal_iterations.record_value(stats.normal_iterations);
    biconf_iterations.record_value(stats.biconf_iterations);
    ask_parity_messages.record_value(stats.ask_parity_messages);
    ask_parity_blocks.record_value(stats.ask_parity_blocks);
    ask_parity_bits.record_value(stats.ask_parity_bits);
    reply_parity_bits.record_value(stats.reply_parity_bits);
    efficiency.record_value(stats.efficiency);
    reconciliation_bits.record_value(stats.reconciliation_bits);
    reconciliation_bits_per_key_bit.record_value(stats.reconciliation_bits_per_key_bit);
    infer_parity_blocks.record_value(stats.infer_parity_blocks);
}

std::string DataPoint::to_json() const
{
    std::string json = "{";

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream now_str;
    now_str << std::put_time(&tm, "%Y-%m-%d %H:%M:%S %Z");

    json += "\"actual_bit_error_rate\": " + actual_bit_error_rate.to_json() + ", ";
    json += "\"actual_bit_errors\": " + actual_bit_errors.to_json() + ", ";
    json += "\"algorithm_name\": \"" + algorithm_name + "\", ";
    json += "\"ask_parity_bits\": " + ask_parity_bits.to_json() + ", ";
    json += "\"ask_parity_blocks\": " + ask_parity_blocks.to_json() + ", ";
    json += "\"ask_parity_messages\": " + ask_parity_messages.to_json() + ", ";
    json += "\"biconf_iterations\": " + biconf_iterations.to_json() + ", ";
    json += "\"elapsed_process_time\": " + elapsed_process_time.to_json() + ", ";
    json += "\"elapsed_real_time\": " + elapsed_real_time.to_json() + ", ";
    json += "\"execution_time\": \"" + now_str.str() + "\", ";
    json += "\"infer_parity_blocks\": " + infer_parity_blocks.to_json() + ", ";
    json += "\"key_size\": " + std::to_string(key_size) + ", ";
    json += "\"normal_iterations\": " + normal_iterations.to_json() + ", ";
    json += "\"reconciliation_bits\": " + reconciliation_bits.to_json() + ", ";
    json += "\"reconciliation_bits_per_key_bit\": " + reconciliation_bits_per_key_bit.to_json() + ", ";
    json += "\"reconciliations\": " + std::to_string(reconciliations) + ", ";
    json += "\"remaining_bit_error_rate\": " + remaining_bit_error_rate.to_json() + ", ";
    json += "\"remaining_bit_errors\": " + remaining_bit_errors.to_json() + ", ";
    json += "\"remaining_frame_error_rate\": " + remaining_frame_error_rate.to_json() + ", ";
    json += "\"reply_parity_bits\": " + reply_parity_bits.to_json() + ", ";
    json += "\"requested_bit_error_rate\": " + std::to_string(requested_bit_error_rate) + ", ";
    json += "\"efficiency\": " + efficiency.to_json();
    json += "}";
    return json;
}
