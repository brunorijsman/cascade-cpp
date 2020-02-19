#ifndef AGGREGATE_STATS_H
#define AGGREGATE_STATS_H

class AggregateStats
{
public:
    AggregateStats();
    void record_value(double value);
private:
    long count;
    double sum;
    double square_sum;
};

#endif /* ifndef AGGREGATE_STATS_H */
