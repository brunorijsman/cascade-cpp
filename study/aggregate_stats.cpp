#include "aggregate_stats.h"

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
