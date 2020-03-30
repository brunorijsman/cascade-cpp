#ifndef DEBUG_H
#define DEBUG_H

#include "report.h"

namespace Cascade {

#define ANSI_BLACK  "\u001b[30m"
#define ANSI_RED    "\u001b[31m"
#define ANSI_GREEN  "\u001b[32m"
#define ANSI_BLUE   "\u001b[34m"
#define ANSI_RESET  "\u001b[0m"

#ifdef ENABLE_DEBUG
#include <report.h>
#define DEBUG(msg) REPORT(msg)
#else
#define DEBUG(msg)
#endif

} /* namespace Cascade */

#endif /* ifndef DEBUG_H */
