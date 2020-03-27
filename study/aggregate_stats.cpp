#include "aggregate_stats.h"
#include <cmath>
#include <limits>

AggregateStats::AggregateStats():
    count(0),
    sum(0.0),
    square_sum(0.0)
{
}

void AggregateStats::record_value(double value)
{
    count += 1;
    sum += value;
    square_sum += value * value;
}

double AggregateStats::average() const
{
    if (count == 0)
        return std::numeric_limits<double>::quiet_NaN();
    return sum / count;
}

double AggregateStats::deviation() const
{
    // Compute the corrected standard deviation. 
    // See https://en.wikipedia.org/wiki/Bessel%27s_correction.
    if (count < 2)
        return std::numeric_limits<double>::quiet_NaN();
    double variance = square_sum / (count - 1);
    variance -= (sum * sum) / ((count - 1) * count);
    // Variance can up being some vary small negative number due to rounding errors
    if (variance <= 0.0)
        variance = 0.0;
    double deviation = sqrt(variance);
    return deviation;
}

std::string AggregateStats::to_json() const
{
    std::string json = "{";
    double avg = average();
    std::string avg_str;
    if (isnan(avg))
        avg_str = "NaN";    // Python program to produce graphs insists on capital N's
    else
        avg_str = std::to_string(average());
    json += "\"average\": " + avg_str + ", ";
    double dev = deviation();
    std::string dev_str;
    if (isnan(dev))
        dev_str = "NaN";    // Python program to produce graphs insists on capital N's
    else
        dev_str = std::to_string(average());
    json += "\"deviation\": " + dev_str;
    json += "}";
    return json;
}
