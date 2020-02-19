#ifndef STATS_H
#define STATS_H

namespace Cascade {

class Stats
{
public:
    Stats();
    double elapsed_process_time;
    double elapsed_real_time;
    int normal_iterations;
    int biconf_iterations;
};

} /* namespace Cascade */

#endif /* ifndef STATS_H */
