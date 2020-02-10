#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG(msg) std::cerr << msg << std::endl
#else
#define DEBUG(msg)
#endif

#endif /* ifndef DEBUG_H */
