#ifndef AGGREGATE_STATS_H
#define AGGREGATE_STATS_H

#include <string>

class AggregateStats
{
public:
    AggregateStats();
    void record_value(double value);
    double average() const;
    double deviation() const;
    std::string to_json() const;
private:
    long count;
    double sum;
    double square_sum;
};

#endif /* ifndef AGGREGATE_STATS_H */
