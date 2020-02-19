#include "aggregate_stats.h"

template <typename ValueType>
AggregateStats<ValueType>::AggregateStats():
    count(0),
    sum(0),
    square_sum(0)
{
}

template <typename ValueType>
void AggregateStats<ValueType>::record_value(ValueType value)
{
    count += 1;
    sum += value;
    square_sum += value * value;
}
