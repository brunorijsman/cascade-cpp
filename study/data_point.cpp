#include "data_point.h"

DataPoint::DataPoint(const std::string& algorithm_name, int key_size,
                     double requested_bit_error_rate):
    algorithm_name(algorithm_name),
    key_size(key_size),
    requested_bit_error_rate(requested_bit_error_rate)
{
}

std::string DataPoint::to_json() const
{
    std::string json = "{";
    json += "\"execution_time\": \"" + execution_time + "\",";
    // TODO: add all other fields
    json += "}";
    return json;
}
