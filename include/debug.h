#ifndef DEBUG_H
#define DEBUG_H

// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#include <report.h>
#define DEBUG(msg) REPORT(msg)
#else
#define DEBUG(msg)
#endif

#endif /* ifndef DEBUG_H */
