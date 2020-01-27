#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include "key.h"
#include "iteration.h"
#include <map>
#include <string>
#include <vector>

namespace Cascade {

class Algorithm;

class Reconciliation
{
public:
    Reconciliation(std::string algorithm_name,
                   const Key& noisy_key,
                   double estimated_bit_error_rate);
    Key& get_reconciled_key(void);
    const Algorithm& get_algorithm(void) const;
    void reconcile(void);
private:
    const Algorithm* algorithm;
    const Key& noisy_key;
    double estimated_bit_error_rate;
    Key reconciled_key;
    std::vector<Iteration> iterations;
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
