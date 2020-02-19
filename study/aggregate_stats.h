#ifndef AGGREGATE_STATS_H
#define AGGREGATE_STATS_H

template <typename ValueType>
class AggregateStats
{
public:
    AggregateStats<ValueType>();
    void record_value(ValueType value);
private:
    long count;
    ValueType sum;
    ValueType square_sum;
};

#endif /* ifndef AGGREGATE_STATS_H */
