#ifndef REPORT_H
#define REPORT_H

#include <iostream>
#include <mutex>
#include <thread>

extern std::mutex report_mutex;

#define REPORT(msg) \
    do { \
        std::lock_guard<std::mutex> guard(report_mutex); \
        std::cerr << msg << std::endl; \
    } while (false)

#endif /* ifndef REPORT_H */
